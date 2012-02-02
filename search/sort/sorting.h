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

#ifndef TP_SORT_H__
#define TP_SORT_H__

#include <QString>
#include "tpassociative.h"
#include "tplog.h"

// FORWARD DECLARATIONS
class TPSortableAssociativeObject;

//! @class TPSortInterface
//! @brief Abstract class defining interface used to
//! perform comparison operations to associative objects.
class TPSortInterface
{
public:

    virtual bool lessThan(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const = 0;
    virtual bool equals(const TPSortableAssociativeObject &o1, const TPSortableAssociativeObject &o2) const = 0;

    inline void addToChain(TPSortInterface *_next)
    {
        if (next)
            next->addToChain(_next);
        else
            next = _next;
    }

    TPSortInterface(const QString _key)
    {
        key = _key;
        next = NULL;
    }

    virtual ~TPSortInterface()
    {
        if (next)
            delete next;
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


class TPSortableAssociativeObject
{
public:

    TPSortableAssociativeObject(TPAssociativeObject *__object, TPSortInterface *__sorter)
     : _object(__object), _sorter(__sorter)
    {
        cachedIntCookie = cachedStringCookie = NULL;
    }

    TPSortableAssociativeObject(const TPSortableAssociativeObject &other)
    {
        this->_object = other._object;
        this->_sorter = other._sorter;
        this->cachedInt = other.cachedInt;
        this->cachedIntCookie = other.cachedIntCookie;
        this->cachedString = other.cachedString;
        this->cachedStringCookie = other.cachedStringCookie;
    }


    bool operator<(const TPSortableAssociativeObject *_other) const
    {
        return _sorter->lessThan(*this, *_other);
    }

    bool operator<(const TPSortableAssociativeObject &_other) const
    {
        return _sorter->lessThan(*this, _other);
    }

    bool operator==(const TPSortableAssociativeObject *_other) const
    {
        return _sorter->equals(*this, *_other);
    }

    bool operator==(const TPSortableAssociativeObject &_other) const
    {
        return _sorter->equals(*this, _other);
    }

    inline void setCachedInt(const TPSortInterface *caller, int value) const
    {
        cachedIntCookie = caller;
        cachedInt = value;
    }

    inline bool hasCachedIntFor(const TPSortInterface *caller) const
    {
        return cachedIntCookie == caller;
    }

    inline int getCachedInt() const
    {
        return cachedInt;
    }

    inline void setCachedString(const TPSortInterface *caller, const QString string) const
    {
        cachedStringCookie = caller;
        cachedString = string;
    }

    inline bool hasCachedStringFor(const TPSortInterface *caller) const
    {
        return cachedStringCookie == caller;
    }

    inline QString getCachedString() const
    {
        return cachedString;
    }

    inline const TPAssociativeObject& object() const
    {
        return *_object;
    }

    inline TPSortInterface* sorter() const
    {
        return _sorter;
    }

private:

    mutable int cachedInt;
    mutable const TPSortInterface *cachedIntCookie;

    mutable QString cachedString;
    mutable const TPSortInterface *cachedStringCookie;

    TPAssociativeObject *_object;
    TPSortInterface *_sorter;
};


//! @class TPSortImplementationNumBase
//! @brief Base class for number sorting that implements a common base.
class TPSortImplementationNumBase : public TPSortInterface
{
protected:

    TPSortImplementationNumBase(const QString _key) : TPSortInterface(_key)
    {
    }

    inline int getNum(const TPSortableAssociativeObject &object) const
    {
        int value;

        if (object.hasCachedIntFor(this))
        {
            value = object.getCachedInt();
//            DEBUG() << "CACHED INT: " << value;
        }
        else
        {
            value = object.object().getInt(key);
//            DEBUG() << "CACHING INT: " << value << " KEY=" << key;
            const_cast<TPSortableAssociativeObject &>(object).setCachedInt(this, value);
        }

        return value;
    }
};


class TPSortImplementationStringBase : public TPSortInterface
{
protected:

    TPSortImplementationStringBase(const QString _key) : TPSortInterface(_key)
    {

    }

    inline const QString getString(const TPSortableAssociativeObject &o) const
    {
        if (o.hasCachedStringFor(this))
            return o.getCachedString();

        const QString value = o.object().getString(key);
        const_cast<TPSortableAssociativeObject &>(o).setCachedString(this, value);
        return value;
    }
};

class TPSortImplementationString : public TPSortImplementationStringBase
{
private:

    bool lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
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

    bool equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
    {
        if (getString(object1).compare(getString(object2), cs) == 0)
        {
            return tryNextEqualsTrue(object1, object2);
        }
        return false;
    }

public:

    TPSortImplementationString(Qt::CaseSensitivity _cs, bool _ascending, const QString _key) : TPSortImplementationStringBase(_key)
    {
        cs = _cs;
        ascending = _ascending;
    }

private:

    bool ascending;
    Qt::CaseSensitivity cs;
};

class TPSortImplementationNum : public TPSortImplementationNumBase
{
private:

    bool lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
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

    bool equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
    {
        if (getNum(object1) == getNum(object2))
            return tryNextEqualsTrue(object1, object2);
        return false;
    }

public:

    TPSortImplementationNum(bool _ascending, const QString _key) : TPSortImplementationNumBase(_key)
    {
        ascending = _ascending;
    }

private:

    bool ascending;
};


class TPSortInterfaceFactory
{
public:

    //! @brief Creates a sort interface instantiatation that is capable of doing
    //! the comparison of two objects
    //! @param _sortOrder, how the sorting is to be applied (ascending, descending, ascending-cs, ascending-num, ..)
    //! @param _sortKey, key within associative map that is used as sort criteria..
    //! @return Newly created sorter instance, or NULL in case invalid _sortOrder parameter that can not be understood.
    static TPSortInterface* create(const QString _sortOrder, const QString _sortKey);
};

#endif
