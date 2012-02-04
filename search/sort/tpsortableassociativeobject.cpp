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

#include "tpsort.h"
#include "tpsortableassociativeobject.h"

TPSortableAssociativeObject::TPSortableAssociativeObject(TPAssociativeObject *__object, TPSortInterface *__sorter)
     : _object(__object), _sorter(__sorter), cachedInt(0), cachedIntCookie(0), cachedStringCookie(0)
{
}

TPSortableAssociativeObject::TPSortableAssociativeObject(const TPSortableAssociativeObject &other)
{
    _object = other._object;
    _sorter = other._sorter;
    cachedInt = other.cachedInt;
    cachedIntCookie = other.cachedIntCookie;
    cachedString = other.cachedString;
    cachedStringCookie = other.cachedStringCookie;
}

bool TPSortableAssociativeObject::operator<(const TPSortableAssociativeObject *_other) const
{
    return _sorter->lessThan(*this, *_other);
}

bool TPSortableAssociativeObject::operator<(const TPSortableAssociativeObject &_other) const
{
    return _sorter->lessThan(*this, _other);
}

bool TPSortableAssociativeObject::operator==(const TPSortableAssociativeObject *_other) const
{
    return _sorter->equals(*this, *_other);
}

bool TPSortableAssociativeObject::operator==(const TPSortableAssociativeObject &_other) const
{
    return _sorter->equals(*this, _other);
}

void TPSortableAssociativeObject::setCachedInt(const TPSortInterface *caller, int value) const
{
    cachedIntCookie = caller;
    cachedInt = value;
}

bool TPSortableAssociativeObject::hasCachedIntFor(const TPSortInterface *caller) const
{
    return cachedIntCookie == caller;
}

int TPSortableAssociativeObject::getCachedInt() const
{
    return cachedInt;
}

void TPSortableAssociativeObject::setCachedString(const TPSortInterface *caller, const QString string) const
{
    cachedStringCookie = caller;
    cachedString = string;
}

bool TPSortableAssociativeObject::hasCachedStringFor(const TPSortInterface *caller) const
{
    return cachedStringCookie == caller;
}

QString TPSortableAssociativeObject::getCachedString() const
{
    return cachedString;
}

const TPAssociativeObject& TPSortableAssociativeObject::object() const
{
    return *_object;
}

TPSortInterface* TPSortableAssociativeObject::sorter() const
{
    return _sorter;
}

