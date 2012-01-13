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

#include "tpalbumdb.h"
#include "tppathutils.h"
#include "tpschemes.h"
#include "tpaudioutils.h"

TPAlbumDB::TPAlbumDB(QObject *parent) :
    QObject(parent),
    TPDBBase<TPAlbum *>(this, TPPathUtils::getAlbumDbFilename(), schemeAlbum)
{
}

TPAlbumDB::~TPAlbumDB()
{
    decAllObjects();
}

TPAlbum* TPAlbumDB::createAlbum(TPArtist *artist, const QString name)
{
    Q_ASSERT(artist);

    // Firstly, try to search this specific album if any.
    for (int i=0;i<objects.count();++i)
    {
        TPAlbum *album = objects.at(i);
        if (TPAudioUtils::albumsEqual(*album, name))
            if (TPAudioUtils::artistsEqual(*artist, album->getArtist()->getName()))
                return album;
    }

    QString dbItemName = artist->getName() + "/../" + name;

    TPAssociativeDBItem *dbItem = db->allocItem(dbItemName);
    Q_ASSERT(dbItem);

    TPAlbum *album = new TPAlbum(artist, name, dbItem);
    Q_ASSERT(album != NULL);

    objects.insertObject(album);

    return album;
}
