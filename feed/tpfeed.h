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

#ifndef TPFEED_H
#define TPFEED_H

#include <QString>
#include <QUrl>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QVector>
#include <QDebug>
#include "tputils.h"
#include "tpidbase.h"
#include "tpschemes.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tpreferencecounted.h"
#include "db/tpdbtemplate.h"

class TPFeed;


QDebug operator<<(QDebug dbg, const TPFeed &feed);

class TPFeedItem : public TPIdBase, public TPAssociativeObject, public TPReferenceCounted
{
public:

    explicit TPFeedItem(TPAssociativeDBItem *dbItem) :
        TPIdBase(schemeFeedItem),
        TPAssociativeObject(dbItem)
    {
        addIdSource(getString(feedItemAttrEnclosure));
        addIdSource(getString(feedItemAttrParentUrl));
    }

    explicit TPFeedItem(const QString _url, const QString _parentUrl, TPAssociativeDB *db) :
        TPIdBase(schemeFeedItem),
        TPAssociativeObject(db->allocItem(_url + "@" + _parentUrl))
    {
        addIdSource(_url);
        addIdSource(_parentUrl);
        setString(feedItemAttrParentUrl, _parentUrl);
        setString(feedItemAttrEnclosure, _url);
    }

    ~TPFeedItem();

    inline void setTitle(const QString _title)
    {
        setString(objectAttrName, _title);
        setString(feedItemAttrTitle, _title);
    }

    inline void setLink(const QString _link)
    {
        setString(feedItemAttrLink, _link);
    }

    inline void setDescription(const QString _description)
    {
        setString(feedItemAttrDescription, _description);
    }

    inline void setSize(int _size)
    {
        setInt(feedItemAttrSize, _size);
    }

    inline const QString getTitle() const
    {
        return getString(feedItemAttrTitle);
    }

    inline const QString getLink() const
    {
        return getString(feedItemAttrLink);
    }

    inline const QString getDescription() const
    {
        return getString(feedItemAttrDescription);
    }

    inline const QString getEnclosure() const
    {
        return getString(feedItemAttrEnclosure);
    }

    inline int getSize() const
    {
        return getInt(feedItemAttrSize);
    }

    inline const QString getId()
    {
        return identifier();
    }

    const QString getString(const QString key, const QString defaultValue = "") const;
    QVariantMap toMap(QStringList *filteredKeys);
};

class TPFeed :
        public TPIdBase,
        public TPAssociativeObject,
        public TPReferenceCounted,
        public TPDBBase<TPFeedItem *>,
        public TPAssociativeDBItemVisitor
{
private: // From TPAssociativeDBItemVisitor

    void visitAssociativeDBItem(TPAssociativeDBItem *item);

public:

    TPFeed(const QString _url, TPAssociativeDB *_db, QObject *parent);
    TPFeed(TPAssociativeDBItem *dbItem, QObject *parent);
    ~TPFeed();

    inline const QUrl getUrl() const
    {
        return getString(feedAttrUrl);
    }

    inline void setContent(const QByteArray _content)
    {
        if (content != _content)
        {
            content = _content;
            internalContentChanged = true;
        }
    }

    inline void setError(QString error)
    {
        lastError = error;
    }

    inline QString getError() const
    {
        return lastError;
    }

    inline bool contentChanged() const
    {
        return internalContentChanged;
    }

    inline const QByteArray getContent() const
    {
        return content;
    }

    void parseContent();

    inline const QString getTitle() const
    {
        return getString(feedAttrTitle);
    }

    inline const QString getDescription() const
    {
        return getString(feedAttrDescription);
    }

    inline const QString getLink() const
    {
        return getString(feedAttrLink);
    }

    const QString getString(const QString key, const QString defaultValue = "") const;
    QVariantMap toMap(QStringList *filteredKeys);

private: // Data

    QString lastError;
    //QUrl url;
    QByteArray content;
    bool internalContentChanged;
};

#endif // TPFEED_H
