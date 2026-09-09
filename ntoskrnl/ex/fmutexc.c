/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    fmutexc.c

Abstract:

    This module implements the code necessary to acquire and release fast
    mutexes.

--*/

#include "exp.h"

#undef ExAcquireFastMutex

VOID
FASTCALL
ExAcquireFastMutex (
    __inout PFAST_MUTEX FastMutex
    )

/*++

Routine Description:

    This function acquires ownership of a fast mutex and raises IRQL to
    APC Level.

Arguments:

    FastMutex  - Supplies a pointer to a fast mutex.

Return Value:

    None.

--*/

{

    xxAcquireFastMutex(FastMutex);
    return;
}

#undef ExReleaseFastMutex

VOID
FASTCALL
ExReleaseFastMutex (
    __inout PFAST_MUTEX FastMutex
    )

/*++

Routine Description:

    This function releases ownership to a fast mutex and lowers IRQL to
    its previous level.

Arguments:

    FastMutex - Supplies a pointer to a fast mutex.

Return Value:

    None.

--*/

{

    xxReleaseFastMutex(FastMutex);
    return;
}

#undef ExTryToAcquireFastMutex

BOOLEAN
FASTCALL
ExTryToAcquireFastMutex (
    __inout PFAST_MUTEX FastMutex
    )

/*++

Routine Description:

    This function attempts to acquire ownership of a fast mutex, and if
    successful, raises IRQL to APC level.

Arguments:

    FastMutex  - Supplies a pointer to a fast mutex.

Return Value:

    If the fast mutex was successfully acquired, then a value of TRUE
    is returned as the function value. Otherwise, a value of FALSE is
    returned.

--*/

{

    return xxTryToAcquireFastMutex(FastMutex);
}
