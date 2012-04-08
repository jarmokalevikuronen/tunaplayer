#include "tpusermanager.h"
#include <QStringList>
#include <QFileInfo>
#include "tplog.h"

static const QString opAllow("allow");
static const QString opRestrict("deny");

// Default TAG -
static const QString tagDefault("all");

TPUserTag::TPUserTag(const QString &tagPath, const QString &tagFilename)
{
    QStringList tokens = tagFilename.split(QChar('.'));

    path = tagPath.trimmed().toLower();
    accessType = Invalid;

    if (tokens.count() == 3)
    {
        tag = tokens.at(0).trimmed().toLower();

        QString accType = tokens.at(1);

        if (accType == opAllow)
            accessType = Allow;
        else if (accType == opRestrict)
            accessType = Restrict;

        DEBUG() << "ACCESSTOKEN: TAG=" << tag << " ACCESS=" << accType << " LOCATION=" << path;
    }
}


TPUserManager::TPUserManager()
{
}

TPUserManager::~TPUserManager()
{
    qDeleteAll(tags);
}

bool TPUserManager::upgrade(QVector<TPUserTag *> *items)
{
    if (!items)
        return false;

    if (equals(*items))
    {
        DEBUG() << "ACCESSTOKEN: NoChanges";
        qDeleteAll((*items));
        delete items;
        return false;
    }

    // Delete old ones, copy and delete the container (but not the contents).
    qDeleteAll(tags);
    tags = *items;
    delete items;

    return true;
}

bool TPUserManager::insertTagFile(const QString &tagFilename)
{
    QFileInfo fi(tagFilename);

    TPUserTag *t = new TPUserTag(fi.absolutePath(), fi.fileName());
    Q_ASSERT(t);

    if (!t->isValid())
    {
        delete t;
        return false;
    }

    if (hasEqualTag(*t))
    {
        // In principle this can not be reached if class is to be used
        // properly. better be safe than sorry though.
        delete t;
        return false;
    }

    tags.append(t);

    // Was Inserted
    return true;
}

QStringList TPUserManager::allTags()
{
    QStringList list;

    foreach (TPUserTag *tag, tags)
    {
        if (!list.contains(tag->tag))
            list.append(tag->tag);
    }

    list.append(tagDefault);
    list.sort();

    return list;
}

QString TPUserManager::tagStringForFile(const QString &filename, const QString pre, const QString post, const QString delimiter)
{
    QString result;
    QStringList t = tagsForFile(filename);

    for (int i=0;i<t.count();i++)
    {
        result += pre;
        result += t.at(i);
        result += post;

        if ((i+1) < t.count())
            result += delimiter;
    }

    return result;
}

QStringList TPUserManager::tagsForFile(const QString &filename)
{
    QString lc = filename.toLower();

    QVector<TPUserTag *> matches;

    foreach(TPUserTag *tag, tags)
    {
        // For Every known TAG
        if (lc.startsWith(tag->path))
        {
            bool replaced = false;
            for (int i=0;!replaced && i<matches.count();i++)
            {
                TPUserTag *existingtag = matches.at(i);

                // For Every already matched item... try to find the best having
                // the same tag name...
                if (tag->tag == existingtag->tag)
                {
                    if (tag->path.length() > existingtag->path.length())
                    {
                        // It was a better match. Do replace and go away.
                        matches.remove(i);
                        replaced = true;
                    }
                }
            }
            if (!replaced)
                matches.append(tag);
        }
    }

    // Construct a stringlist from the items accordingly.
    QStringList list;
    foreach(TPUserTag *t, matches)
    {
        // If the best matching tag is to allow -> do allow it.
        if (t->allowedAccess())
            list.append(t->tag);
    }
    list.append(tagDefault);

    return list;
}

bool TPUserManager::equals(const QVector<TPUserTag *> &other)
{
    if (tags.count() != other.count())
        return false;

    foreach(TPUserTag *t, other)
    {
        if (!hasEqualTag(*t))
            return false;
    }

    return true;
}

bool TPUserManager::equals(const TPUserManager &other)
{
    return equals(other.tags);
}

bool TPUserManager::hasEqualTag(const TPUserTag &other)
{
    foreach(TPUserTag *tag, tags)
    {
        if (tag->equals(other))
            return true;
    }
    return false;
}
