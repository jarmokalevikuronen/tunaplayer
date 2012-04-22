#ifndef TPOBJECTDELEGATE_H
#define TPOBJECTDELEGATE_H

#include "tpassociativeobject.h"
#include "tpreferencecounted.h"

//! @class TPObjectDelegate
//! @brief Implements a simple associative object delegate
//! User when for example youtube object is played as the player
//! only "understands" TPTrack objects.
class TPObjectDelegate
{
public:

    TPObjectDelegate(TPAssociativeObject *_ao = 0, TPReferenceCounted *_rc = 0);

    TPAssociativeObject *ao;
    TPReferenceCounted *rc;
};

#endif // TPOBJECTDELEGATE_H
