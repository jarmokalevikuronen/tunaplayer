#ifndef TPACTIVEFEEDMODEL_H
#define TPACTIVEFEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "tpfeed.h"

class TPActiveFeedModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TPActiveFeedModel(QObject *parent = 0);

signals:

private: // Types

    enum Roles
    {
        FeedTitleRole = Qt::UserRole + 1,
        FeedIdRole,
        FeedLinkRole,
        FeedDescriptionRole,
        FeedUrlRole,
        FeedSizeRole
    };

public slots:


public:

    void setFeed(TPFeed *_feed)
    {
        beginResetModel();
        feed = _feed;
        endResetModel();
    }

    inline TPFeed* getFeed() const
    {
        return feed;
    }

private: // From QAbstractListModel

    int rowCount(const QModelIndex &index) const
    {
        if (index.isValid() || !feed)
            return 0;

        return feed->count();
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || !feed)
            return QVariant();

        TPFeedItem *item = feed->at(index.row());

        if (role == Qt::DisplayRole || role == FeedTitleRole)
            return QVariant(item->getTitle());
        else if (role == FeedIdRole)
            return QVariant(item->getId());
        else if (role == FeedLinkRole)
            return QVariant(item->getLink());
        else if (role == FeedDescriptionRole)
            return QVariant(item->getDescription());
        else if (role == FeedUrlRole)
            return QVariant(item->getEnclosure());
        else if (role == FeedSizeRole)
            return QVariant(item->getSize());

        return QVariant();
    }

private:

    //! This is the currently active feed if any.
    TPFeed *feed;
};

#endif // TPACTIVEFEEDMODEL_H
