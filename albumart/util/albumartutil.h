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

#ifndef ALBUMARTUTIL_H
#define ALBUMARTUTIL_H

#include <QPixmap>
#include "tpalbum.h"

#define LARGE_ART_WIDTH     400
#define SMALL_ART_WIDTH     200

//! @class TPAlbumArtUtil
//! @brief Collection of functions used in album art saving etc.
class TPAlbumArtUtil
{
public:
    static bool saveArt(const QString imageDir, QImage image, TPAlbum *album);

};

#endif // ALBUMARTUTIL_H
