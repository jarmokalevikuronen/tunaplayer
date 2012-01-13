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

#ifndef TPALBUMDB_H
#define TPALBUMDB_H

#include <QObject>
#include "tpdbtemplate.h"
#include "tpassociative.h"
#include "tpalbum.h"

class TPAlbumDB : public QObject, public TPDBBase<TPAlbum *>
{
    Q_OBJECT

public:
    explicit TPAlbumDB(QObject *parent = 0);
    ~TPAlbumDB();

    TPAlbum* createAlbum(TPArtist *artist, const QString albumName);
    TPAlbum* createAlbumWithoutArtist(const QString albumName);

signals:

public slots:

};

#endif // TPALBUMDB_H
