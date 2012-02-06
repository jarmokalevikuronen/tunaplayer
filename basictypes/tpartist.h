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

#ifndef ARTIST_H
#define ARTIST_H

#include <QString>
#include <QList>
#include <QStringList>
#include "tpreferencecounted.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tpidbase.h"
#include "tpcachedvalues.h"

class TPAlbum;

class TPArtist :
        public TPReferenceCounted,
        public TPAssociativeObject,
        public TPIdBase
{
private:

    static int instanceCount;

public:

    TPArtist(QString name, TPAssociativeDBItem *dbItem = 0);
    ~TPArtist();

    void addAlbum(TPAlbum *album);

    int countAlbums() const;
    TPAlbum* getAlbum(int index) const;

    int countTracks() const;
    QString getName() const;

    int getInt(const QString key, int defaultValue = 0) const;

public: // From TPAssociativeObject

    QMap<QString, QVariant> toMap(QStringList *filteredKeys);
    const QString getString(const QString key, const QString defaultValue = "") const;
    void clearCachedValues();

public: // From TPReferenceCounted

#if 1
    void inc();
    void dec();
#endif

private:

    //! Cached values for faster access during sorting/searching.
    mutable TPCachedValues cache;

    //! Copy constructor -> private
    Q_DISABLE_COPY(TPArtist);

    //! List of albums this artist has composed.
    QList<TPAlbum *> albums;
};

#endif // ARTIST_H
