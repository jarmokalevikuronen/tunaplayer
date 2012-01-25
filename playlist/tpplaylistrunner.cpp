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

#include "tpplaylistrunner.h"

//
// TODO: Modify so that currentTrack also affects on playlist stuff.
//

// In this mode -> the playlist is continously repeated
static const QString modeRepeat = "repeat";
// In this mode -> the playlist is played exactly once and
// after playing, the tracks are removed from the playlist
static const QString modeQueue = "queue";
// In this mode -> the playlist is gone through once but the
// tracks are still kept on the playlist.
static const QString modeOnce = "once";

TPPlaylistRunner::TPPlaylistRunner()
{
    playlist = NULL;
    currentTrack = NULL;
    mode = modeQueue;
}

TPPlaylistRunner::~TPPlaylistRunner()
{
    DEBUG() << "PLAYLIST: ~TPPlaylistRunner";
    reset();
}

void TPPlaylistRunner::setMode(const QString _mode)
{
    mode = _mode;

    for (int i=0;i<playedTracks.count();++i)
        playedTracks.at(i)->dec();
    playedTracks.clear();
}

TPTrack* TPPlaylistRunner::processNextTrack(TPTrack *track)
{
    if (mode == modeOnce && track)
    {
        if (playedTracks.contains(track))
            // Already played -> do return null instead of valid track since we met
            // an already played track.
            return NULL;

        track->inc();
        playedTracks.append(track);
    }

    return track;
}

TPTrack* TPPlaylistRunner::nextTrack()
{
    return nextTrackInternal(NULL);
}

TPTrack* TPPlaylistRunner::nextTrackInternal(bool *peek)
{
    if (!playlist || !playlist->count())
    {
        if (peek)
            *peek = false;
        else
        {
            // Get rid of empty playlist if exists.
            if (playlist)
                playlist->dec();
            playlist = NULL;
        }

        return NULL;
    }

    if (mode == modeOnce)
        return nextTrackOnce(peek);
    else if (mode == modeQueue)
        return nextTrackQueue(peek);
    else if (mode == modeRepeat)
        return nextTrackRepeat(peek);

    if (peek)
        *peek = false;

    return NULL;
}

TPTrack* TPPlaylistRunner::nextTrackOnce(bool *peek)
{
    // Unref possible old track if any.
    if (currentTrack && !peek)
    {
        currentTrack->dec();
        currentTrack = NULL;
    }

    // TODO: This behaves really poorly with very
    // large playlists. Add some index where to start the searching maybe.
    // Yet still one needs to deal with the fact that playlist content might
    // change before that index and thus the behavioral might not be wanted after all.

    // Start from the beginning of the playlist
    // and take the first track that we have not yet played.
    for (int i=0;i<playlist->count();++i)
    {
        TPTrack *t = playlist->at(i);

        if (playedTracks.indexOf(t) < 0)
        {
            if (peek)
            {
                *peek = true;
                return NULL;
            }

            currentTrack = t;
            currentTrack->inc();

            return processNextTrack(currentTrack);
        }
    }

    if (peek)
        *peek = false;

    // No more tracks.
    return NULL;
}

TPTrack* TPPlaylistRunner::nextTrackQueue(bool *peek)
{
    if (currentTrack && !peek)
    {
        currentTrack->dec();
        currentTrack = NULL;
    }

    if (!peek)
    {
        if (playlist)
            currentTrack = playlist->takeNext();
        else
            return NULL;

        if (currentTrack)
            currentTrack->inc();
        else
        {
            if (playlist)
            {
                playlist->dec();
                playlist = NULL;
            }
        }

        return processNextTrack(currentTrack);
    }

    *peek = playlist ? playlist->count() > 0 : false;

    return NULL; // Should not make a difference once peeking.
}

TPTrack* TPPlaylistRunner::nextTrackRepeat(bool *peek)
{
    if (!currentTrack)
    {
        if (peek)
        {
            *peek = true;
            return NULL;
        }
        else
        {
            currentTrack = playlist->at(0);
            if (currentTrack)
                currentTrack->inc();
        }
    }
    else
    {
        if (peek)
        {
            *peek = true;
            return NULL;
        }

        int idx = playlist->indexOf(currentTrack) + 1;
        if (idx >= playlist->count())
            idx = 0;

        currentTrack->dec();
        currentTrack = playlist->at(idx);
        if (currentTrack)
            currentTrack->inc();
    }

    return processNextTrack(currentTrack);
}

TPTrack* TPPlaylistRunner::seekToTrack(const TPTrack *track)
{
    if (track == currentTrack)
        return NULL;

    int index = playlist->indexOf(const_cast<TPTrack*>(track));
    if (index < 0)
        return NULL;

    if (currentTrack)
        currentTrack->dec();
    currentTrack = playlist->at(index);
    currentTrack->inc();

    return processNextTrack(currentTrack);
}

bool TPPlaylistRunner::hasNextTrack()
{
    bool result = false;
    nextTrackInternal(&result);
    return result;
}

