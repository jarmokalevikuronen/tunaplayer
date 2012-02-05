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

#include "tpmediafilemetaextractor.h"
#include <taglib.h>
#include "tag.h"
#include "tstring.h"
#include "fileref.h"
#include "tplog.h"
#include "tpaudioutils.h"

int TPMediaFileMetaExtractor::trackNumberFromFilename(QFileInfo fi)
{
    int number = 0;

    QString filename = fi.completeBaseName();

    if (filename.length() >= 2)
    {
        if (filename.at(0).isDigit())
        {
            if (filename.at(1).isDigit())
            {
                if (!filename.at(2).isDigit())
                    number = 10 * filename.at(0).digitValue() + filename.at(1).digitValue();
            }
            else if (filename.at(1) == QChar('.')  || filename.at(1) == QChar(' '))
                number = filename.at(0).digitValue();
        }
    }

    return number;
}

QString TPMediaFileMetaExtractor::albumNameFromFilename(QFileInfo fi)
{
    // Expectation /as/as1/artist/album/track.mp3

    return fi.absolutePath().section('/', -1).replace('_', ' ');
}

QString TPMediaFileMetaExtractor::artistNameFromFilename(QFileInfo fi)
{
    // Expectation /as/as1/artist/album/track.mp3

    return fi.absolutePath().section('/', -2, -2).replace('_', ' ');
}

QString TPMediaFileMetaExtractor::trackNameFromFilename(QFileInfo fi)
{
    QString trackName = fi.completeBaseName();

    // And do some other filtering in case name is prefixed with
    int testpos = 0;
    while (trackName.length() > testpos &&
           (trackName.at(testpos).isDigit() || trackName.at(testpos).isSpace() || trackName.at(testpos) == QChar('.') || trackName.at(testpos) == QChar('-')))
        ++testpos;

    return trackName.right(trackName.length() - testpos).replace('_', ' ');
}

TPAssociativeDBItem *TPMediaFileMetaExtractor::extractTrackInfo(const QString &filename, TPAssociativeDB *db)
{
    QFileInfo fi(filename);

    TagLib::FileRef fr(filename.toLocal8Bit());

    if (fr.isNull())
    {
        ERROR() << "null fileref: " << filename;
        return NULL;
    }

    TagLib::Tag *tag = fr.tag();
    if (!tag)
    {
        ERROR() << "ERROR: No tag: " << filename;
        return NULL;
    }


    TPAssociativeDBItem *item = new TPAssociativeDBItem(filename, db);

    //
    // Construct ARTIST
    //
    QString artistName = TStringToQString(tag->artist()).trimmed();
    if (artistName.length() <= 0)
    {
        DEBUG() << "File: " << filename << " defines no artist";
        artistName = artistNameFromFilename(fi).trimmed();
        if (artistName.length())
            DEBUG() << QString("extracted %1 from %2").arg(artistName).arg(filename);
    }
    item->setValue(trackAttrArtistName, TPAudioUtils::normalizeArtistName(artistName));

    //
    // Construct ALBUM
    //
    QString albumName = TStringToQString(tag->album()).trimmed();
    if (albumName.length() <= 0)
    {
        DEBUG() << "File: " << filename << " defines no album name";
        albumName = albumNameFromFilename(filename).trimmed();
        if (albumName.length())
            DEBUG() << QString("extracted %1 from %2").arg(albumName).arg(filename);
    }
    item->setValue(trackAttrAlbumName, albumName);

    //
    // Construct TRACK
    //
    QString trackName = TStringToQString(tag->title()).trimmed();
    if (trackName.length() <= 0)
        trackName = trackNameFromFilename(filename).trimmed();
    item->setValue(objectAttrName, trackName);

    QString trackGenre = TStringToQString(tag->genre()).trimmed();
    if (trackGenre.length())
        item->setValue(trackAttrGenre, trackGenre);

    int trackNumber = tag->track();
    if (trackNumber <= 0)
        trackNumber = trackNumberFromFilename(filename);
    item->setIntValue(trackAttrIndex, trackNumber);
    item->setIntValue(trackAttrYear, tag->year());
    item->setIntValue(objectAttrLen, fr.audioProperties()->length());

    return item;
}
