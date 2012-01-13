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

#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include "albumartutil.h"

bool TPAlbumArtUtil::saveArt(const QString imageDir, QImage image, TPAlbum *album)
{
    QString smallName = imageDir + album->generateAlbumArtFilename(ALBUM_ART_TYPE, TPAlbum::ESmallArt);
    QString largeName = imageDir + album->generateAlbumArtFilename(ALBUM_ART_TYPE, TPAlbum::EBigArt);

    QFile::remove(smallName);
    QFile::remove(largeName);

    QImage small(image.scaled(SMALL_ART_WIDTH, SMALL_ART_WIDTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QImage large(image.scaled(LARGE_ART_WIDTH, LARGE_ART_WIDTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    return small.save(smallName) && large.save(largeName);
}
