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

#include "tpfeedmgr.h"
#include <QFile>
#include <QStringList>
#include "tpfeed.h"
#include <QDebug>
#include "tppathutils.h"
#include "db/tpdbtemplate.h"
#include "tpfeed.h"

TPFeedMgr::TPFeedMgr(int refreshIntervalSeconds, QObject *parent) :
    QObject(parent),
    TPDBBase<TPFeed *>(this, TPPathUtils::getFeedsSettingsDbFilename(), schemeFeed)
{
    refreshInterval = refreshIntervalSeconds;
    feedDownloader = NULL;

    db->visitItems(this);

    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(startDownloadFeeds()));
    refreshTimer.setSingleShot(true);
    refreshTimer.start(1000);
}

TPFeedMgr::~TPFeedMgr()
{
    decAllObjects();
    delete feedDownloader;
}

void TPFeedMgr::visitAssociativeDBItem(TPAssociativeDBItem *item)
{
    objects.insertObject(new TPFeed(item, this));
}

void TPFeedMgr::createFeedObjects()
{
    QFile f(TPPathUtils::getFeedsDbFilename());

    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!f.atEnd())
        {
            QByteArray lineUtf8 = f.readLine().trimmed();
            QString line = QString::fromUtf8(lineUtf8);

            if (TPUtils::webAddress(line))
            {
                if (!objects.getObject(lineUtf8))
                    objects.insertObject(new TPFeed(line, db, this));
            }
        }
        f.close();
    }
}

void TPFeedMgr::startDownloadFeeds()
{
    if (feedDownloader)
        return;

    // In case configuration file changed,
    // This will create a new feed objects accordingly
    //. On the other hand, this will not
    // destroy possible existing feeds which needs
    // to be added there.
    createFeedObjects();

    feedDownloader = new TPFeedDownloader(this);
    Q_ASSERT(feedDownloader);

    connect(feedDownloader, SIGNAL(feedDownloaded(TPFeed*)), this, SLOT(feedDownloaded(TPFeed*)));

    for (int i=0;i<count();++i)
        feedDownloader->startDownloadFeed(at(i));

}

void TPFeedMgr::feedDownloaded(TPFeed *feed)
{
    qDebug() << "FEED: download complete from: " << feed->getUrl() << " ERROR: " << feed->getError();

    if (feed && !feed->getError().length())
    {
        if (feed->contentChanged())
        {
            feed->parseContent();
            emit feedChanged(feed);
        }
        qDebug() << "Feed Updated: " << *feed;
    }

    if (feedDownloader && !feedDownloader->downloadsPending())
    {
        qDebug() << "FEED: No downloads pending -> delete downloader";

        disconnect(feedDownloader,SIGNAL(feedDownloaded(TPFeed*)), this, SLOT(feedDownloaded(TPFeed*)));
        feedDownloader->deleteLater();
        feedDownloader = NULL;

        refreshTimer.setSingleShot(true);
        refreshTimer.start(refreshInterval * 1000);

        save(200);
        emit feedUpdateComplete();
    }
}


TPFeedItem* TPFeedMgr::getFeedItemById(const QString id)
{
    for (int i=0;i<count();++i)
    {
        TPFeed *feed = at(i);
        for (int i=0;i<feed->count();++i)
            if (feed->at(i)->getId() == id)
                return feed->at(i);

    }

    return NULL;
}


