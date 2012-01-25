/*
This file is part of tunaplayer project
Copyright (C) 2012  Jarmo Kuronen <jarmok@iki.fi>

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "player.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QFile>
#include "tpsettings.h"
#include "tplog.h"

PlayerBackend_MPlayer::PlayerBackend_MPlayer(QObject *parent) :
    QObject(parent)
{
    // Connect to process signals
    QObject::connect(&process, SIGNAL(error(QProcess::ProcessError)), SLOT(processError(QProcess::ProcessError)));
    QObject::connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(processFinished(int,QProcess::ExitStatus)));
    QObject::connect(&process, SIGNAL(readyReadStandardOutput()), SLOT(processHasStdoutData()));
    QObject::connect(&process, SIGNAL(readyReadStandardError()), SLOT(processHasStdoutData()));

    // Connecto to watchdog signals
    QObject::connect(&watchdog, SIGNAL(timeout()), this, SLOT(runWatchdog()));
    watchdog.setSingleShot(true);

    playbackPosition = -1;
    muted = false;
    ignoreStateChange = false;
    playbackLengthReported = false;
    state = PlayerBackend_MPlayer::Stopped;
}

PlayerBackend_MPlayer::~PlayerBackend_MPlayer()
{
    disconnect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    disconnect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    disconnect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(processHasStdoutData()));
    disconnect(&process, SIGNAL(readyReadStandardError()), this, SLOT(processHasStdoutData()));

    disconnect(&watchdog, SIGNAL(timeout()), this, SLOT(runWatchdog()));

    process.terminate();
    process.close();
}

// Occasionally mplayer adds some "crab" to the beginning
// of the command -> get rid of it in order to detect the actual
// states/commands we are about to handle.
QString skipNonControls(const QString input)
{
    int startAt = 0;

    while (startAt < input.length())
    {
        QChar ch = input.at(startAt);
        if (ch.isLetterOrNumber() || ch == '=')
            return input.mid(startAt);

        ++startAt;
    }

    return QString();
}

void PlayerBackend_MPlayer::processOutput(QString output)
{
    bool updatePosition = false;

    output = skipNonControls(output);

    if (output.length() >= 10)
    {

#define ICY_INFO_PREFIX "ICY Info: StreamTitle="

        if (output.contains(ICY_INFO_PREFIX))
        {
            int pos = output.indexOf(ICY_INFO_PREFIX);
            QString remainder = output.mid(pos + strlen(ICY_INFO_PREFIX));
            if (remainder.length() && remainder.at(0) == '\'')
            {
                remainder = remainder.mid(1);
                int endpos = remainder.indexOf(QChar('\''));
                if (endpos > 0)
                {
                    QString title = remainder.left(endpos).trimmed();
                    setNewStreamTitle(title);
                }
            }
        }

        if (output.contains("Mute: enabled"))
        {
            if (!muted)
            {
                muted = true;
                emit playerMutingChanged(muted);
            }
        }
        else if (output.contains("Mute: disabled"))
        {
            if (muted)
            {
                muted = false;
                emit playerMutingChanged(muted);
            }
        }
        else if (output.contains("=====  PAUSE  ====="))
        {
            STATE() << "MPLAYER: PAUSED";
            changeToState(Paused);
        }
        else if (output.startsWith("A:"))
        {
            bool ok = false;
            QString secsStart = output.mid(2).trimmed();
            QString secsOnly;
            int index = 0;
            while (index < secsStart.length( )&& secsStart.at(index).isDigit())
                secsOnly.append(secsStart.at(index++));

            int secs = secsOnly.toInt(&ok);
            if (ok)
            {
                // Parse the total length also.
                if (!playbackLengthReported)
                {
                    ok = false;
                    QString totalLenId = " of ";
                    int totalLenStartIndex = output.indexOf(totalLenId);
                    if (totalLenStartIndex > 0)
                    {
                        QString totalLenStart = output.mid(totalLenStartIndex + totalLenId.length());
                        QString totalLenOnly;
                        int index = 0;
                        while (index < totalLenStart.length() && totalLenStart.at(index).isDigit())
                            totalLenOnly.append(totalLenStart.at(index++));

                        int totalLen = totalLenOnly.toInt(&ok);
                        if (ok)
                        {
                            emit playbackLength(totalLen);
                            playbackLengthReported = true;
                        }
                    }
                }
                changeToState(Playing);

                setNewPlaybackPosition((int)secs);
                updatePosition = true;
            }
        }
        else if (output.contains("Playing "))
        {
            changeToState(Playing);
        }
        else
        {
        }
    }
}

void PlayerBackend_MPlayer::setNewPlaybackPosition(int position)
{
    if (position != playbackPosition)
        emit playbackPositionChanged(playbackPosition = position);
}

void PlayerBackend_MPlayer::setNewStreamTitle(const QString title)
{
    if (title != streamTitle)
        emit playbackStreamTitleChanged(streamTitle = title);
}

void PlayerBackend_MPlayer::processHasStdoutData()
{
    kickWatchdog();

    QString output = process.readAllStandardError();
    output += process.readAllStandardOutput();
    processOutput(output.trimmed());
}

void PlayerBackend_MPlayer::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    stopWatchdog();

    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    changeToState(Playing);
    changeToState(Stopped);
}

void PlayerBackend_MPlayer::processError(QProcess::ProcessError err)
{
    stopWatchdog();

    ERROR() << "MPLAYER: processError: " << err;
    if (state != Paused)
        stop(false);
}

bool PlayerBackend_MPlayer::play(TPTrack *track)
{
    bool status = true;

    if (!canPlay())
        status = false;
    else
    {
        process.terminate();
        process.waitForFinished(3000);
        process.close();

        playbackLengthReported = track->getLen() > 0;
        QString filename;
        filename = track->getFilename();
        QString cli;

        if (filename.startsWith("http://", Qt::CaseInsensitive) &&
                (
                filename.endsWith(".m3u", Qt::CaseInsensitive) ||
                filename.endsWith(".pls", Qt::CaseInsensitive) ||
                filename.endsWith(".asx", Qt::CaseInsensitive)
                )
            )
            cli = TPSettings::instance().get(settingPlayPlaylistCmd).toString().arg(filename);
        else
            cli = TPSettings::instance().get(settingPlayTrackCmd).toString().arg(filename);

        DEBUG() << "PLAYER: executing cli " << cli;
        process.start(cli);
        status = process.waitForStarted(5000);

        if (status)
            kickWatchdog();
        else
            ERROR() << "PLAYER: Failed to start playback\n";

        if (status && muted)
            mute(true);
    }

    return status;
}

bool PlayerBackend_MPlayer::pause()
{
    bool status = true;

    if (!canPause())
        status = false;
    else
    {
        process.write("\np\n");
        process.waitForBytesWritten(1000);
    }

    return status;
}

bool PlayerBackend_MPlayer::resume()
{
    bool status = true;

    if (!canResume())
        status = false;
    else
    {
        process.write("\np\n");
        process.waitForBytesWritten(1000);
    }

    return status;
}

bool PlayerBackend_MPlayer::stop(bool noSignal)
{
    bool status = canStop();

    if (status)
    {
        ignoreStateChange = noSignal;

        process.write("q\n");
        if (!process.waitForFinished(500))
        {
            process.terminate();
            process.waitForFinished(1500);
        }
        changeToState(Stopped);

        ignoreStateChange = false;
    }

    return status;
}

bool PlayerBackend_MPlayer::mute(bool ignoreCheck)
{
    bool status = ignoreCheck || canMute();

    if (status)
    {
        process.write(" \nmute 1\n");
        process.waitForBytesWritten(1000);
    }

    return status;
}

bool PlayerBackend_MPlayer::unmute()
{
    bool status = canUnmute();

    if (status)
    {
        process.write(" \nmute 0\n");
        process.waitForBytesWritten(1000);
    }

    return status;
}


bool PlayerBackend_MPlayer::seekTo(int seconds)
{
    DEBUG() << "MPLAYER: seekTo: " << seconds;

    if (state == Stopped)
        return false;

    if (seconds < 0)
        seconds = 0;

    QString cmd("seek ");
    cmd.append(QString::number(seconds));
    cmd.append(" 2\n");

    process.write(cmd.toAscii());
    process.waitForBytesWritten(1000);

    return true;
}

void PlayerBackend_MPlayer::changeToState(State _state)
{
    if (state != _state)
    {
        STATE() << "MPLAYER: " << state << " -> " << _state;
        state = _state;
        if (!ignoreStateChange)
            emit playerStateChanged(state);
    }
}

void PlayerBackend_MPlayer::kickWatchdog()
{
    watchdog.stop();
    watchdog.start(1000 * 15); // 15s watchdog timer.
}

void PlayerBackend_MPlayer::runWatchdog()
{
    ERROR() << "PLAYER: Watchdog expired. Stopping player process.";

    process.terminate();
    process.waitForFinished(3000);
    process.close();
    changeToState(Playing);
    changeToState(Stopped);
}

void PlayerBackend_MPlayer::stopWatchdog()
{
    watchdog.stop();
}

