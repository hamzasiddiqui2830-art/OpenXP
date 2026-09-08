/*++

Module Name:

    nt.h

Abstract:

    Compatibility entry point for the converted WRK tree. Prefer the real
    WRK public SDK header when it is present; otherwise use the host Windows
    native API declarations supplied by winternl.h.

--*/

#ifndef NT_INCLUDED
#define NT_INCLUDED

#if defined(_MSC_VER)
#include <winternl.h>
#else
#include <stdint.h>
#include <stddef.h>
#if defined(_WIN32) && defined(__has_include)
# if __has_include(<winternl.h>)
#  include <winternl.h>
# endif
#endif
#endif

#endif /* NT_INCLUDED */
