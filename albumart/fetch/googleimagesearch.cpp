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

#include "googleimagesearch.h"
#include "json.h"
#include <QApplication>
#include <QImage>
#include "tplog.h"

using namespace QtJson;

#define PROGRESS_BASE   1
#define PROGRESS_SEARCH 9
#define PROGRESS_READY  100

TPGoogleImageSearch::TPGoogleImageSearch(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);    
    expectedResponses = 0;
}

bool TPGoogleImageSearch::startSearch(const QString artist, const QString album)
{
    if (expectedResponses)
        return false;

    QString query = artist + " " + album;

    QObject::disconnect(manager,0,0,0);
    QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(searchFinished(QNetworkReply*)));

    QUrl url("http://ajax.googleapis.com/ajax/services/search/images");
    url.addQueryItem("q", query);
    url.addQueryItem("v", "1.0");
    //url.addQueryItem("rsz?", QString::number(maxResults));

    manager->get(QNetworkRequest(url));

    return true;
}

void TPGoogleImageSearch::searchFinished(QNetworkReply *reply)
{
    QObject::disconnect(manager,0,0,0);

    bool success = false;
    QVariant result = Json::parse(reply->readAll(), success);

    Q_UNUSED(success);


    QVariantMap responseDataMap = result.toMap()["responseData"].toMap();
    QList<QVariant> resultList = responseDataMap["results"].toList();

    expectedResponses = resultList.count();
    DEBUG() << QString("ALBUMART: GOOGLE: %1 potential album arts to download").arg(expectedResponses);
    if (expectedResponses > 0)
    {      
        QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(downloadFinished(QNetworkReply*)));

        emit expectedImageCount(this, expectedResponses);

        foreach(QVariant v,resultList)
            manager->get(QNetworkRequest(QUrl( v.toMap()["url"].toString())));
    }
    else
    {
        QObject::disconnect(manager,0,0,0);
        emit complete(this);
    }
}


void TPGoogleImageSearch::downloadFinished(QNetworkReply *reply)
{    
    if (reply->error())
        ERROR() << "ALBUMART: GOOGLE: downloadFinished: " << reply->error();
    else
    {
        QImage image;
        if (image.loadFromData(reply->readAll()))
        {
            DEBUG() << "ALBUMART: GOOGLE: image downloaded from: " << reply->request().url().toString();
            emit imageDownloaded(this, image);
        }
        else
            DEBUG() << "ALBUMART: GOOGLE: Failed to load image from: " << reply->request().url().toString();
    }

    if (--expectedResponses <= 0)
    {
        DEBUG() << "ALBUMART: GOOGLE: Complete";

        QObject::disconnect(manager,0,0,0);
        emit complete(this);
    }
}
