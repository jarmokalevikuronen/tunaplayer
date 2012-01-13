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

#ifndef TPFEEDMGR_H
#define TPFEEDMGR_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "tpfeed.h"
#include "tpfeeddownloader.h"
#include "db/tpdbtemplate.h"

class TPFeedMgr : public QObject, public TPDBBase<TPFeed *>, public TPAssociativeDBItemVisitor
{
    Q_OBJECT

public:

    explicit TPFeedMgr(int refreshIntervalSeconds, QObject *parent = 0);
    ~TPFeedMgr();

private: // From

    void visitAssociativeDBItem(TPAssociativeDBItem *item);

signals:

    void feedChanged(TPFeed *);
    void feedUpdateComplete();

private slots:

    void feedDownloaded(TPFeed *feed);

public:

    TPFeedItem* getFeedItemById(const QString id);

private: // New Impl

    void createFeedObjects();

private slots:

    void startDownloadFeeds();

private:

    //! Downloader instance, used to do the http
    TPFeedDownloader *feedDownloader;

    //! Timer for scheduled feed upgrades
    QTimer refreshTimer;

    //! How often upgrade is to be done. seconds.
    int refreshInterval;
};

#endif // TPFEEDMGR_H
