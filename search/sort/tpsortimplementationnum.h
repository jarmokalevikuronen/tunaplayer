#ifndef TPSORTIMPLEMENTATIONNUM_H
#define TPSORTIMPLEMENTATIONNUM_H

#include "tpsortimplementationnumbase.h"

class TPSortImplementationNum : public TPSortImplementationNumBase
{
public:

    TPSortImplementationNum(bool _ascending, const QString _key);

private:

    bool lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;
    bool equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;

private:

    bool ascending;
};

#endif // TPSORTIMPLEMENTATIONNUM_H
