#ifndef TPYOUTUBESEARCH_H
#define TPYOUTUBESEARCH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include "tpassociative.h"
#include "tpyoutubeobject.h"

//!
//! @class TPYouTubeSearch
//! @brief Simple class that performs searches towards YouTube in order
//! to find videos that can then be listened using the mplayer + some additional
//! utility scripts (youtube-dl.py mainly).
//!
class TPYouTubeSearch : public QObject
{
    Q_OBJECT
public:

    explicit TPYouTubeSearch(QObject *parent = 0);

    ~TPYouTubeSearch();

    //! @brief Starts a search operation towards
    //! youtube with the given criteria
    bool search(const QString criteria);

    //! @brief Amount of search results
    int count();

    //! @brief search result item at given index.
    TPYouTubeObject* at(int index);

    //! @brief Takes the object
    TPYouTubeObject* takeById(const QString &identifier);

    //! @brief Find by ID
    TPYouTubeObject* findById(const QString &identifier);

private:

    QUrl buildUrl(const QString criteria);

signals:

    //! @brief emit once search has completed.
    void searchComplete();

private slots:

    void requestComplete(QNetworkReply *);

public slots:

private:

    //! Whether the request is in progress or not.
    QNetworkReply *requestInProcess;

    //! Object constructed.
    QVector<TPYouTubeObject *> objects;

    //! Network access manager if any.
    QNetworkAccessManager *nam;
};

#endif // TPYOUTUBESEARCH_H
