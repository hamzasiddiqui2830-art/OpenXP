/*++

Copyright (c) Microsoft Corporation. All rights reserved.
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    nxamd64.w

Abstract:

    User mode visible AMD64 specific structures and constants.

    This file contains platform specific definitions that are included
    after all other files have been included from nt.h.

--*/

#ifndef _NXAMD64_
#define _NXAMD64_

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

#if defined(_M_AMD64) && !defined(__midl)

__forceinline
struct _TEB *
NtCurrentTeb (
    VOID
    )

{
    return (struct _TEB *)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
}

__forceinline
PVOID
GetCurrentFiber (
    VOID
    )

{

    return (PVOID)__readgsqword(FIELD_OFFSET(NT_TIB, FiberData));
}

__forceinline
PVOID
GetFiberData (
    VOID
    )

{

    return *(PVOID *)GetCurrentFiber();
}

#endif // _M_AMD64 && !defined(__midl)

// end_winnt

#ifdef __cplusplus
}
#endif

#endif // _NXAMD64_

