/*++

Module Name:

    zwapi.h

Abstract:

    Compatibility entry point for private RTL code that includes the WRK
    Zw API declarations. Keep this header intentionally dependency-light;
    the host native SDK provides the common declarations when available.

--*/

#ifndef _ZWAPI_COMPAT_
#define _ZWAPI_COMPAT_

#include "nt.h"

#if defined(_MSC_VER)
#include <winternl.h>
#endif

#endif /* _ZWAPI_COMPAT_ */
