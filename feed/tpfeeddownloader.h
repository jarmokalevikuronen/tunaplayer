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

#ifndef TPFEEDDOWNLOADER_H
#define TPFEEDDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QVector>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "tpfeed.h"

//! @brief Responsible of downloading feeds in asynchronous manner
class TPFeedDownloader : public QObject
{
    Q_OBJECT

public:

    explicit TPFeedDownloader(QObject *parent = 0);

    void startDownloadFeed(TPFeed * feed);

    inline bool downloadsPending() const
    {
        return feeds.count() > 0;
    }

signals:

    void feedDownloaded(TPFeed *feed);

public slots:

private slots:

    void requestComplete(QNetworkReply *response);

private:


    QNetworkAccessManager *naManager;

    //! referred, not owned.
    QList< TPFeed *> feeds;
};

#endif // TPFEEDDOWNLOADER_H
