#include "tpfeed.h"
#include <QDebug>

QDebug operator<<(QDebug dbg, const TPFeed &feed)
{
    dbg.nospace() << "TPFeed  { items=" << feed.count() << " url=" << feed.getUrl().toString() << " }";
    return dbg.space();
}

TPFeed::TPFeed(const QString _url) : url(_url)
{    
    internalContentChanged = false;
    lastError = "";
}

void TPFeed::parseContent()
{
    if (internalContentChanged)
    {
        internalContentChanged = false;

        QDomDocument dom("rss");
        if (!dom.setContent(content))
            return;

        // Get Rid of old items.
        for (int i=0;i<feedItems.count();++i)
            delete feedItems.at(i);
        feedItems.clear();

        QDomElement de = dom.documentElement();

        QDomElement channel = de.firstChildElement("channel");

        QDomNodeList nodeList = channel.childNodes();
        for (int i=0;i<nodeList.count();++i)
        {
            QDomElement e = nodeList.at(i).toElement();

            if (e.tagName() == "title")
                title = e.text();
            else if (e.tagName() == "link")
                link = e.text();
            else if (e.tagName() == "description")
                description = e.text();
            else if (e.tagName() == "item")
            {
                TPFeedItem *item = new TPFeedItem;
                QDomNodeList itemNodes = e.childNodes();
                for (int j=0;j<itemNodes.count();++j)
                {
                    QDomElement e2 = itemNodes.at(j).toElement();
                    if (e2.tagName() == "title")
                        item->setTitle(e2.text());
                    else if (e2.tagName() == "link")
                        item->setLink(e2.text());
                    else if (e2.tagName() == "description")
                        item->setDescription(e2.text());
                    else if (e2.tagName() == "enclosure")
                    {
                        if (e2.hasAttribute("url"))
                            item->setEnclosure(e2.attribute("url"));
                        if (e2.hasAttribute("size"))
                            item->setSize(e2.attribute("size").toInt());
                    }
                }

                if (item->getEnclosure().length())
                    feedItems.append(item);
                else
                    // Only accepts items having enclosure
                    delete item;
            }
        }
    }
}
