#include "tpobjectdelegate.h"

TPObjectDelegate::TPObjectDelegate(TPAssociativeObject *_ao, TPReferenceCounted *_rc)
{
    rc = _rc;
    ao = _ao;
}

