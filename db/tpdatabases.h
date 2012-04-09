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

// FORWARD DECLARATIONS
class TPPlaylistMgr;
class TPTrackDB;
class TPFeedMgr;
class TPArtistDB;
class TPAlbumDB;
class TPUserManager;
class TPYouTubeDB;

//! @class TPDatabases
//! @brief Encapsulates all the different type of database under once facade.
class TPDatabases : public QObject
{
    Q_OBJECT
public:

    //! @brief C++ constructor
    explicit TPDatabases(QObject *parent = 0);

    //! @brief C++ destructor
    ~TPDatabases();

    //! @brief Returns the one and only playlist database.
    TPPlaylistMgr* getPlaylistDB();

    //! @brief Get track database
    TPTrackDB *getTrackDB();

    //! @brief Returns the one and only artist database
    TPArtistDB* getArtistDB();

    //! @brief Returns the one and only album database
    TPAlbumDB* getAlbumDB();

    //! @brief Returns the one and only feed database
    TPFeedMgr* getFeedDB();

    //! @brief Returns the one and only user database.
    TPUserManager* getUserDB();

    //! @brief Returns the Youtube DB if any.
    TPYouTubeDB* getYouTubeDB();

    //! @brief Loads the database into memory (kind of at least).
    void load();

    //! This will construct the database - fragment by fragment.
    //! returns the amount of _media_ files that was processed (access token files not taking effect).
    int build(QStringList &mediaFiles);

    //! Called when it is expected that the build will not be called again.
    void buildFinished();

signals:

public slots:


private:  // New Implementation

    //! @brief Processes all User access token files (*.tt) and removes
    //! those from the list. if mgr is provided, it will be updated instead
    //! of creating new instance.
    TPUserManager *processUserAccessFiles(QStringList &filenames, TPUserManager *mgr);

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

    //! User mgr -> provides information which users
    //! will like to see which audio files.
    TPUserManager* userMgr;

    //! Youtube database handler..
    TPYouTubeDB *youtubeDB;
};

#endif // TPDATABASES_H
