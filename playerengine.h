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

#ifndef PLAYERENGINE_H
#define PLAYERENGINE_H

#include <QObject>
#include "player.h"
#include "playlist.h"
#include "tpplaylistrunner.h"

// All the supported player commands listed here.
static const QString playerCmdPlay("play");
static const QString playerCmdPause("pause");
static const QString playerCmdStop("stop");
static const QString playerCmdNext("next");
static const QString playerCmdMute("mute");
static const QString playerCmdUnmute("unmute");
static const QString playerCmdSeek("seek");

class TPPlayerProxy : public QObject
{
    Q_OBJECT
public:
    explicit TPPlayerProxy(PlayerBackend_MPlayer *_player, QObject *parent = 0);
    ~TPPlayerProxy();

signals:

    void currentTrackChanged(TPTrack *track);
    void currentPlaybackPositionChanged(TPTrack *track, int seconds);
    void playbackOperationsChanged();
    void activePlaylistChanged();
    void volumeChanged(int percents);

public slots:

    void setMode(const QString mode)
    {
        playlistRunner->setMode(mode);
    }

    const QString getMode() const
    {
        return playlistRunner->getMode();
    }

    bool Execute(const QString &cmd, bool ignoreCheck = false);
    bool CanExecute(const QString &cmd);
    //! Sets a playlist. Ownership of the playlist changes!
    void setPlaylist(TPPlaylist *_playlist);
    TPPlaylist *getPlaylist();

    inline TPTrack* getCurrentTrack() const
    {
        return playlistRunner->getCurrentTrack();
    }

    inline bool seekToPosition(int seconds)
    {
        Q_ASSERT(player);

        return player->seekTo(seconds);
    }

    bool seekToTrack(const TPTrack *track);

private:

private slots:

    void playerError(int errorCode);
    void playerStateChanged(PlayerBackend_MPlayer::State state);
    void playbackPositionChanged(int secs);
    void playerMutingChanged(bool muted);
    void playbackStreamTitleChanged(const QString streamTitle);
    void playbackLength(int seconds);

private:

    TPPlaylistRunner *playlistRunner;
    PlayerBackend_MPlayer *player;
    int playIdx;
};

#endif // PLAYERENGINE_H
