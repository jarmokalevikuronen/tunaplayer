#include "tpfeedmgr.h"
#include <QFile>
#include <QStringList>
#include "tpfeed.h"
#include <QDebug>

TPFeedMgr::TPFeedMgr(int refreshIntervalSeconds, const QString _feedsDbFile, QObject *parent) :
    QObject(parent)
{
    refreshInterval = refreshIntervalSeconds;
    feedsDbFile = _feedsDbFile;
    feedDownloader = NULL;

    createFeedObjects();
    startDownloadFeeds();
}

void TPFeedMgr::createFeedObjects()
{
    QFile f(feedsDbFile);

    QStringList addedUrls;

    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!f.atEnd())
        {
            QString line = QString::fromUtf8(f.readLine()).trimmed();

            // Just some sanity checks to add same url only once.
            if (line.length() > 5 && !addedUrls.contains(line))
            {
                feedHandlers.append(new TPFeed(line));
                addedUrls.append(line);
            }
        }
        f.close();
    }
}

void TPFeedMgr::startDownloadFeeds()
{
    if (feedDownloader)
        return;

    feedDownloader = new TPFeedDownloader(this);

    connect(feedDownloader, SIGNAL(feedDownloaded(TPFeed*)), this, SLOT(feedDownloaded(TPFeed*)));
    feedDownloader->startDownloadFeeds(feedHandlers);
}

void TPFeedMgr::feedDownloaded(TPFeed *feed)
{
    qDebug() << "FEED: download complete from: " << feed->getUrl() << " ERROR: " << feed->getError();

    if (feed && !feed->getError().length())
    {
        // if feed-is-visible-one
        // currentFeed->startResetModel()
        feed->parseContent();
        // currentFeed->endResetModel()

        qDebug() << "Feed Updated: " << *feed;
    }

    if (feedDownloader && !feedDownloader->downloadsPending())
    {
        qDebug() << "FEED: No downloads pending -> delete downloader";
        disconnect(feedDownloader,SIGNAL(feedDownloaded(TPFeed*)), this, SLOT(feedDownloaded(TPFeed*)));
        feedDownloader->deleteLater();
        feedDownloader = NULL;

        // TODO: Reschedule update for example!
    }
}


