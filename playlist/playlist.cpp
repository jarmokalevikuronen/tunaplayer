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

//! Amount of items always available in random playlist.
#define RANDOM_PLAYLIST_ITEMS 6

void doRandomize()
{
    static bool randomized = false;
    if (!randomized)
    {
        qsrand(QTime::currentTime().msec());
        randomized = true;
    }
}

TPPlaylist::TPPlaylist(const QString _name, TPAssociativeDB *db, const QString _filename)
    : TPIdBase(schemePlaylist, _filename), TPAssociativeObject(db->allocItem(_filename))
{
    setString(objectAttrName, _name);
    setString(playlistAttrFilename, _filename);
    setString(objectAttrScheme, schemePlaylist);
    clonedFrom = 0;
    db = 0;
}

TPPlaylist::TPPlaylist() : TPIdBase(schemePlaylist, "currentlyplayed")
{
    setString(objectAttrName, "(Not Saved)");
    clonedFrom = 0;
    db = 0;
}

TPPlaylist::TPPlaylist(TPTrackDB &_db) : TPIdBase(schemePlaylist, "randomized")
{
    db = &_db;
    clonedFrom = 0;
    setString(objectAttrName, randomPlaylistName);
    setString(objectAttrScheme, schemePlaylist);
    setInt(playlistAttrBuiltIn, 1); // This is a "built-in" playlist
    fill();
}

TPPlaylist::~TPPlaylist()
{
    if (clonedFrom)
        clonedFrom->dec();

    TPDecForAll(tracks)
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
    TPAlbum *album = new TPAlbum(url); // album:ref => 1
    TPTrack *track = new TPTrack(album); // album:ref => 2, track:ref => 2
    album->dec(); // album:ref => 1
    track->dec(); // track:ref => 1
    track->setString(trackAttrFilename, url);
    track->setString(objectAttrName, url);
    add(track, toBack); // track:ref => 2
    track->dec(); // track:ref => 1
}

void TPPlaylist::add(TPFeedItem *feedItem, bool toBack)
{
    Q_ASSERT(feedItem);

    TPAlbum *album = new TPAlbum(feedItem->getTitle());
    TPTrack *track = new TPTrack(album);
    album->dec();
    track->dec(); // see above.

    track->setActingAsDelegateOf(TPObjectDelegate(feedItem, feedItem));
    track->setString(trackAttrFilename, feedItem->getEnclosure());
    track->setString(objectAttrName, feedItem->getDescription());

    add(track, toBack);
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
    else
    {
        ERROR() << "PLAYLIST: " << "duplicate track -> not inserted";
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
    doRandomize();

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
    fill();
}


bool TPPlaylist::needToBeCloned()
{
    return db ? false : true;
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

void TPPlaylist::setFilename(const QString _filename)
{
    setString(playlistAttrFilename, _filename);
}

const QString TPPlaylist::getFilename() const
{
    if (clonedFrom)
        return clonedFrom->getFilename();

    return getString(playlistAttrFilename);
}

void TPPlaylist::setCategory(const QString _category)
{
    setString(playlistAttrCategory, _category);
}

void TPPlaylist::setName(const QString _name)
{
    setString(objectAttrName, _name);
}

void TPPlaylist::setSmallArtName(const QString _smallArtName)
{
    setString(playlistAttrArtSmall, _smallArtName);
}

const QString TPPlaylist::getSmallArtName() const
{
    if (clonedFrom)
        return clonedFrom->getSmallArtName();

    QString smallArtName = TPAssociativeObject::getString(playlistAttrArtSmall);

    if (smallArtName.length() < 1)
        return smallArtName;

    QString fn = TPPathUtils::getPlaylistArtFolder() + smallArtName;
    QFile f(fn);

    return f.exists() ? TPPathUtils::getNormalizedPathForWebServer(fn) : QString("");
}

const QString TPPlaylist::getLargeArtName() const
{
    if (clonedFrom)
        return clonedFrom->getLargeArtName();

    QString largeArtName = TPAssociativeObject::getString(playlistAttrArtLarge);

    if (largeArtName.length() < 1)
        return largeArtName;

    QString fn = TPPathUtils::getPlaylistArtFolder() + largeArtName;
    QFile f(fn);

    return f.exists() ? TPPathUtils::getNormalizedPathForWebServer(fn) : QString("");
}

void TPPlaylist::setLargeArtName(const QString _largeArtName)
{
    setString(playlistAttrArtLarge, _largeArtName);
}

int TPPlaylist::getLength() const
{
    int len = 0;

    for (int i=0; i<count(); ++i)
    {
        int l = at(i)->getLen();
        if (l > 0)
           len += l;
    }

    return len;
}

int TPPlaylist::count() const
{
    return tracks.count();
}

TPTrack* TPPlaylist::findTrack(const QString id)
{
    for (int i=0;i<tracks.count();++i)
        if (tracks.at(i)->identifier(true) == id)
            return tracks.at(i);

    return NULL;
}

TPTrack* TPPlaylist::at(int index) const
{
    return tracks.at(index);
}

TPTrack* TPPlaylist::takeNext()
{
    if (tracks.count())
    {
        TPTrack *track = tracks.takeFirst();
        track->dec();
        fill();
        return track;
    }

    return NULL;
}

QString TPPlaylist::getName() const
{
    if (clonedFrom)
        return clonedFrom->getName();

    return getString(objectAttrName);
}

QString TPPlaylist::getCategory() const
{
    if (clonedFrom)
        return clonedFrom->getCategory();

    return getString(playlistAttrCategory);
}

int TPPlaylist::indexOf(TPTrack *track) const
{
    return tracks.indexOf(track);
}

bool TPPlaylist::isLastTrack(TPTrack *track) const
{
    int index = indexOf(track);
    if (index < 0 || tracks.count() <= 0)
        return false;
    return index == (tracks.count() - 1);
}

TPPlaylist* TPPlaylist::getClonedFrom() const
{
    return clonedFrom;
}

TPTrack *TPPlaylist::getRandomTrackNotInList()
{
    int dbItems = db->count();

    if (dbItems < 1)
        return 0;

    int startPosition = (qrand() % db->count());
    for (int i=0;i<dbItems;i++)
    {
        TPTrack *track = db->at((i + startPosition) % dbItems);
        if (!hasTrack(track))
        {
            if (trackMatchesUserProfile(track))
                return track;
        }
    }

    // This means that there were no matching tracks.
    return 0;
}

void TPPlaylist::fill()
{
    if (!db)
        return;

    doRandomize();

    while (tracks.count() < RANDOM_PLAYLIST_ITEMS)
    {
        TPTrack *track = getRandomTrackNotInList();
        if (track)
        {
            track->inc();
            tracks.append(track);
        }
        else
            break;
    }
}

void TPPlaylist::setUserProfile(const QString _userProfile)
{
    DEBUG() << "PLAYLIST: setUserProfile: " << _userProfile;
    userProfile = _userProfile;

    //
    // Do fill in the tracks.
    //
    if (userProfile.length())
    {
        foreach(TPTrack *t, tracks)
        {
            t->dec();
        }
        tracks.clear();
        fill();
    }
}

bool TPPlaylist::trackMatchesUserProfile(TPTrack *track)
{
    if (!userProfile.length())
        return true;
    if (!track)
        return false;

    QString trackTok = track->getString(objectAttrUserTokens_DYNAMIC);

    return trackTok.contains(userProfile, Qt::CaseInsensitive);
}
