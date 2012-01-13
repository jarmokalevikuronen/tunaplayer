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

#ifndef TPPLAYLISTRUNNER_H
#define TPPLAYLISTRUNNER_H

#include "playlist.h"

class TPPlaylistRunner
{
public:

    TPPlaylistRunner();
    ~TPPlaylistRunner();

    // @brief sets the playlist runner mode if any.
    // @param _mode "once", "repeat", "queue"
    void setMode(const QString _mode);

    inline const QString getMode() const
    {
        return mode;
    }

    inline TPTrack* setPlaylist(TPPlaylist *_playlist)
    {
        if (playlist)
        {
            playlist->dec();
            playlist = NULL;
            reset();
        }

        playlist = _playlist;
        if (playlist)
            playlist->inc();

        return nextTrack();
    }

    inline void currentTrackEnded()
    {
        if (currentTrack)
        {
            currentTrack->dec();
            currentTrack = NULL;
        }
    }

    inline TPPlaylist* getPlaylist() const
    {
        return playlist;
    }

    inline TPTrack* getCurrentTrack() const
    {
        return currentTrack;
    }

    //! @brief Takes next track from the playlist
    //! @return track or null in case no more tracks to provide.
    TPTrack* nextTrack();

    bool hasNextTrack();

    TPTrack* seekToTrack(const TPTrack *track);

    void reset()
    {
        for (int i=0;i<playedTracks.count();++i)
            playedTracks.at(i)->dec();

        playedTracks.clear();
        if (currentTrack)
            currentTrack->dec();
        currentTrack = NULL;
    }

private:

    TPTrack* nextTrackInternal(bool *peek);
    TPTrack* processNextTrack(TPTrack *track);
    TPTrack* nextTrackOnce(bool *peek);
    TPTrack* nextTrackQueue(bool *peek);
    TPTrack* nextTrackRepeat(bool *peek);

private: // Data

    //! Current operation mode if any.
    QString mode;
    TPPlaylist *playlist;

    //! Here, we will effectively follow the tracks.
    QVector<TPTrack*> playedTracks;

    //! Last returned track, in order to detect the next one also.
    TPTrack* currentTrack;
};

#endif // TPPLAYLISTRUNNER_H
