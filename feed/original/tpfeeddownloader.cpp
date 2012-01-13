#include <QDebug>
#include "tpfeeddownloader.h"


TPFeedDownloader::TPFeedDownloader(QObject *parent) :
    QObject(parent)
{
    naManager = new QNetworkAccessManager(this);
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestComplete(QNetworkReply *)));
}

void TPFeedDownloader::startDownloadFeeds(QVector<TPFeed *> newFeeds)
{
    for (int i=0;i<newFeeds.count();++i)
    {
        TPFeed *feed = newFeeds.at(i);
        if (feed)
        {
            naManager->get(QNetworkRequest(feed->getUrl()));
            feeds.append(feed);
        }
    }
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
            return;
        }
        ++it;
    }

    qDebug() << "FEED: ERROR: Url: " << requestUrl << " not known?";
}
