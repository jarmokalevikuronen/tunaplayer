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

#include "tpsortimplementationstring.h"

TPSortImplementationString::TPSortImplementationString(Qt::CaseSensitivity _cs, bool _ascending, const QString _key) : TPSortImplementationStringBase(_key)
{
    cs = _cs;
    ascending = _ascending;
}

bool TPSortImplementationString::lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
{
    int result = getString(object1).compare(getString(object2), cs);

    if (result == 0)
    {
        return tryNextLessThanFalse(object1, object2);
    }
    else if (result < 0)
    {
        return ascending ? true : false;
    }
    return ascending ? false : true;
}

bool TPSortImplementationString::equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
{
    if (getString(object1).compare(getString(object2), cs) == 0)
    {
        return tryNextEqualsTrue(object1, object2);
    }
    return false;
}

