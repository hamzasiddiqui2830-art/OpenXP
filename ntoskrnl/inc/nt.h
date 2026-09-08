/*++

Module Name:

    nt.h

Abstract:

    Compatibility entry point for the converted WRK tree. Prefer the real
    WRK public SDK header when it is present; otherwise use the host Windows
    native API declarations supplied by the Windows SDK.

--*/

#ifndef NTOSKRNL_NT_H_INCLUDED
#define NTOSKRNL_NT_H_INCLUDED

/*
 * Do not define NT_INCLUDED here. The Windows SDK's minwindef.h uses
 * NT_INCLUDED to decide whether it must include winnt.h. Defining it before
 * including winternl.h prevents winnt.h from being pulled in, which leaves
 * fundamental types such as UINT_PTR undefined and causes the later WPARAM /
 * LPARAM typedefs in minwindef.h to fail.
 *
 * The compatibility header therefore has its own guard and lets the Windows
 * SDK own NT_INCLUDED while processing winternl.h.
 */
#if defined(_MSC_VER)
#include <sal.h>

/*
 * winternl.h uses these legacy SpecStrings entry-point annotations directly.
 * They must be defined before winternl.h is parsed because nt.h is often the
 * first compatibility header included by converted WRK sources.
 */
#ifndef __kernel_entry
#define __kernel_entry
#endif
#ifndef __rpc_entry
#define __rpc_entry
#endif
#ifndef __gdi_entry
#define __gdi_entry
#endif
#ifndef __control_entrypoint
#define __control_entrypoint(category)
#endif

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

#endif /* NTOSKRNL_NT_H_INCLUDED */
