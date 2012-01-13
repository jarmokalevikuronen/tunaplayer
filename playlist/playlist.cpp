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

#include "playlist.h"
#include <QtGlobal>
#include <QTime>

TPPlaylist::TPPlaylist(const QString _name, TPAssociativeDB *db, const QString _filename)
    : TPIdBase(schemePlaylist, _filename), TPAssociativeObject(db->allocItem(_filename))
{
    setString(objectAttrName, _name);
    setString(playlistAttrFilename, _filename);
    setString(objectAttrScheme, schemePlaylist);
    clonedFrom = NULL;
}

TPPlaylist::TPPlaylist() : TPIdBase(schemePlaylist, "currentlyplayed")
{
    setString(objectAttrName, "(Not Saved)");
    clonedFrom = NULL;
}

TPPlaylist::~TPPlaylist()
{
    if (clonedFrom)
        clonedFrom->dec();

    for (int i=0;i<tracks.count();++i)
        tracks.at(i)->dec();
    tracks.clear();
}

bool TPPlaylist::moveToBottom(TPTrack *trackToMove)
{
    remove(trackToMove);
    tracks.append(trackToMove);
    return true;
}

bool TPPlaylist::moveToTop(TPTrack *trackToMove)
{
    return moveAfter(trackToMove, NULL);
}

bool TPPlaylist::moveBefore(TPTrack *trackToMove, TPTrack *trackToMoveBefore)
{
    int idx = tracks.indexOf(trackToMoveBefore);
    if (idx < 0)
        // Move before anything -> move to bottom
        return moveToBottom(trackToMove);
    if (idx == 0)
        // Move before first track -> well, move it then.
        return moveToTop(trackToMove);
    // moveBefore(x) == moveAfter(x-1)
    return moveAfter(trackToMove, tracks.at(idx-1));
}

bool TPPlaylist::moveAfter(TPTrack *trackToMove, TPTrack* afterToMove)
{
    int oldIdx = tracks.indexOf(trackToMove);
    if (oldIdx < 0)
        return false;

    int newIdx = 0;
    if (afterToMove)
    {
        newIdx = tracks.indexOf(afterToMove);
        if (newIdx < 0)
            return false;
        newIdx += 1;
    }

    tracks.removeAt(oldIdx);
    tracks.insert(newIdx, trackToMove);

    return true;
}

void TPPlaylist::add(const QString url, bool toBack)
{
    TPAlbum *album = new TPAlbum(url);
    TPTrack *track = new TPTrack(album);
    album->dec();
    track->setString(trackAttrFilename, url);
    track->setString(objectAttrName, url);
    add(track, toBack);
    track->dec();
}

void TPPlaylist::add(TPFeedItem *feedItem, bool toBack)
{
    Q_ASSERT(feedItem);

    TPAlbum *album = new TPAlbum(feedItem->getTitle());
    TPTrack *track = new TPTrack(album);
    album->dec();

    track->setActingAsDelegateOf(TPObjectDelegate(feedItem, feedItem));

    track->setString(trackAttrFilename, feedItem->getEnclosure());
    track->setString(objectAttrName, feedItem->getDescription());
    add(track, toBack);
    // Playlist now owns the track.
    track->dec();
}

void TPPlaylist::add(TPFeed *feed, bool toBack)
{
    Q_ASSERT(feed);

    if (toBack)
        for (int i=0;i<feed->count();++i)
            add(feed->at(i), toBack);
    else
        for (int i=feed->count()-1;i>=0;--i)
            add(feed->at(i), toBack);
}

void TPPlaylist::add(TPTrack *track, bool toBack)
{
    Q_ASSERT(track);
    if (!hasTrack(track))
    {
        track->inc();
        if (toBack)
            tracks.append(track);
        else
            tracks.insert(0, track);
    }
}

void TPPlaylist::add(TPAlbum *album, bool toBack)
{
    Q_ASSERT(album);
    if (toBack)
        for (int i=0; i<album->countTracks();++i)
            add(album->getTrackByPos(i), toBack);
    else
        for (int i=album->countTracks()-1;i>=0;--i)
            add(album->getTrackByPos(i), toBack);
}

void TPPlaylist::add(TPArtist *artist, bool toBack)
{
    Q_ASSERT(artist);
    for (int i=0;i<artist->countAlbums();++i)
        add(artist->getAlbum(i), toBack);
}

void TPPlaylist::add(TPPlaylist *playlist, bool toBack)
{
    Q_ASSERT(playlist);
    for (int i=0;i<playlist->tracks.count();++i)
        add(playlist->tracks.at(i), toBack);
}

void TPPlaylist::shuffle()
{
    static bool randomized = false;

    if (!randomized)
    {
        qsrand(QTime::currentTime().msec());
        randomized = true;
    }

    if (tracks.count() <= 1)
        return;

    QList<TPTrack *> shuffled;
    while (tracks.count())
        shuffled.append(tracks.takeAt(qrand() % tracks.count()));

    tracks = shuffled;
}

bool TPPlaylist::hasTrack(const TPTrack *track)
{
    return tracks.indexOf((TPTrack *)track) >= 0;
}

void TPPlaylist::remove(TPTrack *track)
{
    int index = tracks.indexOf(track);
    if (index >= 0)
    {
        tracks.removeAt(index);
        track->dec();
    }
}

void TPPlaylist::cloneFrom(TPPlaylist *playlist)
{
    if (clonedFrom)
        clonedFrom->dec();

    clonedFrom = playlist;
    clonedFrom->inc();
    add(playlist);
}

const QString TPPlaylist::getString(const QString key, const QString defaultValue) const
{
    if (key == objectAttrIdentifier)
        return const_cast<TPPlaylist *>(this)->identifier();
    else if (key == playlistAttrArtSmall)
        return getSmallArtName();
    else if (key == playlistAttrArtLarge)
        return getLargeArtName();

    return TPAssociativeObject::getString(key, defaultValue);
}

QVariantMap TPPlaylist::toMap(QStringList *filteredKeys)
{
    QVariantMap result = TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(objectAttrIdentifier))
            result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
        if (!filteredKeys->contains(playlistAttrArtSmall))
        {
            QString smallArt = getSmallArtName();
            if (smallArt.length())
                result.insert(playlistAttrArtSmall, smallArt);
        }
        if (!filteredKeys->contains(playlistAttrArtLarge))
        {
            QString largeArt = getLargeArtName();
            if (largeArt.length())
                result.insert(playlistAttrArtLarge, largeArt);
        }
    }
    else
    {
        result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));

        QString largeArt = getLargeArtName();
        if (largeArt.length())
            result.insert(playlistAttrArtLarge, largeArt);

        QString smallArt = getSmallArtName();
        if (smallArt.length())
            result.insert(playlistAttrArtSmall, smallArt);
    }

    return result;
}



