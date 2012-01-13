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

#include "tpgenre.h"
#include <QDebug>

// Static data initialization.
TPGenre* TPGenre::_instance = NULL;

TPGenre& TPGenre::instance()
{
    if (!_instance)
        _instance = new TPGenre;

    return *_instance;
}

void TPGenre::release()
{
    delete _instance; _instance = NULL;
}

TPGenre::TPGenre()
{
}

int TPGenre::indexOf(const QString name)
{
    QString searched = name.toLower();

    int index = genres.indexOf(searched);
    if (index >= 0)
        return index;

    genres.append(searched);

    //
    // DEBUGGING. DUMPING ALL GENRES
    //
    qDebug() << "---- GENRES ----- ";
    for (int i=0;i<genres.count();++i)
        qDebug() << "  " << genres.at(i);
    qDebug() << "------------------";

    return genres.count() - 1;
}

const QString TPGenre::nameOf(int index) const
{
    if (index < 0 || index >= genres.size())
        return "";

    return genres[index];
}
