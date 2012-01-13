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

#include "tpartistdb.h"

#include "tppathutils.h"
#include "tpschemes.h"
#include "tpaudioutils.h"

TPArtistDB::TPArtistDB(QObject *parent) :
    QObject(parent),
    TPDBBase<TPArtist *>(this, TPPathUtils::getArtistDbFilename(), schemeArtist)
{

}

TPArtistDB::~TPArtistDB()
{
    decAllObjects();
}

TPArtist* TPArtistDB::find(const QString name) const
{
    for (int i=0;i<objects.count();++i)
    {
        TPArtist *artist = objects.at(i);

        // This indeed contains some intelligence
        // where "The Who" and "Who" equals and so forth.
        if (TPAudioUtils::artistsEqual(*artist, name))
            return artist;
    }

    return NULL;
}

TPArtist* TPArtistDB::createArtist(const QString name)
{
    // Fetch existing if such exists.. there is some
    // intelligence in searching due the various reasons.
    TPArtist *artist = find(name);
    if (artist)
        return artist;

    TPAssociativeDBItem *dbItem = db->allocItem(name);
    Q_ASSERT(dbItem);

    artist = new TPArtist(name, dbItem);
    Q_ASSERT(artist);

    objects.insertObject(artist);

    return artist;
}
