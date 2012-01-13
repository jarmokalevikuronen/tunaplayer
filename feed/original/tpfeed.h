#ifndef TPFEED_H
#define TPFEED_H

#include <QString>
#include <QUrl>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QVector>

class TPFeed;

QDebug operator<<(QDebug dbg, const TPFeed &feed);

class TPFeedItem
{

public:

    explicit TPFeedItem()
    {
        size = 0;
    }


    inline void setTitle(const QString _title)
    {
        title = _title;
    }

    inline void setLink(const QString _link)
    {
        link = _link;
    }

    inline void setDescription(const QString _description)
    {
        description = _description;
    }

    inline void setEnclosure(const QString _enclosure)
    {
        enclosure = _enclosure;
    }

    inline void setSize(int _size)
    {
        size = _size;
    }

    inline const QString getTitle() const
    {
        return title;
    }

    inline const QString getLink() const
    {
        return link;
    }

    inline const QString getDescription() const
    {
        return description;
    }

    inline const QString getEnclosure() const
    {
        return enclosure;
    }

    inline int getSize() const
    {
        return size;
    }

private:

    int size;
    QString enclosure;
    QString title;
    QString link;
    QString description;
};

class TPFeed
{
public:

    TPFeed(const QString _url);

    inline const QUrl getUrl() const
    {
        return url;
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
        return title;
    }

    inline const QString getDescription() const
    {
        return description;
    }

    inline const QString getLink() const
    {
        return link;
    }

    inline int count() const
    {
        return feedItems.count();
    }

    TPFeedItem* at(int index) const
    {
        return feedItems.at(index);
    }

private: // Data

    QString lastError;
    QUrl url;
    QByteArray content;
    bool internalContentChanged;
    QVector< TPFeedItem *> feedItems;
    QString title;
    QString link;
    QString description;
};

#endif // TPFEED_H
