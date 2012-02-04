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

#ifndef TPSORTINTERFACE_H
#define TPSORTINTERFACE_H

#include <QString>

// FORWARD DECLARATIONS
class TPSortableAssociativeObject;

//! @class TPSortInterface
//! @brief Abstract class defining interface used to
//! perform comparison operations to associative objects.
class TPSortInterface
{
public:

    TPSortInterface(const QString _key) : key(_key), next(0)
    {
    }

    virtual ~TPSortInterface()
    {
        if (next)
            delete next;
    }

    virtual bool lessThan(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const = 0;
    virtual bool equals(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const = 0;

    inline void addToChain(TPSortInterface *_next)
    {
        if (next)
            next->addToChain(_next);
        else
            next = _next;
    }

    inline bool tryNextLessThanFalse(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const
    {
        return next ? next->lessThan(o1, o2) : false;
    }

    inline bool tryNextEqualsFalse(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const
    {
        return next ? next->equals(o1, o2) : false;
    }

    inline bool tryNextLessThanTrue(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const
    {
        return next ? next->lessThan(o1, o2) : true;
    }

    inline bool tryNextEqualsTrue(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const
    {
        return next ? next->equals(o1, o2) : true;
    }

protected:

    QString key;
    TPSortInterface *next;
};

#endif // TPSORTINTERFACE_H
