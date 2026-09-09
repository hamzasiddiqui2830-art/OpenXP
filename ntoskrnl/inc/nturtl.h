/*++

Module Name:

    nturtl.h

Abstract:

    Compatibility entry point for the WRK user-mode RTL declarations.
    OpenXP supplies its own NT type universe; kernel builds must not pull
    declarations from the host Windows SDK.

--*/

#ifndef _NTURTL_COMPAT_
#define _NTURTL_COMPAT_

#include "nt.h"

#endif /* _NTURTL_COMPAT_ */
