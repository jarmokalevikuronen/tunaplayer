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

#ifndef ALBUMARTDOWNLOADMODEL_H
#define ALBUMARTDOWNLOADMODEL_H

#include <QObject>
#include <QList>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QObject>
#include "tpalbum.h"
#include "albumartutil.h"




class TPAlbumArtDownloadModel : public QObject
{
    Q_OBJECT


public:

    explicit TPAlbumArtDownloadModel(QString _imageDir, QObject *parent = 0);

    inline void addImage(QImage image)
    {
        qDebug() << "addImage - count: " << images.count()+1;
        images.append(image);
    }

    bool saveImage(int index, TPAlbum *album)
    {
        if (index < 0 || index >= images.count())
            return false;

        return TPAlbumArtUtil::saveArt(imageDir, images.at(index), album);
    }

    void clearImages()
    {
        images.clear();
    }

private: // Data

    // Place where images can be saved.
    QString imageDir;

    // List of downloaded images.
    QList<QImage> images;
};

#endif // ALBUMARTDOWNLOADMODEL_H
