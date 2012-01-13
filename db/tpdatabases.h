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

#ifndef TPDATABASES_H___
#define TPDATABASES_H___

#include <QObject>
//#include "tptrackdb.h"
//#include "playlistmgr.h"
//#include "tpfeedmgr.h"

// FORWARD DECLARATIONS
class TPPlaylistMgr;
class TPTrackDB;
class TPFeedMgr;
class TPArtistDB;
class TPAlbumDB;

class TPDatabases : public QObject
{
    Q_OBJECT
public:
    explicit TPDatabases(QObject *parent = 0);
    ~TPDatabases();

    TPPlaylistMgr* getPlaylistDB();
    TPTrackDB *getTrackDB();
    TPArtistDB* getArtistDB();
    TPAlbumDB* getAlbumDB();
    TPFeedMgr* getFeedDB();

signals:

public slots:

private: // Data

    //! Feed database, virtual one, at least
    TPFeedMgr *feedMgr;

    //! Track database that actually acts
    //! as a container for album and artist databases
    //! That are simply constructed from tracks.
    TPTrackDB *trackDB;

    //! Playlist manager used to handle
    //! all things related to playlists.
    TPPlaylistMgr *playlistMgr;
};

#endif // TPDATABASES_H
