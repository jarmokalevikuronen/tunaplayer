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

#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H

#include <QVector>
#include <QMap>
#include <QString>
#include "tptrack.h"
#include "tpalbum.h"

//! @class TPAudioUtils
//! @brief Miscellaneous utility functions for artist name etc. handling
class TPAudioUtils
{
public:

    static bool albumsEqual(const TPAlbum &_1, const TPAlbum &_2);
    static bool albumsEqual(const TPAlbum &album, const QString name);
    static bool artistsEqual(const TPArtist &_1, QString name);
    static bool artistsEqual(QString name1, QString name2);
    static bool album1BeforeAlbum2(const TPAlbum &_1, const TPAlbum &_2);
    static bool artist1BeforeArtist2(const TPArtist &_1, const TPArtist &_2);
    static const QString normalizeArtistName(const QString &originalName);

private:

    static void buildArtistNameMapping();

private:

    //! Flag variable to contain initialization
    static bool artistNameMappingInitialized;
    //! some sort of manual normalization of artist
    //! names. Basically just name value pairs.
    static QMap<QString, QString> artistNameMapping;
};

#endif // AUDIOUTILS_H
