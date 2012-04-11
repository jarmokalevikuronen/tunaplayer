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

#include "tputils.h"
#include <stdio.h>
#include <QDateTime>

bool TPUtils::webAddress(const QString uri)
{
    return uri.startsWith("http://", Qt::CaseInsensitive) || uri.startsWith("https://", Qt::CaseInsensitive);
}

int TPUtils::currentEpoch()
{
    return (int)(QDateTime::currentDateTime().toTime_t());
}

int TPUtils::age(int born)
{
    int now = currentEpoch();
    return born < now ? now - born : 0;
}

QString TPUtils::ensureFilenameCharacters(const QString source)
{
    QString destination;

    // NOTE = is used as escape character.
    static const QString unescapedCharacters("abcdefghijklmnopqrstuvwxyz0123456789-+_");
    for (int i=0;i<source.length();i++)
    {
        QChar ch = source.at(i);
        if (unescapedCharacters.contains(ch.toLower()))
            destination.append(ch);
        else
        {
            char escaped[16];
            sprintf(escaped, "=%04u", ch.unicode());
            destination.append(escaped);
        }
    }

    return destination;
}

bool TPUtils::playlistFile(QString filename)
{
    if (filename.startsWith("http://", Qt::CaseInsensitive) &&
                    (
                    filename.endsWith(".m3u", Qt::CaseInsensitive) ||
                    filename.endsWith(".pls", Qt::CaseInsensitive) ||
                    filename.endsWith(".asx", Qt::CaseInsensitive)
                    ))
        return true;
    return false;
}
