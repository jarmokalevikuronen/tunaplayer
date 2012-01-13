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

#include "tpfeed.h"
#include "tppathutils.h"
#include <QDebug>


const QString TPFeedItem::getString(const QString key, const QString defaultValue) const
{
    if (key == objectAttrIdentifier)
        return const_cast<TPFeedItem *>(this)->identifier();

    return TPAssociativeObject::getString(key, defaultValue);
}

QVariantMap TPFeedItem::toMap(QStringList *filteredKeys)
{
    QVariantMap result = TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(objectAttrIdentifier))
            result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }
    else
    {
        result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }

    return result;
}


QDebug operator<<(QDebug dbg, const TPFeed &feed)
{
    dbg.nospace() << "TPFeed  { items=" << feed.count() << " url=" << feed.getUrl().toString() << " }";
    return dbg.space();
}

TPFeed::TPFeed(const QString _url, TPAssociativeDB *_db, QObject *parent) :
    TPIdBase(schemeFeed, _url),
    TPAssociativeObject(_db->allocItem(_url)),
    TPDBBase<TPFeedItem *>(parent, TPPathUtils::getFeedItemsSettingsDbFilename(), schemeFeedItem)
{
    internalContentChanged = false;
    lastError = "";
    setString(feedAttrUrl, _url);
    setString(objectAttrScheme, schemeFeed);
    setString(objectAttrName, _url);
}

TPFeed::TPFeed(TPAssociativeDBItem *dbItem, QObject *parent) :
    TPIdBase(schemeFeed, dbItem->stringValue(feedAttrUrl)),
    TPAssociativeObject(dbItem),
    TPDBBase<TPFeedItem *>(parent, TPPathUtils::getFeedItemsSettingsDbFilename(), schemeFeedItem)
{
    // OK, we kind of can assume
    // scheme and other stuff is properly set up fro the dbItem here.
    internalContentChanged = false;
    lastError = "";
    db->visitItems(this);
}

TPFeed::~TPFeed()
{
    decAllObjects();
}

void TPFeed::visitAssociativeDBItem(TPAssociativeDBItem *dbItem)
{
    objects.insertObject(new TPFeedItem(dbItem));
}

void TPFeed::parseContent()
{
    if (internalContentChanged)
    {
        internalContentChanged = false;

        QDomDocument dom("rss");
        if (!dom.setContent(content))
            return;

        QDomElement de = dom.documentElement();

        QDomElement channel = de.firstChildElement("channel");

        QDomNodeList nodeList = channel.childNodes();
        for (int i=0;i<nodeList.count();++i)
        {
            QDomElement e = nodeList.at(i).toElement();

            if (e.tagName() == "title")
            {
                setString(feedAttrTitle, e.text());
                setString(objectAttrName, e.text());
            }
            else if (e.tagName() == "link")
                setString(feedAttrLink, e.text());
            else if (e.tagName() == "description")
                setString(feedAttrDescription, e.text());
            else if (e.tagName() == "item")
            {
                QDomNodeList itemNodes = e.childNodes();

                QString title, link, description, url, size;

                for (int j=0;j<itemNodes.count();++j)
                {
                    QDomElement e2 = itemNodes.at(j).toElement();
                    if (e2.tagName() == "title")
                        title = e2.text();
                    else if (e2.tagName() == "link")
                        link = e2.text();
                    else if (e2.tagName() == "description")
                        description = e2.text();
                    else if (e2.tagName() == "enclosure")
                    {
                        if (e2.hasAttribute("url"))
                            url = e2.attribute("url");
                        if (e2.hasAttribute("size"))
                            size = e2.attribute("size");
                    }
                }

                if (url.length())
                {
                    TPFeedItem *item = new TPFeedItem(url, getString(feedAttrUrl), db);
                    TPFeedItem *existingItem =
                            getById(item->identifier(false));

                    if (existingItem)
                    {
                        delete item;
                        item = existingItem;
                    }

                    if (title.length())
                        item->setTitle(title);
                    if (link.length())
                        item->setLink(link);
                    if (description.length())
                        item->setDescription(description);
                    if (size.length())
                        item->setSize(size.toInt());

                    // Mark item updated..
                    item->setInt(objectAttrUpdated, TPUtils::currentEpoch());

                    // This will internally deal with the
                    // adding of some item twice.
                    objects.insertObject(item);
                }
            }
        }
    }

    executePostCreateTasks();
    TPDBBase<TPFeedItem *>::save(1000);
}

const QString TPFeed::getString(const QString key, const QString defaultValue) const
{
    if (key == objectAttrIdentifier)
        return const_cast<TPFeed *>(this)->identifier();

    return TPAssociativeObject::getString(key, defaultValue);
}

QVariantMap TPFeed::toMap(QStringList *filteredKeys)
{
    QVariantMap result = TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(objectAttrIdentifier))
            result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }
    else
    {
        result.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }

    return result;
}

