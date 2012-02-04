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

#ifndef TPSORTABLEASSOCIATIVEOBJECT_H
#define TPSORTABLEASSOCIATIVEOBJECT_H

#include <QString>


// FORWARD DECLARATIONS
class TPAssociativeObject;
class TPSortInterface;

class TPSortableAssociativeObject
{
public:

    TPSortableAssociativeObject(TPAssociativeObject *__object, TPSortInterface *__sorter);
    TPSortableAssociativeObject(const TPSortableAssociativeObject &other);

    bool operator<(const TPSortableAssociativeObject *_other) const;
    bool operator<(const TPSortableAssociativeObject &_other) const;
    bool operator==(const TPSortableAssociativeObject *_other) const;
    bool operator==(const TPSortableAssociativeObject &_other) const;
    void setCachedInt(const TPSortInterface *caller, int value) const;
    bool hasCachedIntFor(const TPSortInterface *caller) const;;
    int getCachedInt() const;
    void setCachedString(const TPSortInterface *caller, const QString string) const;
    bool hasCachedStringFor(const TPSortInterface *caller) const;
    QString getCachedString() const;
    const TPAssociativeObject& object() const;
    TPSortInterface* sorter() const;

private:

    TPAssociativeObject *_object;
    TPSortInterface *_sorter;

    mutable int cachedInt;
    mutable const TPSortInterface *cachedIntCookie;

    mutable QString cachedString;
    mutable const TPSortInterface *cachedStringCookie;
};


#endif // TPSORTABLEASSOCIATIVEOBJECT_H
