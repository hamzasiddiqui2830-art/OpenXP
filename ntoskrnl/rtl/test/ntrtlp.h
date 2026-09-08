/*++

Module Name:

    ntrtlp.h

Abstract:

    Private RTL test compatibility header. The original WRK tests include
    ntrtlp.h, but the converted source tree did not contain the header.
    Keep the test-private include local so normal kernel headers are not
    polluted with test-only declarations.

--*/

#ifndef _OPENXP_NTRTLP_TEST_
#define _OPENXP_NTRTLP_TEST_

#include "ntos.h"
#include <ntrtl.h>

#ifndef NUMBER_OF
#define NUMBER_OF(_Array) (sizeof(_Array) / sizeof((_Array)[0]))
#endif

#ifndef MAX_USTRING
#define MAX_USTRING (sizeof(WCHAR) * (MAXUSHORT / sizeof(WCHAR)))
#endif

#endif /* _OPENXP_NTRTLP_TEST_ */
