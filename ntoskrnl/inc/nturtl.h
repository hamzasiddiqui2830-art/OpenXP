/*++

Module Name:

    nturtl.h

Abstract:

    Compatibility entry point for user-mode RTL declarations used by WRK
    tests. The native Windows SDK supplies the common declarations through
    winternl.h when building on Windows.

--*/

#ifndef _NTURTL_COMPAT_
#define _NTURTL_COMPAT_

#include "nt.h"

#if defined(_MSC_VER)
#include <winternl.h>
#endif

#endif /* _NTURTL_COMPAT_ */
