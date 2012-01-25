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

#ifndef TPIDBASEMAP_H
#define TPIDBASEMAP_H

#include <QMap>
#include <QVector>
#include <QByteArray>
#include "tplog.h"

template <class Type>
class TPIdBasedContainer
{
private:

    QMap<QByteArray, Type> objectMap;
    QVector<Type> objectArray;
    typedef typename QMap<QByteArray, Type>::const_iterator citer;
    typedef typename QMap<QByteArray, Type>::iterator iter;

public:

    inline int count() const
    {
        return objectArray.count();
    }

    bool insertObject(Type object)
    {
        Type existing = getObject(object->identifier(false));
        if (existing)
        {
            return false;
        }

        objectMap.insert(object->identifier(false), object);
        objectArray.append(object);

        return true;
    }

    Type takeLast()
    {
        if (objectArray.count() <= 0)
            return NULL;

        Type object = objectArray.at(objectArray.count()-1);
        objectArray.remove(objectArray.count()-1);
        objectMap.remove(object->identifier(false));
        return object;
    }

    Type at(int index) const
    {
        return objectArray.at(index);
    }

    void removeObject(const QByteArray id)
    {
        Type ptr = getObject(id);
        if (!ptr)
            return;

        int arrayIdx = objectArray.indexOf(ptr);
        if (arrayIdx < 0)
            return;

        objectArray.remove(arrayIdx);
        objectMap.remove(id);
    }

    void removeObject(Type object)
    {
        removeObject(object->identifier(false));
    }

    Type getObject(const QByteArray id) const
    {
        citer it = objectMap.constFind(id);
        if (it == objectMap.constEnd())
            return NULL;

        return it.value();
    }
};
#endif // TPIDBASEMAP_H
