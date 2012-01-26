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

#ifndef TPAUTOMATICALBUMARTDOWNLOADER_H
#define TPAUTOMATICALBUMARTDOWNLOADER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "db/tpalbumdb.h"
#include "tplastfmimagesearch.h"

//! @class TPAutomaticAlbumArtDownloaer
//! @brief Automatically downloads album arts for albums not
//! having such art yet, and for which automatic download
//! has not been accomplished yet.
//! NOTE: This class only utilizes the LastFM service and
//! not at all the google search since google does not allow
//! their service to be used for automatic downloads.
class TPAutomaticAlbumArtDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TPAutomaticAlbumArtDownloader(QObject *parent = 0);
    ~TPAutomaticAlbumArtDownloader();

    void execute(TPAlbumDB *albumDB);

signals:

    void albumArtDownloaded(TPAlbum *album);
    void complete();

public slots:


private slots:

    void expectedImageCount(QObject *caller, int count);
    void imageDownloaded(QObject *caller, QImage image);
    void complete(QObject *caller);
    void downloadNextAlbum();

private:

    bool startDownloadNextAlbum();

private:

    // Contains a list of albums to which album art is missing and
    // has not been tried even once.
    QList<TPAlbum *> albumsToDownload;

    //! Image search implementation, uses last FM.
    TPLastFMImageSearch *is;

    //! Currently processed album
    TPAlbum *currentAlbum;

    //! Timer used to delay album art downloads a bit.
    QTimer *delayTimer;
};

#endif // TPAUTOMATICALBUMARTDOWNLOADER_H
