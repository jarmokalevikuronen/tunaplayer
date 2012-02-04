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

