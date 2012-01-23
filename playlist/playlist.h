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

class TPPlaylist : public TPIdBase, public TPAssociativeObject, public TPReferenceCounted
{
public:

    TPPlaylist(const QString name, TPAssociativeDB *db, const QString filename);
    TPPlaylist();
    ~TPPlaylist();

    //! @brief moves a specified track to be after a specific track.
    bool moveAfter(TPTrack *trackToMove, TPTrack *trackToMoveAfter);
    bool moveBefore(TPTrack *trackToMove, TPTrack *trackToMoveBefore);
    bool moveToTop(TPTrack *trackToMove);
    bool moveToBottom(TPTrack *trackToMove);

    void cloneFrom(TPPlaylist *playlist);
    void add(TPPlaylist *playlist, bool toBack = true);
    void add(TPTrack *track, bool toBack = true);
    void add(TPArtist *artist, bool toBack = true);
    void add(TPAlbum *album, bool toBack = true);
    void add(TPFeedItem *feedItem, bool toBack = true);
    void add(TPFeed *feed, bool toBack = true);
    void add(const QString url, bool toBack = true);

    //! @brief Removes track from playlist and if removed, will decrease its reference count by one.
    void remove(TPTrack *track);
    void removeArtist(const QString &id);
    void removeAlbum(const QString &id);

    bool hasTrack(const TPTrack *track);

    void shuffle();

    inline void setFilename(const QString _filename)
    {
        setString(playlistAttrFilename, _filename);
    }

    inline const QString getFilename() const
    {
        if (clonedFrom)
            return clonedFrom->getFilename();

        return getString(playlistAttrFilename);
    }

    inline void setCategory(const QString _category)
    {
        setString(playlistAttrCategory, _category);
    }

    inline void setName(const QString _name)
    {
        setString(objectAttrName, _name);
    }

    // TODO: getSmallArtName ja niin edelleen
    // Ovat periaatteessa turhia metodeita, koska
    // homma pitäisi käytännössä hoitaa toisella tavalla kokonaan!.

    inline void setSmallArtName(const QString _smallArtName)
    {
        setString(playlistAttrArtSmall, _smallArtName);
    }

    inline const QString getSmallArtName() const
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

    inline const QString getLargeArtName() const
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

    inline void setLargeArtName(const QString _largeArtName)
    {
        setString(playlistAttrArtLarge, _largeArtName);
    }

    inline int getLength() const
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

    inline int count() const
    {
        return tracks.count();
    }

    inline TPTrack* findTrack(const QString id)
    {
        for (int i=0;i<tracks.count();++i)
            if (tracks.at(i)->identifier(true) == id)
                return tracks.at(i);

        return NULL;
    }

    inline TPTrack* at(int index) const
    {
        return tracks.at(index);
    }

    inline TPTrack* takeNext()
    {
        if (tracks.count())
            return tracks.takeFirst();

        return NULL;
    }

    inline QString getName() const
    {
        if (clonedFrom)
            return clonedFrom->getName();

        return getString(objectAttrName);
    }

    inline QString getCategory() const
    {
        if (clonedFrom)
            return clonedFrom->getCategory();

        return getString(playlistAttrCategory);
    }

    inline int indexOf(TPTrack *track) const
    {
        return tracks.indexOf(track);
    }

    inline bool isLastTrack(TPTrack *track) const
    {
        int index = indexOf(track);
        if (index < 0 || tracks.count() <= 0)
            return false;
        return index == (tracks.count() - 1);
    }

    inline TPPlaylist* getClonedFrom() const
    {
        return clonedFrom;
    }

    friend class PlaylistMgr;
    friend class PlaylistUtils;

    const QString getString(const QString key, const QString defaultValue = "") const;
    QVariantMap toMap(QStringList *filteredKeys);

private:
    //! All contained tracks.
    QList<TPTrack *> tracks;

    //! Reference to other playlist instance.
    TPPlaylist *clonedFrom;
};

#endif // PLAYLIST_H
