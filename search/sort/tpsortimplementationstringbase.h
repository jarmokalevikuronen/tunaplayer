#ifndef TPSORTIMPLEMENTATIONSTRINGBASE_H
#define TPSORTIMPLEMENTATIONSTRINGBASE_H

#include "tpsortinterface.h"

class TPSortImplementationStringBase : public TPSortInterface
{
protected:

    TPSortImplementationStringBase(const QString _key);
    const QString getString(const TPSortableAssociativeObject &o) const;
};

#endif // TPSORTIMPLEMENTATIONSTRINGBASE_H
