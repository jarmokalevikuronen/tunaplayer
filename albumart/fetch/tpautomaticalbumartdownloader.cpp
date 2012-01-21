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

#include "tpautomaticalbumartdownloader.h"
#include "tputils.h"
#include "tpassociativemeta.h"
#include "tpalbum.h"
#include "albumartutil.h"
#include "tppathutils.h"
#include "tplog.h"

TPAutomaticAlbumArtDownloader::TPAutomaticAlbumArtDownloader(QObject *parent) :
    QObject(parent)
{
    currentAlbum = NULL;
    delayTimer = new QTimer(this);
    delayTimer->setSingleShot(true);
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(downloadNextAlbum()));
}

void TPAutomaticAlbumArtDownloader::execute(TPAlbumDB *db)
{
    TPDecForAll(albumsToDownload);

    for (int i=0;i<db->count();++i)
    {
        TPAlbum *album = db->at(i);
        if (!album->hasAlbumArt(TPAlbum::ESmallArt))
        {
            // We will try max. 5 times to download a specific album art.
            int triesLeft = album->getInt(albumAttrAutomaticAlbumArtDownloadTries, 3);

            if (triesLeft > 0)
            {
                album->setInt(albumAttrAutomaticAlbumArtDownloadTries, triesLeft - 1);
                album->save(10 * 10000);
                albumsToDownload.append(album);
                album->inc();
            }
        }
    }

    DEBUG() << "ALBUMART: albumsToDownload: " << albumsToDownload.count();

    if (albumsToDownload.count())
    {
        is = new TPLastFMImageSearch(this);
        is->setMaxImageCount(1);
        connect(is, SIGNAL(expectedImageCount(QObject*,int)), this, SLOT(expectedImageCount(QObject*,int)));
        connect(is, SIGNAL(imageDownloaded(QObject*,QImage)), this, SLOT(imageDownloaded(QObject*,QImage)));
        connect(is, SIGNAL(complete(QObject*)), this, SLOT(complete(QObject*)));

        startDownloadNextAlbum();
    }
}

void TPAutomaticAlbumArtDownloader::expectedImageCount(QObject */*caller*/, int count)
{
    Q_UNUSED(count);
    Q_ASSERT(count <= 1);
}

void TPAutomaticAlbumArtDownloader::imageDownloaded(QObject */*caller*/, QImage image)
{
    Q_ASSERT(currentAlbum);

    DEBUG() << "imageDownloaded..";

    // Process here the downloaded image
    if (!image.isNull())
    {
        bool ok = TPAlbumArtUtil::saveArt(TPPathUtils::getAlbumArtFolder(), image, currentAlbum);
        if (ok)
        {
            currentAlbum->setInt(albumAttrAutomaticAlbumArtDownloadTries, 0);
            currentAlbum->save(1000 * 10);
        }
        emit albumArtDownloaded(currentAlbum);
    }
    else
    {
        currentAlbum->setInt(albumAttrAutomaticAlbumArtDownloadTries, 0);
        currentAlbum->save(1000 * 10);
    }
}

void TPAutomaticAlbumArtDownloader::complete(QObject */*caller*/)
{
    Q_ASSERT(is);



    if (!startDownloadNextAlbum())
    {
        if (is)
            is->deleteLater();
        is = NULL;
        emit complete();
    }
}

bool TPAutomaticAlbumArtDownloader::startDownloadNextAlbum()
{
    Q_ASSERT(is);

    DEBUG() << "startDownloadNextAlbum: remaining: " << albumsToDownload.count();

    if (albumsToDownload.count() <= 0)
        return false;

    if (currentAlbum)
        currentAlbum->dec();

    currentAlbum = albumsToDownload.takeFirst();

    // We will not do sort of busy downloads, but
    // wait for a period in between requests.
    delayTimer->start(500);
    return true;
}

void TPAutomaticAlbumArtDownloader::downloadNextAlbum()
{
    Q_ASSERT(currentAlbum);
    Q_ASSERT(is);

    is->startSearch(currentAlbum->getArtist()->getName(), currentAlbum->getName());
}
