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

#ifndef TPTRACKDB_H
#define TPTRACKDB_H

#include <QObject>
#include "tptrack.h"
#include "tpdbtemplate.h"
#include "tpassociative.h"
#include "tpartistdb.h"
#include "tpalbumdb.h"

class TPTrackDB : public QObject, public TPDBBase<TPTrack *>, public TPAssociativeDBItemVisitor
{
    Q_OBJECT

private: // From TPAssociativeDBItemVisitor

    void visitAssociativeDBItem(TPAssociativeDBItem *item);

public:

    explicit TPTrackDB(QObject *parent = 0);
    ~TPTrackDB();

    //! @brief Gets list of all the filenames known
    //! @return list of all the filenames currently known
    //! The ownership of the returned pointer transfers to caller.
    QStringList* getTrackFilenames() const;

    TPTrack* getTrackByFilename(const QString filename);
    void processFiles(const QStringList files);

    inline TPAssociativeDB *associativeDb() const
    {
        return db;
    }

    inline TPArtistDB *getArtistDB() const
    {
        return artistDB;
    }

    inline TPAlbumDB* getAlbumDB() const
    {
        return albumDB;
    }

    void insertItem(TPAssociativeDBItem *dbItem);

    void executePostCreateTasks();

private: // DAta


    //! Artist DB. owned.
    TPArtistDB *artistDB;

    //! Album DB. owned.
    TPAlbumDB *albumDB;

    //! Member variable used only during the construction
    //! to collect a list of removed tracks.
    QVector<TPAssociativeDBItem *> removedItems;
};

#endif // TPTRACKDB_H
