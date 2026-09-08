/*++

Module Name:

    nt.h

Abstract:

    Compatibility entry point for the converted WRK tree. Prefer the real
    WRK public SDK header when it is present; otherwise use the host Windows
    native API declarations supplied by the Windows SDK.

--*/

#ifndef NT_INCLUDED
#define NT_INCLUDED

/*
 * The Windows SDK's minwindef.h uses SAL annotations such as
 * _Null_terminated_.  winternl.h normally reaches sal.h through the normal
 * SDK include chain, but this compatibility header is included directly by
 * WRK sources and must establish that contract itself.  Without sal.h MSVC
 * parses the annotation as an identifier and reports C2054/C2085 in
 * minwindef.h.
 */
#if defined(_MSC_VER)
#include <sal.h>
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
