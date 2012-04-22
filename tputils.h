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

#ifndef TPUTILS_H
#define TPUTILS_H

#include <QString>

//! @class TPUtils
//! @brief Generic utility functions of time, url, filename handling.
class TPUtils
{
public:

    static bool playlistFile(const QString filename);
    static bool webAddress(const QString line);
    static int currentEpoch();
    static int age(int bornEpoch);
    static QString ensureFilenameCharacters(const QString source);
};

//!
//! @brief Calls a dec method for all objects within the given container.
//!
#define TPDecForAll(container)\
{\
    for (int i=0;i<container.count();++i){\
        if (container.at(i))\
            container.at(i)->dec();}\
    container.clear();\
}

#endif // TPUTILS_H
