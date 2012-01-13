#ifndef TPFEEDMODEL_H
#define TPFEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "tpfeed.h"

class TPFeedModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TPFeedModel(QObject *parent = 0);

    void setFeed(TPFeed *_feed);

    inline bool containsFeed(TPFeed *_feed) const
    {
        return _feed == feed;
    }

signals:

public slots:



private:

    //! Feed that is "contained"
    TPFeed *feed;
};

#endif // TPFEEDMODEL_H
