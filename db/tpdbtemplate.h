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

#ifndef TPDBTEMPLATE_H
#define TPDBTEMPLATE_H


#include "tpidbasemap.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tputils.h"
#include "tplog.h"

template <class Type>
class TPDBBase
{
protected: // Data

    TPIdBasedContainer<Type> objects;
    TPAssociativeDB* db;
    QByteArray scheme;

protected: // Implementation

    TPDBBase(QObject *parent, const QString filename, const QByteArray _scheme, TPAssociativeDBItemVisitor *visitorIf = 0)
    {
        scheme = _scheme;
        db = new TPAssociativeDB(parent, filename);
        Q_ASSERT(db);
        if (visitorIf)
            db->visitItems(visitorIf);
    }

    ~TPDBBase()
    {
        db->saveIfDirty(0);
        delete db;
    }

    void decAllObjects()
    {
        Type object = objects.takeLast();
        while (object)
        {
            object->dec();
            object = objects.takeLast();
        }
    }

public: // Implementation

    inline Type getById(const QByteArray id) const
    {
        Type object;

        if (id.startsWith(scheme))
            object = objects.getObject(id.mid(scheme.length()));
        else
            object = objects.getObject(id);

        return object;
    }

    inline Type getById(const QString id) const
    {
        return getById(id.toUtf8());
    }

    inline const QByteArray idOf(Type object) const
    {
        return object->identifier(true);
    }

    inline void save(int timeoutMs = 0)
    {
        db->saveIfDirty(timeoutMs);
    }

    inline const QString getScheme() const
    {
        return scheme;
    }

    Type at(int index) const
    {
        return objects.at(index);
    }

    inline int count() const
    {
        return objects.count();
    }

    inline void executePostCreateTasks()
    {
        int epoch = TPUtils::currentEpoch();

        for (int i=0;i<objects.count();++i)
        {
            if (!objects.at(i)->getInt(objectAttrCreated))
                objects.at(i)->setInt(objectAttrCreated, epoch);
        }
    }
};

#endif // TPDBTEMPLATE_H
