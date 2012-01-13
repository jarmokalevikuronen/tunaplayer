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

#include "tpwebsocketvirtualfolder.h"
#include <QDir>
#include <QDebug>

TPWebSocketVirtualFolder::TPWebSocketVirtualFolder(const QString _rootFolder, const QString _secret)
{
    secret = _secret;
    secretWithPathDelims = "/" + secret + "/";
    rootFolder = _rootFolder;

    if (!rootFolder.endsWith(QDir::separator()))
        rootFolder += QDir::separator();
}

const QString TPWebSocketVirtualFolder::objectToFile(const char *objectPath)
{
    if (!objectPath || !(*objectPath))
        return QString();

    QString path = objectPath;
    if (path.contains(".."))
    {
        qDebug() << "ERROR: HTTP: SERVE: dotdot found from url -> not accepted";
        return QString();
    }

    if (secret.length())
    {
        if (!path.startsWith(secretWithPathDelims))
        {
            qDebug() << "ERROR: HTTP: SERVE: No secret passed -> not serving";
            return QString();
        }
        path = path.mid(secretWithPathDelims.length());
    }

    if (path.startsWith("/"))
        return rootFolder + path.mid(1);

    return rootFolder + path;
}

