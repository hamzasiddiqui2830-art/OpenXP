/*++

Module Name:

    nt.h

Abstract:

    Compatibility entry point for the converted WRK tree. The kernel uses
    the WRK public NT definitions directly and prevents user-mode Windows
    SDK headers from replacing them during kernel compilation.

--*/

#ifndef NTOSKRNL_NT_H_INCLUDED
#define NTOSKRNL_NT_H_INCLUDED

#if defined(_MSC_VER)
#include <sal.h>
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

/*
 * ntrtl.h is the WRK kernel RTL header and expects the WRK ntdef.h type
 * universe. Do not include winternl.h here: the host SDK's winternl.h
 * redeclares several structures that ntrtl.h intentionally defines itself,
 * including RTL_USER_PROCESS_PARAMETERS, and does not provide every WRK
 * declaration such as RTL_ATOM in the form expected by this tree.
 *
 * NT_INCLUDED tells the Windows SDK that the NT base header has already been
 * established. This keeps later SDK minwindef/winnt include paths from
 * replacing the WRK definitions with a second, incompatible type universe.
 */
#include <ntdef.h>

#ifndef NT_INCLUDED
#define NT_INCLUDED
#endif

/* WRK ntrtl.h consumes these declarations before ntrtl.h is parsed. */
#ifndef RTL_ATOM
typedef USHORT RTL_ATOM, *PRTL_ATOM;
#endif

#ifndef FASTCALL
#define FASTCALL __fastcall
#endif
#ifndef CLONG
typedef LONG CLONG;
#endif
#else
#include <stdint.h>
#include <stddef.h>
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