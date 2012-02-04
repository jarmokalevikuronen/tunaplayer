#ifndef TPSORTIMPLEMENTATIONNUMBASE_H
#define TPSORTIMPLEMENTATIONNUMBASE_H

#include "tpsortinterface.h"

//! @class TPSortImplementationNumBase
//! @brief Base class for number sorting that implements a common base.
class TPSortImplementationNumBase : public TPSortInterface
{
protected:

    TPSortImplementationNumBase(const QString _key);
    int getNum(const TPSortableAssociativeObject &object) const;
};

#endif // TPSORTIMPLEMENTATIONNUMBASE_H

