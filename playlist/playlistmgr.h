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

#ifndef PLAYLISTMGR_H
#define PLAYLISTMGR_H

#include <QList>
#include <QStringList>
#include <QAbstractListModel>
#include <QDebug>
#include "playlist.h"
#include "db/tptrackdb.h"
#include "db/tpalbumdb.h"
#include "tpassociative.h"
#include "tpschemes.h"
#include "playlistutils.h"
#include "db/tpdbtemplate.h"
#include "db/tpdatabases.h"

//! @class TPPlaylistMgr
//! @brief Playlist manager - maintains the things that are needed.
class TPPlaylistMgr : public QObject, public TPDBBase<TPPlaylist *>
{
    Q_OBJECT

public:

    //! @brief C++ constructor
    TPPlaylistMgr(QObject *parent = NULL);

    //! @brief C++ destructor
    ~TPPlaylistMgr();

    //! @brief Scans a specific folder for playlists and parses those into objects
    //! @folder where to search playlists from.
    void scanPlaylists(TPTrackDB *trackDB, TPAlbumDB *albumDB);

    //! @brief Gets a playlist in specific index. NULL if index out of bounds.
    inline TPPlaylist* getPlaylist(int index) const
    {
        if (index < 0 || index >= objects.count())
            return 0;

        return objects.at(index);
    }

    inline TPTrack* findTrack(const QString id)
    {
        for (int i=0;i<objects.count();++i)
        {
            TPTrack *track = objects.at(i)->findTrack(id);
            if (track)
                return track;
        }

        return NULL;
    }

    inline TPPlaylist* getPlaylist(const QString someId)
    {
        TPPlaylist *pl = getPlaylistByName(someId);
        return pl ? pl : getPlaylistById(someId);
    }

    inline TPPlaylist* getPlaylistById(const QString id)
    {
        return getById(id);
    }

    //! @brief Reloads a playlist from a file.
    bool reloadPlaylist(TPDatabases *_db, TPPlaylist *pl)
    {
        Q_ASSERT(pl);
        objects.removeObject(pl);
        pl->dec();

        TPPlaylist *reloaded =
                TPPlaylistUtils::importLocalPlaylist(_db->getTrackDB(), _db->getAlbumDB(), db, pl->getFilename());
        if (!reloaded)
            return false;
        objects.insertObject(reloaded);

        return true;
    }

    TPPlaylist* getPlaylistByName(const QString name);
    bool removePlaylistByName(const QString name);
    bool removePlaylistById(const QString id);
    bool removePlaylist(const QString someId)
    {
        return removePlaylistById(someId) || removePlaylistByName(someId);
    }

    //! @brief Adds a playlist. Pointer ownership transfers.
    //! @param playlist playlist to be added.
    void addPlaylist(TPPlaylist *playlist);

    const QString uniquePlaylistName(const QString name)
    {
        TPPlaylist *existing = getPlaylistByName(name);
        if (existing)
        {
            QString basename = name;
            QString newname;
            int index = 2;
            do
            {
                newname = basename;
                newname += " (";
                newname += QString::number(index++);
                newname += ")";
                existing = getPlaylistByName(newname);
            } while (existing);

            return newname;
        }

        return name;
    }

    TPPlaylist* createEmptyPlaylist()
    {
        return new TPPlaylist;
    }

    TPPlaylist* createPlaylist(const QString uniqueName)
    {
        if (getPlaylistByName(uniqueName))
            return NULL;

        // TODO: Should fix the name through some function that will
        // remove unwanted characters if such exists.
        TPPlaylist *pl = new TPPlaylist(uniqueName, db, TPPathUtils::getPlaylistFolder() + uniqueName + ".m3u");
        Q_ASSERT(pl);
        return pl;
    }

private:

    void listPlaylistFiles(const QString folder, QStringList &list);

    //! @brief Checks the from the database which playlists are marked as deleted
    //! and removes those from the disk.
    void cleanupPlaylists();

private:

};

#endif // PLAYLISTMGR_H
