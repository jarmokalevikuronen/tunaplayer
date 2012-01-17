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

#ifndef ALBUMARTDOWNLOADER_H
#define ALBUMARTDOWNLOADER_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include "albumartdownloadrequest.h"
#include "tpschemes.h"

class TPAlbumArtDownloadRequest;
class TPGoogleImageSearch;
class TPLastFMImageSearch;
class Album;

//! @class TPAlbumArtDownloader
//! @brief Implements album art downloads from various services (lastfm, google, ..)
class TPAlbumArtDownloader : public QObject
{
    Q_OBJECT

public:

    explicit TPAlbumArtDownloader(QObject *parent = 0);
    ~TPAlbumArtDownloader();

    bool exec(TPAlbumArtDownloadRequest *_request, const QString services = "");

    inline bool busy() const
    {
        return (request != NULL);
    }

    inline TPAlbum *currentAlbum() const
    {
        return request ? request->getAlbum() : 0;
    }

    //! @brief Returns a full path to file specified by a index
    const QString getFullPathToFile(int index);

    //! @brief Converts a index to a filename.
    const QString indexToFilename(int index);

    //! @brief Saves downloaded image as album art of the given album
    //! @param id idenfier of the downloaded album art to save
    //! @param album album that this downloaded art shall be bound to.
    bool saveImage(const QString id, TPAlbum *album);

    inline void reset()
    {
        images.clear();
        cachedIndexes.clear();
    }

private:

    int idToIndex(const QString id);
    int getNextIndex();
    void cleanDownloadFolder();

signals:

    void downloadProgress(int percents, TPAlbumArtDownloadRequest *request, QImage image);
    void downloadComplete();
    void albumArtDownloaded(int index);

private slots:

    void handleExpectedImageCount(QObject *caller, int count);
    void imageDownloaded(QObject *caller, QImage image);
    void imageDownloadComplete(QObject *caller);

private:

    //! Completion percentage.
    int percentsComplete;

    //! Album art download request
    TPAlbumArtDownloadRequest *request;

    //! Total amount of image downloaded so far.
    int totalImagesDownloaded;

    //! Class doing searches using google image search API (json)
    TPGoogleImageSearch *google;
    int googleImages;
    bool googleComplete;

    //! class doing searches using last.fm webservice API (xml)
    TPLastFMImageSearch *lastfm;
    int lastfmImages;
    bool lastfmComplete;

    QList<int>  cachedIndexes;


    QMap<int, QImage>   images;

    //! Cached value that is used to track previous index.
    int nextIndex;
};

#endif // ALBUMARTDOWNLOADER_H
