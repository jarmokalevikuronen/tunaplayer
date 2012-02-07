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

#include "tplastfmimagesearch.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkInterface>
#include "tplog.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QPixmap>

TPLastFMImageSearch::TPLastFMImageSearch(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    maxImageCount = 1000;
}

void TPLastFMImageSearch::setMaxImageCount(int count)
{
    maxImageCount = count;
}

bool TPLastFMImageSearch::startSearch(const QString artist, const QString album)
{
    QUrl url("http://ws.audioscrobbler.com/2.0/");

    url.addQueryItem("method", "album.getinfo");
    // API key from LastFM demo application.
    url.addQueryItem("api_key", "b25b959554ed76058ac220b7b2e0a026"); // from last.fm demo app
    url.addQueryItem("artist", artist);
    url.addQueryItem("album", album);

    QObject::disconnect(manager,0,0,0);
    QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(searchFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(url));

    return true;
}

void TPLastFMImageSearch::searchFinished(QNetworkReply *reply)
{
    QObject::disconnect(manager,0,0,0);

    /*
<?xml version="1.0" encoding="utf-8"?>
<lfm status="ok">
<album>
    <name>Greatest Lovesongs Vol 666</name>
    <artist>HIM</artist>
    <id>3969624</id>
    <mbid></mbid>
    <url>http://www.last.fm/music/HIM/Greatest+Lovesongs+Vol+666</url>
    <releasedate>    </releasedate>
    <image size="small"></image>
    <image size="medium"></image>
    <image size="large"></image>
    <image size="extralarge"></image>
    <image size="mega"></image>
        <listeners>1908</listeners>
    <playcount>32422</playcount>
        <tracks>
            </tracks>
    <toptags>
      </toptags>
   </album></lfm>
    */

    QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!status.isValid())
    {
        DEBUG() << "LASTFM: No connectivity. Bailing out.";

        emit connectivityLost(this);
        return;
    }

    QByteArray response = reply->readAll();
    QDomDocument dom("lastfm");
    if (!dom.setContent(response))
    {
        emit complete(this);
        return;
    }

    QDomElement de = dom.documentElement();
    QDomElement album = de.firstChildElement("album");
    QDomNodeList nodeList = album.childNodes();

    imageUrls.clear();

    for (int i=0;i<nodeList.count();++i)
    {
        QDomElement e = nodeList.at(i).toElement();

        if (e.tagName() == "image")
        {
            if (e.hasAttribute("size"))
            {
                QString size = e.attribute("size");
                if (size == "extralarge" || size == "mega" || size == "large")
                    if (e.text().length() && imageUrls.count() < maxImageCount)
                        imageUrls.append(e.text());
            }
        }
    }

    emit expectedImageCount(this, qMin(imageUrls.count(), maxImageCount));

    if (imageUrls.count() <= 0)
    {
        DEBUG() << "LASTFM: Art Search -> No matches";
        emit complete(this);
    }
    else
    {
        QObject::disconnect(manager,0,0,0);
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processImageDownloaded(QNetworkReply *)));

        // Start download each and every image at once.
        for (int i=0;i<imageUrls.count() && i < maxImageCount;++i)
            manager->get(QNetworkRequest(QUrl(imageUrls.at(i))));
    }
}

void TPLastFMImageSearch::processImageDownloaded(QNetworkReply *reply)
{
    imageUrls.removeOne(reply->request().url().toString());

    QImage image;
    if (image.loadFromData(reply->readAll()))
    {
        DEBUG() << "LASTFM: image downloaded from: " << reply->request().url().toString();
        emit imageDownloaded(this, image);
    }
    else
    {
        DEBUG() << "LASTFM: image download failed from: " << reply->request().url().toString();
    }

    if (imageUrls.count() <= 0)
    {
        QObject::disconnect(manager,0,0,0);
        emit complete(this);
    }
}
