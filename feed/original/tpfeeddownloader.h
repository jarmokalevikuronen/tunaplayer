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

    void startDownloadFeeds(QVector<TPFeed *> feeds);

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
