#include "tpsortimplementationstringbase.h"
#include "tpsort.h"
#include "tpassociative.h"

TPSortImplementationStringBase::TPSortImplementationStringBase(const QString _key) : TPSortInterface(_key)
{
}

const QString TPSortImplementationStringBase::getString(const TPSortableAssociativeObject &o) const
{
    if (o.hasCachedStringFor(this))
        return o.getCachedString();

    const QString value = o.object().getString(key);
    const_cast<TPSortableAssociativeObject &>(o).setCachedString(this, value);

    return value;
}
