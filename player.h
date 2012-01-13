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

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QProcess>
#include "tptrack.h"


//! @class Player
//! @brief Player backend for MPlayer.

//
// class PlayerImpl<MPlayer_Backend> {
// backed->stop()
// };
//
//
//

class PlayerBackend_MPlayer : public QObject
{
    Q_OBJECT

public:
    explicit PlayerBackend_MPlayer(QObject *parent = 0);
    ~PlayerBackend_MPlayer();

    enum State {
        Stopped,
        Paused,
        Playing
    };

    inline PlayerBackend_MPlayer::State getState() const
    {
        return state;
    }

    inline bool canPlay() const
    {
        return state == Stopped || state == Paused;
    }

    inline bool canStop() const
    {
        return state == Playing || state == Paused;
    }

    inline bool canPause() const
    {
        return state == Playing;
    }

    inline bool canResume() const
    {
        return state == Paused;
    }

    inline int getPlaybackPosition() const
    {
        return playbackPosition;
    }

    inline bool canMute() const
    {
        return state == Playing && !muted;
    }

    inline bool canUnmute() const
    {
        return state == Playing && muted;
    }

signals:

    void playerError(int errorCode);
    void playerStateChanged(PlayerBackend_MPlayer::State state);
    void playbackPositionChanged(int secs);
    void playbackLength(int secs);
    void playbackStreamTitleChanged(const QString streamTitle);
    void playerMutingChanged(bool muted);

private slots:

    void processError(QProcess::ProcessError err);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processHasStdoutData();

public:

    bool seekTo(int seconds);
    bool play(TPTrack *track);
    bool pause();
    bool resume();
    bool stop(bool noSignal=true);
    bool mute(bool ignoreTest = false);
    bool unmute();

private:

    void changeToState(State _state);
    void processOutput(QString );
    void setNewPlaybackPosition(int position);
    void setNewStreamTitle(const QString title);

private:

    QProcess process;
    State state;
    QString streamTitle;
    int playbackPosition;
    bool muted;
    bool ignoreStateChange;
    bool playbackLengthReported;
};

#endif // PLAYER_H
