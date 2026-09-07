/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    nt.h

Abstract:

    Top level include file for applications and subsystems written to
    the NT API.  Including this file defines all the public types
    and system calls that an application program can use.

    Initial clients of this include file are the NT Shell, along with
    the Windows, OS2 and POSIX subsystems.

--*/

#ifndef NT_INCLUDED
#define NT_INCLUDED

//
// GCC compatibility macros
//
#if defined(__GNUC__)
#define __forceinline inline __attribute__((always_inline))
#define __declspec(x) __attribute__((x))
#define __fastcall __attribute__((fastcall))
#define __stdcall __attribute__((stdcall))
#define __cdecl __attribute__((cdecl))

//
// SEH (Structured Exception Handling) macros for GCC
// These are no-ops in GCC as it doesn't support MSVC-style SEH
//
#define __try
#define __except(x) if(0)
#define __finally if(0)
#define __leave break
#define _exception_code() 0
#define _exception_info() NULL
#define _abnormal_termination() FALSE

//
// Map single-underscore versions to double-underscore for GCC
//
#define try __try
#define except __except
#define finally __finally
#define AbnormalTermination() _abnormal_termination()

//
// Architecture detection for GCC
//
#if defined(__arm__) || defined(__aarch64__)
#if defined(__aarch64__)
#define _ARM64_ 1
#define _M_ARM64 4
#else
#define _ARM_ 1
#define _M_ARM 4
#endif
#endif
#endif

#if defined (_MSC_VER)
#if ( _MSC_VER >= 800 )
#pragma warning(disable:4514)
#ifndef __cplusplus
#pragma warning(disable:4116)       // TYPE_ALIGNMENT generates this - move it
                                    // outside the warning push/pop scope.
#endif
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#endif
#if (_MSC_VER > 1020)
#pragma once
#endif
#endif
//
//  Common definitions
//

#ifndef _CTYPE_DISABLE_MACROS
#define _CTYPE_DISABLE_MACROS
#endif

#include <excpt.h>
#include <stdarg.h>
#include <ntdef.h>

#include <ntstatus.h>
#include <ntkeapi.h>

#if defined(_AMD64_)
#include "ntamd64.h"

#elif defined(_X86_)
#include "nti386.h"

#elif defined(_ARM_)
#include "ntarm.h"

#elif defined(_ARM64_)
#include "ntarm64.h"

#else
#error "no target defined"
#endif // _AMD64_

//
//  Each NT Component that exports system call APIs to user programs
//  should have its own include file included here.
//

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

#include "mce.h"

#if defined(_AMD64_)
#include "nxamd64.h"

#elif defined(_X86_)
#include "nxi386.h"

#elif defined(_ARM_)
#include "nxarm.h"

#elif defined(_ARM64_)
#include "nxarm64.h"

#else
#error "no target defined"
#endif // _AMD64_

#if defined (_MSC_VER)
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4001)
#pragma warning(default:4201)
#pragma warning(default:4214)
#endif
#endif

#endif // NT_INCLUDED

