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

#include <QFile>
#include <QDir>
#include "albumartdownloader.h"
#include "googleimagesearch.h"
#include "tplastfmimagesearch.h"
#include "albumartdownloadrequest.h"
#include "tputils.h"
#include "tppathutils.h"
#include "albumartutil.h"
#include "tplog.h"

#define TP_IMAGE_EXT ".PNG"

TPAlbumArtDownloader::TPAlbumArtDownloader(QObject *parent) :
    QObject(parent)
{
    totalImagesDownloaded = 0;
    google = 0;
    lastfm = 0;
    request = 0;
    nextIndex = TPUtils::currentEpoch();
}

TPAlbumArtDownloader::~TPAlbumArtDownloader()
{
    reset();
}

bool TPAlbumArtDownloader::exec(TPAlbumArtDownloadRequest *_request, const QString services)
{
    Q_ASSERT(_request);

    if (!google && (services.contains("google") || services.length() == 0))
    {
        google = new TPGoogleImageSearch(this);
        QObject::connect(google, SIGNAL(expectedImageCount(QObject*,int)), this, SLOT(handleExpectedImageCount(QObject*,int)));
        QObject::connect(google, SIGNAL(imageDownloaded(QObject *, QImage)), this, SLOT(imageDownloaded(QObject *, QImage)));
        QObject::connect(google, SIGNAL(complete(QObject *)), this, SLOT(imageDownloadComplete(QObject *)));
        QObject::connect(google, SIGNAL(connectivityLost(QObject *)), this, SLOT(imageDownloadComplete(QObject *)));
    }
    if (!lastfm && (services.contains("lastfm") || services.length() == 0))
    {
        lastfm = new TPLastFMImageSearch(this);
        QObject::connect(lastfm, SIGNAL(complete(QObject*)), this, SLOT(imageDownloadComplete(QObject*)));
        QObject::connect(lastfm, SIGNAL(expectedImageCount(QObject*,int)), this, SLOT(handleExpectedImageCount(QObject*,int)));
        QObject::connect(lastfm, SIGNAL(imageDownloaded(QObject*,QImage)), this, SLOT(imageDownloaded(QObject*,QImage)));
        QObject::connect(lastfm, SIGNAL(connectivityLost(QObject *)), this, SLOT(imageDownloadComplete(QObject *)));
    }

    // We serve only one client at a time.
    if (request)
        return false;

    images.clear();
    cachedIndexes.clear();

    // Some maximum value here, even though that is not likely ever met.
    for (int i = 1; i < 1024; ++i)
    {
        QString filename = TPPathUtils::getAlbumArtDownloadFolder() + QString::number(i) + TP_IMAGE_EXT;
        bool success = QFile::remove(filename);
        if (!success)
            break;
    }

    totalImagesDownloaded = 0;
    request = _request;
    googleImages = -1;
    lastfmImages = -1;
    googleComplete = lastfmComplete = false;

    // TODO: We should cancel the latter if earlier fails.
    if (google && !google->startSearch(request->artistName(), request->albumName()))
        return false;

    if (lastfm && !lastfm->startSearch(request->artistName(), request->albumName()))
        return false;

    cleanDownloadFolder();

    return true;
}

void TPAlbumArtDownloader::imageDownloaded(QObject *caller, QImage image)
{
    Q_UNUSED(caller);

    int key = getNextIndex();
    DEBUG() << "ALBUMART: nextIndex: " << key;

    ++totalImagesDownloaded;    

    QString filename = TPPathUtils::getAlbumArtDownloadFolder() + QString::number(key) + TP_IMAGE_EXT;
    // TODO: Some utility to be used so that we won't hardcode 400 around the software!!!!
    image.scaled(400, 400).save(filename);

    bool canReportProgress = googleImages >= 0 && lastfmImages >= 0;

    int totalExpected = qMax(googleImages, 0) + qMax(lastfmImages, 0);
    if (totalExpected > 0)
    {
        DEBUG() << "ALBUMART: DOWNLOADED [" << totalImagesDownloaded << "] / [" << totalExpected << "]";
        int percents = (totalImagesDownloaded * 100.0) / totalExpected;
        if (canReportProgress)
        {
            for (int i=0;i<cachedIndexes.count();++i)
                emit albumArtDownloaded(cachedIndexes.at(i));
            cachedIndexes.clear();

            emit albumArtDownloaded(key);
            emit downloadProgress(percents, request, image);
        }
        else
            cachedIndexes.append(key);

        images.insert(key, image);
    }
}

void TPAlbumArtDownloader::imageDownloadComplete(QObject *caller)
{
    if (caller == google)
        googleComplete = true;
    else if (caller == lastfm)
        lastfmComplete = true;
    else
        ERROR() << "ALBUMART: unknown caller in " << __func__;

    if (googleComplete && lastfmComplete)
    {
        emit downloadComplete();
        delete request; request = NULL;
    }
}

void TPAlbumArtDownloader::handleExpectedImageCount(QObject *caller, int count)
{
    if (caller == google)
    {
        DEBUG() << "ALBUMART: GOOGLE: Potential images: " << count;
        googleImages = count;
    }
    else if (caller == lastfm)
    {
        DEBUG() << "ALBUMART: LASTFM: Potential images: " << count;
        lastfmImages = count;
    }
    else
        DEBUG() << "ERROR: unknown caller in " << __func__;
}

const QString TPAlbumArtDownloader::indexToFilename(int index)
{
    return QString::number(index) + TP_IMAGE_EXT;
}

const QString TPAlbumArtDownloader::getFullPathToFile(int index)
{
    return TPPathUtils::getAlbumArtDownloadFolder() + indexToFilename(index);
}

int TPAlbumArtDownloader::idToIndex(const QString id)
{
    if (!id.startsWith(schemeDlAlbumArt))
    {
        ERROR() << "ALBUMART: Invalid index - " + id;
        return -1;
    }

    bool ok = false;
    int index = id.mid(schemeDlAlbumArt.length()).toInt(&ok);

    return ok ? index : -1;
}

bool TPAlbumArtDownloader::saveImage(const QString id, TPAlbum *album)
{
    int index = idToIndex(id);
    if (index < 0)
        return false;

    if (images.contains(index))
        return TPAlbumArtUtil::saveArt(TPPathUtils::getAlbumArtFolder(), images.find(index).value(), album);

    return false;
}

int TPAlbumArtDownloader::getNextIndex()
{
    return ++nextIndex;
}

void TPAlbumArtDownloader::cleanDownloadFolder()
{
    QDir d(TPPathUtils::getAlbumArtDownloadFolder());
    QFileInfoList entries = d.entryInfoList();
    for (int i=0;i<entries.count();++i)
    {
        if (entries.at(i).isFile())
        {
            QFile::remove(entries.at(i).absoluteFilePath());
        }
    }
}
