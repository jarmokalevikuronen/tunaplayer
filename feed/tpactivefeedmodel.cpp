#include "tpactivefeedmodel.h"

TPActiveFeedModel::TPActiveFeedModel(QObject *parent) :
    QAbstractListModel(parent)
{
    feed = NULL;

    QHash<int, QByteArray> roles;
    roles.insert(FeedTitleRole, "title");
    roles.insert(FeedIdRole, "id");
    roles.insert(FeedLinkRole, "link");
    roles.insert(FeedDescriptionRole, "description");
    roles.insert(FeedUrlRole, "url");
    roles.insert(FeedSizeRole, "size");
    setRoleNames(roles);
}
