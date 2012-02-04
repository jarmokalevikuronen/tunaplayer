#ifndef TPSORTIMPLEMENTATIONSTRING_H
#define TPSORTIMPLEMENTATIONSTRING_H

#include <QtCore>
#include "tpsortimplementationstringbase.h"

class TPSortImplementationString : public TPSortImplementationStringBase
{
public:

    TPSortImplementationString(Qt::CaseSensitivity _cs, bool _ascending, const QString _key);

private:

    bool lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;
    bool equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;

private:

    bool ascending;
    Qt::CaseSensitivity cs;
};

#endif // TPSORTIMPLEMENTATIONSTRING_H
