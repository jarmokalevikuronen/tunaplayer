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

#ifndef TPARTISTDB_H
#define TPARTISTDB_H

#include <QObject>
#include "tpdbtemplate.h"
#include "tpartist.h"

class TPArtistDB : public QObject, public TPDBBase<TPArtist *>
{
    Q_OBJECT

public:
    explicit TPArtistDB(QObject *parent = 0);
    ~TPArtistDB();

    //! @brief Creates, or returns, existing artist with
    //! given name. Some "intelligence" is applied
    //! when trying to match possible existing same named artist.
    TPArtist* createArtist(const QString name);

private:

    TPArtist* find(const QString name) const;

signals:

public slots:
};

#endif // TPARTISTDB_H
