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

#include <QDebug>
#include "playerengine.h"
#include "tplog.h"

TPPlayerProxy::TPPlayerProxy(PlayerBackend_MPlayer *_player, QObject *parent) :
    QObject(parent), player(_player)
{
    Q_ASSERT(player);

    playlistRunner = new TPPlaylistRunner;
    Q_ASSERT(playlistRunner);

    connect(player, SIGNAL(playbackPositionChanged(int)), this, SLOT(playbackPositionChanged(int)));
    connect(player, SIGNAL(playbackStreamTitleChanged(QString)), this, SLOT(playbackStreamTitleChanged(QString)));
    connect(player, SIGNAL(playerStateChanged(PlayerBackend_MPlayer::State)), this, SLOT(playerStateChanged(PlayerBackend_MPlayer::State)));
    connect(player, SIGNAL(playerError(int)), this, SLOT(playerError(int)));    
    connect(player, SIGNAL(playbackLength(int)), this, SLOT(playbackLength(int)));
    connect(player, SIGNAL(playerMutingChanged(bool)), this, SLOT(playerMutingChanged(bool)));
}

TPPlayerProxy::~TPPlayerProxy()
{
    delete playlistRunner;
}

bool TPPlayerProxy::Execute(const QString &command, bool ignoreCheck)
{
    bool result = false;

    if (!ignoreCheck && !CanExecute(command))
    {
        emit playbackOperationsChanged();
        return (false);
    }

    if (command == playerCmdMute)
    {
        if (player->canMute())
            result = player->mute();
    }
    else if (command == playerCmdUnmute)
    {
        if (player->canUnmute())
            result = player->unmute();
    }
    else if (command == playerCmdPlay)
    {
        if (player->canResume())
            result = player->resume();
        else if (player->canPlay())
            result = player->play(playlistRunner->getCurrentTrack());
    }
    else if (command == playerCmdStop)
    {
        result = player->stop(true);
        if (result)
            emit playbackOperationsChanged();
    }
    else if (command == playerCmdPause)
    {
        result = player->pause();
    }
    else if (command == playerCmdNext)
    {
        TPTrack *nextTrack = playlistRunner->nextTrack();

        if (nextTrack)
        {
            player->stop(true);
            result = player->play(nextTrack);
        }
        else
        {
            emit currentTrackChanged(NULL);
            emit playbackOperationsChanged();
        }
    }
    else if (command.startsWith(playerCmdSeek))
    {
        QString seekWithDelimiter(playerCmdSeek + ":");

        QString remainder = command.mid(seekWithDelimiter.length());
        if (!remainder.length())
        {
            ERROR() << "PLAYER: Invalid seek command";
            return false;
        }

        DEBUG() << "PLAYER: seek instruction: " << remainder;

        int position = 0;
        if (remainder.endsWith('%'))
        {
            int percents = remainder.mid(0, remainder.length()-1).toInt();
            DEBUG() << "PLAYER: Seek Percents: " << percents;
            // range to 1..99
            percents = qMin(99, percents);
            percents = qMax(1, percents);

            if (!playlistRunner->getCurrentTrack())
                return false;

            int len = playlistRunner->getCurrentTrack()->getLen();
            if (len < 1)
                return false;

            position = len * percents;
            position /= 100;
            DEBUG() << "PLAYER: seek percents " << percents << " translated to seconds " << position;
        }
        else
            position = remainder.toInt();

        if (position < 0)
            position = 0;
        result = seekToPosition(position);
    }
    else
    {
        ERROR() << "PLAYER: Unsupported command: " << command;
    }

    return result;
}

bool TPPlayerProxy::CanExecute(const QString &cmd)
{
    bool can = false;

    if (cmd == playerCmdSeek)
        can = playlistRunner->getCurrentTrack() && player->canStop();
    else if (cmd == playerCmdMute)
        can = player->canMute() && player->canStop();
    else if (cmd == playerCmdUnmute)
        can = player->canUnmute() && player->canStop();
    else if (cmd == playerCmdPlay)
         can = (playlistRunner->getCurrentTrack() && (player->canPlay() || player->canResume()));
    else if (cmd == playerCmdStop)
         can = playlistRunner->getCurrentTrack() && player->canStop();
    else if (cmd == playerCmdNext)
        can = !player->canStop() ? false : playlistRunner->hasNextTrack();
    else if (cmd == playerCmdPause)
        can = playlistRunner->getCurrentTrack() && player->canPause();
    else if (cmd.startsWith(playerCmdSeek))
        can = player->canPause() && player->canStop();
    else
        qDebug() << "PLAYER: Unsupported command: " << cmd;

    qDebug() << "PLAYBACK: CONTROL: " << cmd << (can ? " ENABLED" : "DISABLED");

    return can;
}

void TPPlayerProxy::setPlaylist(TPPlaylist *_playlist)
{
    Execute(playerCmdStop);

    TPTrack* track = playlistRunner->setPlaylist(_playlist);
    emit currentTrackChanged(track);
    emit activePlaylistChanged();
}

TPPlaylist *TPPlayerProxy::getPlaylist()
{
    return playlistRunner->getPlaylist();
}

void TPPlayerProxy::playerError(int errorCode)
{
    qDebug() << "playerError: " << errorCode;
}

void TPPlayerProxy::playerStateChanged(PlayerBackend_MPlayer::State state)
{
    switch (state)
    {
    case PlayerBackend_MPlayer::Stopped:
        qDebug() << "PlayerEngine::Player::state = Stopped";
        playlistRunner->currentTrackEnded();
        Execute(playerCmdNext, true);
        break;

    case PlayerBackend_MPlayer::Paused:
        qDebug() << "PlayerEngine::Player::state = Paused";
        emit playbackOperationsChanged();
        break;

    case PlayerBackend_MPlayer::Playing:
    {
        TPTrack *currentTrack = playlistRunner->getCurrentTrack();

        if (currentTrack)
        {
            qDebug() << "PlayerEngine::Player::state = Playing";
            emit currentTrackChanged(currentTrack);
            emit playbackOperationsChanged();
        }
    }
    break;

    default:
        qDebug() << "ERROR: unsupported player state: " << state;
    }
}

void TPPlayerProxy::playbackPositionChanged(int secs)
{
    TPTrack *currentTrack = playlistRunner->getCurrentTrack();

    if (currentTrack)
    {
//        qDebug() << "playbackPositionChanged: " << secs << "/" << currentTrack->getLen();
        emit currentPlaybackPositionChanged(currentTrack, secs);
    }
}

void TPPlayerProxy::playerMutingChanged(bool muted)
{
    Q_UNUSED(muted)
    emit playbackOperationsChanged();
}

void TPPlayerProxy::playbackStreamTitleChanged(const QString streamTitle)
{
    qDebug() << "playbackStreamTitleChanged: " << streamTitle;
}

bool TPPlayerProxy::seekToTrack(const TPTrack *track)
{
    bool status = false;
    TPTrack *t = playlistRunner->seekToTrack(track);

    if (t)
    {
        status = Execute(playerCmdStop) && Execute(playerCmdPlay);
        if (status)
        {
            emit currentTrackChanged(t);
            emit activePlaylistChanged();
        }
    }
    return status;
}


void TPPlayerProxy::playbackLength(int seconds)
{
    TPTrack *currentTrack = playlistRunner->getCurrentTrack();

    if (currentTrack)
        currentTrack->setLen(seconds);
}
