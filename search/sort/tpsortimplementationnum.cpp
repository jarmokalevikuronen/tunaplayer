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

#include "tpsortimplementationnum.h"

TPSortImplementationNum::TPSortImplementationNum(bool _ascending, const QString _key) : TPSortImplementationNumBase(_key)
{
    ascending = _ascending;
}

bool TPSortImplementationNum::lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
{
    int int1 = getNum(object1);
    int int2 = getNum(object2);

    // If equals at this level, do dig a bit deeper to find the real order if any.
    if (int1 == int2)
        return tryNextLessThanFalse(object1, object2);
    if (ascending)
        return int1 < int2;
    return int1 > int2;
}

bool TPSortImplementationNum::equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
{
    if (getNum(object1) == getNum(object2))
        return tryNextEqualsTrue(object1, object2);
    return false;
}

