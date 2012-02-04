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

#include "tpsortinterfacefactory.h"
#include "tpsortinterface.h"
#include "tpsort.h"

static const QString sortOrderAscending("ascending");
static const QString sortOrderDescending("descending");
static const QString sortOrderAscendingCase("ascending-cs");
static const QString sortOrderDescendingCase("descending-cs");
static const QString sortOrderAscendingNum("ascending-num");
static const QString sortOrderDescendingNum("descending-num");


TPSortInterface* TPSortInterfaceFactory::create(const QString _sortOrder, const QString _sortKey)
{
 if (_sortOrder == sortOrderAscending)
     return new TPSortImplementationString(Qt::CaseInsensitive, true, _sortKey);
 else if (_sortOrder == sortOrderDescending)
     return new TPSortImplementationString(Qt::CaseInsensitive, false, _sortKey);
 else if (_sortOrder == sortOrderAscendingCase)
     return new TPSortImplementationString(Qt::CaseSensitive, true, _sortKey);
 else if (_sortOrder == sortOrderDescendingCase)
     return new TPSortImplementationString(Qt::CaseSensitive, false, _sortKey);
 else if (_sortOrder == sortOrderAscendingNum)
     return new TPSortImplementationNum(true, _sortKey);
 else if (_sortOrder == sortOrderDescendingNum)
     return new TPSortImplementationNum(false, _sortKey);

 return NULL;
}


