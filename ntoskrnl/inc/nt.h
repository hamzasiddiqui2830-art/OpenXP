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
 * redeclares structures that ntrtl.h intentionally defines itself.
 */
#include <excpt.h>
#include <stdarg.h>
#include <ntdef.h>

#ifndef NT_INCLUDED
#define NT_INCLUDED
#endif

/* Keep the same public-header dependency order as the WRK nt.h. */
#include <ntstatus.h>
#include <ntkeapi.h>

#if defined(_AMD64_)
#include <ntamd64.h>
#elif defined(_X86_)
#include <nti386.h>
#elif defined(_ARM_)
#include <ntarm.h>
#elif defined(_ARM64_)
#include <ntarm64.h>
#endif

#include <ntseapi.h>
#include <ntobapi.h>
#include <ntimage.h>
#include <ntldr.h>
#include <ntpsapi.h>
#include <ntxcapi.h>
#include <ntlpcapi.h>
#include <ntioapi.h>
#include <ntiolog.h>
#include <ntpoapi.h>
#include <ntexapi.h>
#include <ntmmapi.h>
#include <ntregapi.h>
#include <ntelfapi.h>
#include <ntconfig.h>
#include <ntnls.h>
#include <ntpnpapi.h>
#include <mce.h>

/* WRK ntrtl.h consumes this declaration before its atom APIs are parsed. */
typedef USHORT RTL_ATOM;
typedef RTL_ATOM *PRTL_ATOM;

#ifndef FASTCALL
#define FASTCALL __fastcall
#endif

#else
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#endif

/*
 * Some host Windows SDK headers expose AVL RTL routines as macros. ntrtl.h
 * provides the WRK declarations for these routines, so prevent host SDK
 * macros from rewriting the function names while ntrtl.h is parsed.
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