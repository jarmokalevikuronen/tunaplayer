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

#ifndef TPSORTINTERFACEFACTORY_H
#define TPSORTINTERFACEFACTORY_H

#include <QString>

// FORWARD DECLARATIONS
class TPSortInterface;

//! @class TPSortInterfaceFactory
//! @brief Sort interface factory, responsible of creating sort implementations.
class TPSortInterfaceFactory
{
public:
    static TPSortInterface* create(const QString _sortOrder, const QString _sortKey);

};

#endif // TPSORTINTERFACEFACTORY_H
