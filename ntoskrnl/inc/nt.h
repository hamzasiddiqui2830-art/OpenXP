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
 * ntrtl.h is included immediately by ntos.h. Establish the WRK public NT
 * types it consumes before entering the host Windows SDK include chain.
 * In particular, ntrtl.h uses RTL_ATOM/PRTL_ATOM, CSHORT and
 * PNT_PRODUCT_TYPE, which are supplied by these SDK headers.
 */
#include <ntdef.h>
#include <ntexapi.h>

/*
 * Load the NTOSKRNL SpecStrings compatibility layer explicitly before
 * winternl.h. This prevents the Windows SDK from selecting a different
 * SpecStrings definition later in the include chain and guarantees that
 * the legacy entry-point annotations are available while winternl.h is parsed.
 */
#include "specstrings.h"

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

/*
 * Legacy WRK declarations in ntrtl.h use FASTCALL and CLONG. Modern Windows
 * SDK headers do not provide these WRK spellings consistently, so provide
 * the compatibility definitions here after the SDK has supplied the base
 * Windows types.
 */
#ifndef FASTCALL
#define FASTCALL __fastcall
#endif
#ifndef CLONG
typedef LONG CLONG;
#endif
#else
#include <stdint.h>
#include <stddef.h>
#if defined(_WIN32) && defined(__has_include)
# if __has_include(<winternl.h>)
#  include <winternl.h>
# endif
#endif
#endif
/*
 * Some host Windows SDK headers expose AVL RTL routines as macros.
 * ntrtl.h provides the WRK declarations for these routines, so prevent
 * host SDK macros from rewriting the function names while ntrtl.h is parsed.
 */
#ifdef RtlInitializeGenericTableAvl
#undef RtlInitializeGenericTableAvl
#endif

#ifdef RtlInsertElementGenericTableAvl
#undef RtlInsertElementGenericTableAvl
#endif

#ifdef RtlInsertElementGenericTableFullAvl
#undef RtlInsertElementGenericTableFullAvl
#endif

#ifdef RtlDeleteElementGenericTableAvl
#undef RtlDeleteElementGenericTableAvl
#endif

#ifdef RtlLookupElementGenericTableAvl
#undef RtlLookupElementGenericTableAvl
#endif

#ifdef RtlLookupElementGenericTableFullAvl
#undef RtlLookupElementGenericTableFullAvl
#endif

#ifdef RtlEnumerateGenericTableAvl
#undef RtlEnumerateGenericTableAvl
#endif

#ifdef RtlEnumerateGenericTableWithoutSplayingAvl
#undef RtlEnumerateGenericTableWithoutSplayingAvl
#endif

#ifdef RtlEnumerateGenericTableLikeADirectory
#undef RtlEnumerateGenericTableLikeADirectory
#endif

#ifdef RtlGetElementGenericTableAvl
#undef RtlGetElementGenericTableAvl
#endif

#ifdef RtlNumberGenericTableElementsAvl
#undef RtlNumberGenericTableElementsAvl
#endif

#ifdef RtlIsGenericTableEmptyAvl
#undef RtlIsGenericTableEmptyAvl
#endif
#endif /* NTOSKRNL_NT_H_INCLUDED */
