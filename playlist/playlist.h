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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QList>
#include <QLinkedList>
#include <QAbstractListModel>
#include "tptrack.h"
#include "db/tptrackdb.h"
#include "tpartist.h"
#include "tpalbum.h"
#include "tppathutils.h"
#include "tputils.h"
#include "tpfeed.h"
#include "tpidbase.h"
#include "tpidbasemap.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tplog.h"
#include "tpyoutubeobject.h"

// STATIC CONSTANT DATA
static const QString randomPlaylistName("Random tracks");

//! @class TPPlaylist
//! @brief Playlist class that encapsulates various types of playlists
class TPPlaylist : public TPIdBase, public TPAssociativeObject, public TPReferenceCounted
{
public:

    //! @brief C++ constructor for playlist created from a m3u file.
    TPPlaylist(const QString name, TPAssociativeDB *db, const QString filename);

    //! @brief C++ constructor - the default one used to create empty playlists
    TPPlaylist();

    //! @brief C++ construct for randomly ordered playlists.
    TPPlaylist(TPTrackDB &_db);

    //! @brief C++ destructor
    ~TPPlaylist();

    //! @brief sets user profile that is used mainly if/when
    //! playing random tracks where only matching items should
    //! get pick'd
    void setUserProfile(const QString userProfile);

    void dec()
    {
//        DEBUG() << "PLAYLIST: references-- " << counter-1;
        TPReferenceCounted::dec();
    }

    void inc()
    {
//        DEBUG() << "PLAYLIST: " << getName() << " references++ " << counter+1;
        TPReferenceCounted::inc();
    }

    //! @brief moves a specified track to be after a specific track.
    bool moveAfter(TPTrack *trackToMove, TPTrack *trackToMoveAfter);
    bool moveBefore(TPTrack *trackToMove, TPTrack *trackToMoveBefore);
    bool moveToTop(TPTrack *trackToMove);
    bool moveToBottom(TPTrack *trackToMove);

    //! @brief Checks whether playlist can be used as such
    //! or should it be cloned before using (so that its content
    //! can be altered without being worried on not changing the original).
    bool needToBeCloned();

    void cloneFrom(TPPlaylist *playlist);
    void add(TPPlaylist *playlist, bool toBack = true);
    void add(TPTrack *track, bool toBack = true);
    void add(TPArtist *artist, bool toBack = true);
    void add(TPAlbum *album, bool toBack = true);
    void add(TPFeedItem *feedItem, bool toBack = true);
    void add(TPFeed *feed, bool toBack = true);
    void add(const QString url, bool toBack = true);
    void add(TPYouTubeObject *object, bool toBack=true);

    //! @brief Removes track from playlist and if removed, will decrease its reference count by one.
    void remove(TPTrack *track);
    void removeArtist(const QString &id);
    void removeAlbum(const QString &id);

    bool hasTrack(const TPTrack *track);

    void shuffle();

    void setFilename(const QString _filename);

    const QString getFilename() const;

    void setCategory(const QString _category);

    void setName(const QString _name);

    void setSmallArtName(const QString _smallArtName);

    const QString getSmallArtName() const;

    const QString getLargeArtName() const;

    void setLargeArtName(const QString _largeArtName);

    int getLength() const;

    int count() const;

    TPTrack* findTrack(const QString id);

    TPTrack* at(int index) const;

    TPTrack* takeNext();

    QString getName() const;

    QString getCategory() const;

    int indexOf(TPTrack *track) const;

    bool isLastTrack(TPTrack *track) const;

    TPPlaylist* getClonedFrom() const;

    friend class PlaylistMgr;
    friend class PlaylistUtils;

    const QString getString(const QString key, const QString defaultValue = "") const;
    QVariantMap toMap(QStringList *filteredKeys);

private:

    void fill();

    //! @brief Gets a random track that is not in tracklist
    TPTrack* getRandomTrackNotInList();

    //! @brief Checks whether track matches against the currently
    //! configured user profile so that it can be added to playlist.
    bool trackMatchesUserProfile(TPTrack *track);

private:

    //! TrackdB. Optional and only viable for playlists
    //! that are played on random order.
    TPTrackDB *db;

    //! All contained tracks.
    QList<TPTrack *> tracks;

    //! Reference to other playlist instance.
    TPPlaylist *clonedFrom;

    //! User profile.
    QString userProfile;
};

#endif // PLAYLIST_H
