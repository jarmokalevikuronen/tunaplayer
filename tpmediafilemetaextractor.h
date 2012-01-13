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

#ifndef TPMEDIAFILEMETAEXTRACTOR_H
#define TPMEDIAFILEMETAEXTRACTOR_H

#include "tpassociative.h"
#include "tpassociativemeta.h"
#include <QFileInfo>

//! @class TPMediaFileMetaExtractor
//! @brief Interface class that is used to extract meta information, like length, artist, album, .. from
//! audio (.mp3, .wav, ..) files.
class TPMediaFileMetaExtractor
{
private: // Helper functions to exctract certain information also from
    // the filename instead of using id3, and other embedded, information tags.
    static QString trackNameFromFilename(QFileInfo fi);
    static QString artistNameFromFilename(QFileInfo fi);
    static QString albumNameFromFilename(QFileInfo fi);
    static int trackNumberFromFilename(QFileInfo fi);

public:

    static TPAssociativeDBItem *extractTrackInfo(const QString &filename, TPAssociativeDB *db);
};

#endif // TPMEDIAFILEMETAEXTRACTOR_H
