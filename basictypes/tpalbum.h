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

#ifndef TPALBUM_H
#define TPALBUM_H

#include <QString>
#include <QVector>
#include <QList>
#include <QSet>
#include <QFile>
#include "tptrack.h"
#include "tpartist.h"
#include "tpreferencecounted.h"
#include "tpassociative.h"
#include "tpidbase.h"
#include "tppathutils.h"
#include "tputils.h"

#define ALBUM_ART_TYPE  "PNG"

const int AlbumYearNotResolved = -2;
const int AlbumYearNotAvailable = -1;


class TPAlbum :
        public TPReferenceCounted,
        public TPAssociativeObject,
        public TPIdBase
{
public:

    enum ArtType
    {
        EBigArt,
        ESmallArt
    };


    TPAlbum(QString name);
    TPAlbum(QString name, TPAssociativeDBItem *dbItem);
    TPAlbum(TPArtist *artist, QString name, TPAssociativeDBItem *dbItem);
    ~TPAlbum();

    QString getName() const;
    void addTrack(TPTrack *track);
    void unlinkTrack(TPTrack *track);

    TPTrack* getTrack(int index) const;
    TPTrack* getTrackByPos(int pos) const;


    int countTracks() const;
    int lastTrack() const;
    int getLen() const;
    bool isComplete() const;
    bool isEmpty() const;
    bool isEqual(const TPAlbum &other) const;
    int getYear() const;

    inline bool hasAlbumArt(ArtType type) const
    {
        QString filename = TPPathUtils::getAlbumArtFolder() + const_cast<TPAlbum*>(this)->getAlbumArtFilename(type);
        QFile f(filename);
        return f.exists();
    }

    inline const QString fullPathToAlbumArt(ArtType type)
    {
        return TPPathUtils::getAlbumArtFolder() + getAlbumArtFilename(type);
    }

    inline const QString getAlbumArtFilename(ArtType type)
    {
        if (type == ESmallArt)
        {
            QString artSmall = getString(albumAttrArtSmall);
            if (artSmall.length() > 4)
                return artSmall;
            artSmall = generateAlbumArtFilename(ALBUM_ART_TYPE, type);
            setString(albumAttrArtSmall, artSmall);
            save(10);

            return artSmall;
        }
        else
        {
            QString artLarge = getString(albumAttrArtLarge);
            if (artLarge.length() > 4)
                return artLarge;
            artLarge = generateAlbumArtFilename(ALBUM_ART_TYPE, type);
            setString(albumAttrArtLarge, artLarge);
            save(10);

            return artLarge;
        }
    }

    inline QString generateAlbumArtFilename(const QString extension, ArtType type)
    {        
        QString fn;
        fn.reserve(256);
        if (artist)
            fn += artist->getName().toUtf8().toLower();
        else
            fn += "noartist";

        fn += "_" + getString(objectAttrName).toUtf8().toLower();
        fn = TPUtils::ensureFilenameCharacters(fn);
        if (type == ESmallArt)
            fn += ".SMALL.";
        else
            fn += ".LARGE.";
        fn += extension;

        return fn;
    }

    TPArtist* getArtist() const;

    QSet<int> getMissingTracks() const;
#if 1
    void inc();
    void dec();
#endif

public: // From TPAssociativeObject (specialization)

    QMap<QString, QVariant> toMap(QStringList *filteredKeys = 0);
    int getInt(const QString key, int defaultValue = 0) const;
    const QString getString(const QString key, const QString defaultValue="") const;

private:

    //! Artist performing this album
    TPArtist *artist;
    mutable int year;
    QList<TPTrack *> tracks;
  };

#endif // ALBUM_H
