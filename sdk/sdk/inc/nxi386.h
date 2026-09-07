/*++

Copyright (c) OpenXP Contributors
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    nxi386.w

Abstract:

    User mode visible X86 specific structures and constants.

    This file contains platform specific definitions that are included
    after all other files have been included from nt.h.

--*/

#ifndef _NXI386_
#define _NXI386_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Define platform specific functions to access the TEB.
//

// begin_winnt

#if defined(_M_IX86) && !defined(MIDL_PASS)

#define PcTeb 0x18

#if (_MSC_FULL_VER >= 13012035)

__inline struct _TEB * NtCurrentTeb( void ) { return (struct _TEB *) (ULONG_PTR) __readfsdword (PcTeb); }

#else

#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning (disable:4035)        // disable 4035 (function must return something)

__inline struct _TEB * NtCurrentTeb( void ) { __asm mov eax, fs:[PcTeb] }

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning (default:4035)        // reenable it
#endif

#endif

#endif // defined(_M_IX86) && !defined(MIDL_PASS)

// end_winnt

#ifdef __cplusplus
}
#endif

#endif // _NXI386_

