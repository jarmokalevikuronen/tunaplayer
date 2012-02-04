#include "tpsort.h"
#include "tpsortimplementationnumbase.h"
#include "tpassociative.h"

TPSortImplementationNumBase::TPSortImplementationNumBase(const QString _key) : TPSortInterface(_key)
{
}

int TPSortImplementationNumBase::getNum(const TPSortableAssociativeObject &object) const
{
    int value;

    if (object.hasCachedIntFor(this))
    {
        value = object.getCachedInt();
    }
    else
    {
        value = object.object().getInt(key);
        const_cast<TPSortableAssociativeObject &>(object).setCachedInt(this, value);
    }

    return value;
}
