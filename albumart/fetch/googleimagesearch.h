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

#ifndef GOOGLEIMAGESEARCH_H
#define GOOGLEIMAGESEARCH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QStringList>
#include <QImage>

class TPGoogleImageSearch : public QObject
{
Q_OBJECT
public:
    explicit TPGoogleImageSearch(QObject *parent = 0);
    bool startSearch(const QString artist, const QString album);

private:

    QNetworkAccessManager *manager;
    int expectedResponses;

private slots:

    void searchFinished(QNetworkReply *reply);
    void downloadFinished(QNetworkReply *reply);

signals:

    void expectedImageCount(QObject *caller, int count);
    void imageDownloaded(QObject *caller, QImage image);
    void complete(QObject *caller);
};

#endif // GOOGLEIMAGESEARCH_H
