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

