/*++

Copyright (c) OpenXP Contributors

Module Name:

    bytefns.h

Abstract:
    
    Prototypes for instructions which operate on BYTES.


    06-Jun-1995 BarryBo, Created

Revision History:

--*/

#ifndef BYTEFNS_H
#define BYTEFNS_H

#define DISPATCHCOMMON(x) DISPATCH(x ## 8)
#include "common.h"
#undef DISPATCHCOMMON

#endif //BYTEFNS_H
