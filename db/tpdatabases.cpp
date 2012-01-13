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

#include "tpdatabases.h"
#include "tpsettings.h"

#include "tptrackdb.h"
#include "tpartistdb.h"
#include "tpalbumdb.h"
#include "tpfeedmgr.h"
#include "playlistmgr.h"

TPDatabases::TPDatabases(QObject *parent) :
    QObject(parent), feedMgr(0), trackDB(0), playlistMgr(0)
{
}

TPDatabases::~TPDatabases()
{
    delete trackDB;
    delete playlistMgr;
    delete feedMgr;
}

TPTrackDB *TPDatabases::getTrackDB(/* maybe include some identifier here in future.. */)
{
    if (!trackDB)
    {
        trackDB = new TPTrackDB(this);
        Q_ASSERT(trackDB);
    }
    return trackDB;
}

TPArtistDB* TPDatabases::getArtistDB()
{
    return getTrackDB()->getArtistDB();
}

TPAlbumDB* TPDatabases::getAlbumDB()
{
    return getTrackDB()->getAlbumDB();
}

TPPlaylistMgr* TPDatabases::getPlaylistDB()
{
    if (!playlistMgr)
    {
        playlistMgr = new TPPlaylistMgr(this);
        Q_ASSERT(playlistMgr);
        playlistMgr->scanPlaylists(getTrackDB(), getAlbumDB());
    }
    return playlistMgr;
}

TPFeedMgr* TPDatabases::getFeedDB()
{
    if (!feedMgr)
    {
        int intervalSecs = TPSettings::instance().get(settingFeedUpdateIntervalSecs, 60*30).toInt();
        intervalSecs = qMax(60, intervalSecs);
        feedMgr = new TPFeedMgr(intervalSecs, this);
        Q_ASSERT(feedMgr);
    }

    return feedMgr;
}
