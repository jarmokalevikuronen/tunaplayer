#include "tpsortimplementationnum.h"

TPSortImplementationNum::TPSortImplementationNum(bool _ascending, const QString _key) : TPSortImplementationNumBase(_key)
{
    ascending = _ascending;
}

bool TPSortImplementationNum::lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
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

bool TPSortImplementationNum::equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const
{
    if (getNum(object1) == getNum(object2))
        return tryNextEqualsTrue(object1, object2);
    return false;
}

