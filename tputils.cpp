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
    QString destination = source;

    static const QString replacedCharacters(" /\\?\%*:|\"<>.");
    for (int i=0;i<replacedCharacters.count();++i)
        destination.replace(replacedCharacters.at(i), "_");

    return destination;
}

