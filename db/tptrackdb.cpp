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

#include "tptrackdb.h"
#include "tpschemes.h"
#include "tppathutils.h"
#include "tpartist.h"
#include "tpalbum.h"
#include "tpmediafilemetaextractor.h"
#include "tpaudioutils.h"
#include "tplog.h"

TPTrackDB::TPTrackDB(QObject *parent) :
    QObject(parent),
    TPDBBase<TPTrack *>(this, TPPathUtils::getMusicDbFilename(), schemeTrack)
{
    artistDB = new TPArtistDB(this);
    Q_ASSERT(artistDB);

    albumDB = new TPAlbumDB(this);
    Q_ASSERT(albumDB);

    db->visitItems(this);

    // Do process here the removed items collected during the visit process
    for (int i=0;i<removedItems.count();++i)
        db->removeItem(removedItems.at(i));
    removedItems.clear();
}

TPTrackDB::~TPTrackDB()
{
    DEBUG() << "DB: ~TPTrackDB";

    // Not really needed..
    delete artistDB;
    delete albumDB;

    TPTrack *track = objects.takeLast();
    while (track)
    {
        track->detachFromAlbum();
        track->dec();
        track = objects.takeLast();
    }
}

void TPTrackDB::insertItem(TPAssociativeDBItem *item)
{
    QString artistName = TPAudioUtils::normalizeArtistName(item->stringValue(trackAttrArtistName));
    QString albumName = item->stringValue(trackAttrAlbumName);

    TPArtist *artist = artistDB->createArtist(artistName);
    Q_ASSERT(artist);

    TPAlbum *album = albumDB->createAlbum(artist, albumName);
    Q_ASSERT(album);

    TPTrack *track = new TPTrack(album, item);
    Q_ASSERT(track);

    objects.insertObject(track);
}

void TPTrackDB::visitAssociativeDBItem(TPAssociativeDBItem *item)
{
    QString filename = item->primaryKey();

    if (!filename.length())
    {
        ERROR() << "DB: Track has empty filename";
        return;
    }

    QFile f(filename);
    if (!f.exists())
    {
        removedItems.append(item);
        DEBUG() << "DB: Track \"" << filename << "\" removed";
    }
    else
    {
        insertItem(item);
    }
}

TPTrack* TPTrackDB::getTrackByFilename(const QString _filename)
{
    QString filename = _filename.trimmed();

    TPTrack *track = NULL;

    TPAssociativeDBItem *item = db->item(filename);
    if (item)
    {
        const QByteArray identifier = item->stringValue(objectAttrIdentifier).toUtf8();

        if (identifier.length())
            track = objects.getObject(identifier);
        if (track)
            return track;
    }

    // fallback to brute force. In
    // general, this path is taken at construction if playlist
    // contains a reference to non-existent track.
    for (int i=0;i<objects.count();++i)
    {
        TPTrack *object = objects.at(i);
        if (object->getFilename() == filename)
            return object;
    }

    return NULL;
}

void TPTrackDB::processFiles(const QStringList files)
{
    for (int i=0;i<files.count();++i)
    {
        QString filename(files.at(i));

        if (!db->item(filename))
        {
            TPAssociativeDBItem *item =
                    TPMediaFileMetaExtractor::extractTrackInfo(filename, db);
            if (item)
            {
                item->setValue(trackAttrFilename, filename);
                db->addItem(item);
                insertItem(item);
            }
        }
    }
}

QStringList* TPTrackDB::getTrackFilenames() const
{
    QStringList *result = new QStringList;
    Q_ASSERT(result);

    for (int i=0;i<count();++i)
        result->append(at(i)->getFilename());

    return result;
}


