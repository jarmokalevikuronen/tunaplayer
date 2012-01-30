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

#include <QDirIterator>

#include "playlistmgr.h"
#include "playlistutils.h"
#include "tppathutils.h"
#include "tplog.h"

TPPlaylistMgr::TPPlaylistMgr(QObject *parent) :
    QObject(parent),
    TPDBBase<TPPlaylist *>(this, TPPathUtils::getPlaylistDbFilename(), schemePlaylist)
{
    // Intentionally left empty.
}

TPPlaylistMgr::~TPPlaylistMgr()
{
    DEBUG() << "PLAYLIST: " << "~TPPlaylistMgr";

    decAllObjects();
}

void TPPlaylistMgr::scanPlaylists(TPTrackDB *trackDB, TPAlbumDB *albumDB)
{
    QStringList playlistFiles;

    listPlaylistFiles(TPPathUtils::getPlaylistFolder(), playlistFiles);

    QStringList::iterator it = playlistFiles.begin();
    while (it != playlistFiles.end())
    {
        addPlaylist(TPPlaylistUtils::importLocalPlaylist(trackDB, albumDB, db, *it));
        ++it;
    }

    cleanupPlaylists();
}

void TPPlaylistMgr::listPlaylistFiles(const QString folder, QStringList &list)
{
    QStringList nameFilters; nameFilters << "*.m3u" << "*.M3U";

    QDirIterator *dirIterator = new QDirIterator(
                    folder,
                    nameFilters,
                    QDir::Readable | QDir::Files,
                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while (dirIterator->hasNext())
    {
        dirIterator->next();
        list.append(dirIterator->fileInfo().absoluteFilePath());
    }
    delete dirIterator;

}

TPPlaylist* TPPlaylistMgr::getPlaylistByName(const QString nameOrId)
{
    if (nameOrId.length() == 0)
        return NULL;

    for (int i=0;i<objects.count();++i)
        if (objects.at(i)->getName() == nameOrId)
            return objects.at(i);

    return getById(nameOrId);
}

bool TPPlaylistMgr::removePlaylistById(const QString id)
{
    TPPlaylist *pl = getById(id);

    if (pl)
        return removePlaylistByName(pl->getName());

    return false;
}

bool TPPlaylistMgr::removePlaylistByName(const QString name)
{
    TPPlaylist *pl = getPlaylistByName(name);
    if (pl)
    {
        // NOTE: We do not actually remove the playlist just yet, but
        // just mark it as deleted!!
        objects.removeObject(pl);
        pl->setInt(objectAttrDeleted, 1);
        pl->dec();

        // This will mark database to be saved after 10ms - or some time after last changes.
        save(10);

        return true;
    }

    return false;
}

void TPPlaylistMgr::addPlaylist(TPPlaylist *playlist)
{
    if (!playlist)
        return;

    TPPlaylist *existing = getPlaylistByName(playlist->getName());
    if (existing)
    {
        QString basename = playlist->getName();
        QString newname;
        int index = 1;
        do
        {
            newname = basename;
            newname += " (";
            newname += QString::number(index++);
            newname += ")";
            existing = getPlaylistByName(newname);
        } while (existing);

        playlist->setName(newname);
    }

    objects.insertObject(playlist);
}

void TPPlaylistMgr::cleanupPlaylists()
{
    for (int i=objects.count()-1;i>=0;--i)
    {
        // From last
        TPPlaylist *pl = objects.at(i);
        if (pl)
        {
            if (pl->getInt(objectAttrDeleted, 0) > 0)
            {
                DEBUG() << "PLAYLIST: REMOVE: " << pl->getName() << " [" << pl->getFilename() << "]";
                objects.removeObject(pl);
                QFile::remove(pl->getFilename());
                pl->dec();
                save(10);
            }
        }
    }
}

