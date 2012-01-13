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
#include "albumartdownloadmodel.h"
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

    bool exec(TPAlbumArtDownloadRequest *_request, const QString services = "");

    inline bool busy() const
    {
        return (request != NULL);
    }

    inline TPAlbum *currentAlbum() const
    {
        if (request)
            return request->getAlbum();

        return NULL;
    }

    int idToIndex(const QString id)
    {
        if (!id.startsWith(schemeDlAlbumArt))
            return -1;

        bool ok = false;
        int index = id.mid(schemeDlAlbumArt.length()).toInt(&ok);

        return ok ? index : -1;
    }

    const QString getFullPathToFile(int index);
    const QString indexToFilename(int index);

    TPAlbumArtDownloadModel *getModel() const
    {
        return model;
    }

    bool saveImage(const QString id, TPAlbum *album)
    {
        int index = idToIndex(id);
        if (index < 0)
            return false;

        return model->saveImage(index, album);
    }

    void reset()
    {
        model->clearImages();
    }

signals:

    void downloadProgress(int percents, TPAlbumArtDownloadRequest *request, QImage image);
    void downloadComplete();
    void albumArtDownloaded(int index);

private slots:

    void handleExpectedImageCount(QObject *caller, int count);
    void imageDownloaded(QObject *caller, QImage image);
    void imageDownloadComplete(QObject *caller);

private:

    int percentsComplete;

    //! Album art download request
    TPAlbumArtDownloadRequest *request;

    int totalImagesDownloaded;

    //! Class doing searches using google image search API (json)
    TPGoogleImageSearch *google;
    int googleImages;
    bool googleComplete;

    //! class doing searches using last.fm webservice API (xml)
    TPLastFMImageSearch *lastfm;
    int lastfmImages;
    bool lastfmComplete;


    // Download mode if any.
    TPAlbumArtDownloadModel *model;

    QList<int>  cachedIndexes;
};

#endif // ALBUMARTDOWNLOADER_H
