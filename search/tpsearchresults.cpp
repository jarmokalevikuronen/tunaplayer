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

#include "tpsearchresults.h"

static bool lessThan(const TPSortableAssociativeObject *object1, const TPSortableAssociativeObject *object2)
{
    return object1->sorter()->lessThan(*object1, *object2);
}

TPSearchResults::TPSearchResults(TPSortInterface *_sorter, int estimatedCount) :
    sorter(_sorter)
{
    estimatedCount = qMax(0, estimatedCount);
    results.reserve(estimatedCount + 1);
}

void TPSearchResults::add(TPAssociativeObject *object)
{
    results.append(new TPSortableAssociativeObject(object, sorter));
}

void TPSearchResults::sort()
{
    if (sorter)
        qStableSort(results.begin(), results.end(), lessThan);
}
