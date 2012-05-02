#include "tpyoutubesearch.h"
#include "tplog.h"
#include <QDomDocument>

TPYouTubeSearch::TPYouTubeSearch(QObject *parent) :
    QObject(parent), requestInProcess(0), nam(0)
{
}

TPYouTubeSearch::~TPYouTubeSearch()
{
    delete requestInProcess;
    delete nam;
    TPDecForAll(objects)
}

bool TPYouTubeSearch::search(const QString criteria)
{
    if (!nam)
    {
        nam = new QNetworkAccessManager(this);
        Q_ASSERT(nam);
        connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestComplete(QNetworkReply *)));
    }

    // Busy?
    if (requestInProcess)
        return false;

    TPDecForAll(objects);
    objects.clear();

    requestInProcess = nam->get(QNetworkRequest(buildUrl(criteria)));

    return (requestInProcess != 0);
}

void TPYouTubeSearch::requestComplete(QNetworkReply *response)
{
    Q_ASSERT(response == requestInProcess && response);

    TPDecForAll(objects);
    objects.clear();

    QByteArray xml = response->readAll();

    DEBUG() << "YOUTUBE: " << xml.length() << " bytes received";

    QDomDocument dom("feed");
    if (dom.setContent(xml))
    {
        // In YT case this is <feed..>
        QDomElement de = dom.documentElement();

        QDomNodeList nl = de.childNodes();
        for (int i=0;i<nl.count();++i)
        {
            QDomElement e = nl.at(i).toElement();

            static const QString tagNameEntry("entry");

            if (e.tagName() == tagNameEntry)
            {
                QString url;
                QString name;
                QString thumbnail;

                QDomNodeList entryElements = e.childNodes();
                for (int j=0;j<entryElements.count();j++)
                {
                    QDomElement ee = entryElements.at(j).toElement();

                    static const QString entryTagTitle("title");
                    static const QString entryTagLink("link");
                    static const QString entryTagMediaGroup("media:group");

                    if (ee.tagName() == entryTagTitle)
                    {
                        if (!name.length())
                            name = ee.text();
//                        DEBUG() << "YOUTUBE: TITLE: " << ee.text();
                    }
                    else if (ee.tagName() == entryTagLink)
                    {
                        static const QString entryLinkAttributeRel("rel");
                        static const QString entryLinkAttributeRelValueAlternate("alternate");
                        static const QString entryLinkAttributeHref("href");

                        if (ee.hasAttribute(entryLinkAttributeRel))
                        {
                            if (ee.attribute(entryLinkAttributeRel) == entryLinkAttributeRelValueAlternate)
                            {
                                if (ee.hasAttribute(entryLinkAttributeHref))
                                {
                                    if (!url.length())
                                        url = ee.attribute(entryLinkAttributeHref);
                                    //DEBUG() << "YOUTUBE: LINK: " << url;
                                }

                            }
                        }
                    }
                    else if (ee.tagName() == entryTagMediaGroup)
                    {
                        QDomNodeList mediaElements = ee.childNodes();
                        for (int i=0;i<mediaElements.count();i++)
                        {
                            QDomElement me = mediaElements.at(i).toElement();
                            static const QString mediaTagThumbnail("media:thumbnail");
                            if (me.tagName() == mediaTagThumbnail)
                            {
                                static const QString mediaTagThumbnailAttributeUrl("url");
                                if (me.hasAttribute(mediaTagThumbnailAttributeUrl))
                                {
                                    QString url(me.attribute(mediaTagThumbnailAttributeUrl));
                                    if (!thumbnail.length())
                                        thumbnail = url;
//                                    DEBUG() << "YOUTUBE: THUMBNAIL: " << url;
                                }
                            }
                        }
                    }
                }

                if (name.length() && url.length())
                {
                    TPYouTubeObject *uto = new TPYouTubeObject(url);
                    uto->setString(youtubeAttrUrl, url);
                    uto->setString(objectAttrName, name);
                    if (thumbnail.length())
                        uto->setString(youtubeAttrThumbnailUrl, thumbnail);

                    objects.append(uto);
                }
            }
        }
    }
    else
    {
        ERROR() << "YOUTUBE: Failed to parse XML";
    }

    DEBUG() << "YOUTUBE: Search result count: " << objects.count();

    requestInProcess->deleteLater();
    requestInProcess = 0;

    emit searchComplete();
}

int TPYouTubeSearch::count()
{
    return objects.count();
}

TPYouTubeObject* TPYouTubeSearch::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());
    return objects.at(index);
}

TPYouTubeObject* TPYouTubeSearch::findById(const QString &identifier)
{
    foreach(TPYouTubeObject *o, objects)
    {
        if (o->identifier(true) == identifier || o->identifier(false) == identifier)
        {
            return o;
        }
    }

    return 0;
}

TPYouTubeObject* TPYouTubeSearch::takeById(const QString &identifier)
{
    TPYouTubeObject *o = findById(identifier);
    if (!o)
        return 0;

    objects.remove(objects.indexOf(o));
    return o;
}

QUrl TPYouTubeSearch::buildUrl(const QString criteria)
{
    static QByteArray baseUrl("https://gdata.youtube.com/feeds/api/videos?q=");

    QByteArray u8 = baseUrl + criteria.toUtf8().toPercentEncoding();

    return QUrl(u8);
}

