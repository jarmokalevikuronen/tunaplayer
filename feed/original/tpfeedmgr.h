#ifndef TPFEEDMGR_H
#define TPFEEDMGR_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "tpfeed.h"
#include "tpfeeddownloader.h"

class TPFeedMgr : public QObject
{
    Q_OBJECT

public:

    explicit TPFeedMgr(int refreshIntervalSeconds, const QString feeds, QObject *parent = 0);

signals:

private slots:

    void feedDownloaded(TPFeed *feed);

private: // New Impl

    void createFeedObjects();
    void startDownloadFeeds();

private:

    QTimer refreshTimer;
    TPFeedDownloader *feedDownloader;
    int refreshInterval;
    QVector< TPFeed *> feedHandlers;

    QString feedsDbFile;
};

#endif // TPFEEDMGR_H
