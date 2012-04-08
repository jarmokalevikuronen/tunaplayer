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
#include "tpusermanager.h"

TPDatabases::TPDatabases(QObject *parent) :
    QObject(parent), feedMgr(0), trackDB(0), playlistMgr(0), userMgr(0)
{
}

TPDatabases::~TPDatabases()
{
    delete trackDB;
    delete playlistMgr;
    delete feedMgr;
    delete userMgr;
}

void TPDatabases::load()
{
    // This will just instantiate the relevant databases - in practise
    // that means that existing data will be loaded from disk.
    (void)getTrackDB()->getAlbumDB();
    (void)getTrackDB()->getArtistDB();
}

int TPDatabases::build(QStringList &mediaFiles)
{
    userMgr = processUserAccessFiles(mediaFiles, userMgr);
    getTrackDB()->processFiles(mediaFiles);

    return mediaFiles.count();
}

void TPDatabases::buildFinished()
{
    TPTrackDB *tdb = getTrackDB();
    TPUserManager *umgr = getUserDB();

    tdb->executePostCreateTasks();

    for (int i=0;i<tdb->count();i++)
    {
        TPTrack *t = tdb->at(i);

        // This will setup a dynamic property for a track. This property defines
        // which users actually like to see the particular track.
        QString tags = umgr->tagStringForFile(t->getFilename());
//        DEBUG() << "TAGSFORFILE: " << t->getFilename() << " TAGS: " << tags;
        t->setString(objectAttrUserTokens_DYNAMIC, tags);
    }
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

TPUserManager* TPDatabases::getUserDB()
{
    return userMgr;
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

TPUserManager* TPDatabases::processUserAccessFiles(QStringList &filenames, TPUserManager *mgr)
{
    TPUserManager *m = mgr;
    if (!m)
        m = new TPUserManager;

    Q_ASSERT(m);

    //
    // Loop "backwards" as we will remove items from the list along the way.
    //
    for (int i=filenames.length()-1;i>=0;--i)
    {
        QString fn = filenames.at(i);

        static const QString UserAccessFileExt(".tt");

        if (fn.endsWith(UserAccessFileExt))
        {
            filenames.removeAt(i);
            m->insertTagFile(fn);
        }
    }

    return m;
}


