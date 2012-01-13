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

#ifndef ALBUMARTDOWNLOADREQUEST_H
#define ALBUMARTDOWNLOADREQUEST_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "tpartist.h"
#include "tpalbum.h"

class TPAlbumArtDownloadRequest : public QObject
{
Q_OBJECT;

public:

    TPAlbumArtDownloadRequest(TPAlbum *_album, int _maxArts = 4, QObject *parent = 0);
    ~TPAlbumArtDownloadRequest();

    void setMaxArts(int _maxArts) { maxArts = _maxArts; }
    int getMaxArts() const { return maxArts; }

    inline const QString artistName() const
    {
        return album->getArtist()->getName();
    }

    inline const QString albumName() const
    {
        return album->getName();
    }

    inline TPAlbum *getAlbum() const
    {
        return album;
    }

private:

    //! The album related to the download request.
    TPAlbum *album;

    //! Maximum arts to download.
    int maxArts;
};

#endif // ALBUMARTDOWNLOADREQUEST_H
