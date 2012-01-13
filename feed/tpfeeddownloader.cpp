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

#include <QDebug>
#include "tpfeeddownloader.h"


TPFeedDownloader::TPFeedDownloader(QObject *parent) :
    QObject(parent)
{
    naManager = new QNetworkAccessManager(this);
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestComplete(QNetworkReply *)));
}

void TPFeedDownloader::startDownloadFeed(TPFeed *feed)
{
    naManager->get(QNetworkRequest(feed->getUrl()));
    feeds.append(feed);
    feed->inc();
}

void TPFeedDownloader::requestComplete(QNetworkReply *reply)
{
    QUrl requestUrl(reply->request().url());
    qDebug() << "FEED: DOWNLOAD: complete: " << requestUrl;

    QList< TPFeed *>::iterator it = feeds.begin();
    while (it != feeds.end())
    {
        TPFeed *feed = *it;
        if (feed->getUrl() == requestUrl)
        {
            if (reply->error())
                feed->setError(reply->errorString());
            else
                feed->setContent(reply->readAll());

            feeds.erase(it);
            emit feedDownloaded(feed);
            feed->dec();
            return;
        }
        ++it;
    }

    qDebug() << "FEED: ERROR: Url: " << requestUrl << " not known?";
}
