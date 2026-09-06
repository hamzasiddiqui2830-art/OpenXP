/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    intbits.c

Abstract:

    This module contains routines to do interlocked bit manipulation

--*/

#include "ntrtlp.h"
#pragma hdrstop

ULONG
FASTCALL
RtlInterlockedSetClearBits (
    IN OUT PULONG Flags,
    IN ULONG sFlag,
    IN ULONG cFlag
    )

/*++

Routine Description:

    This function atomically sets and clears the specified flags in the target

Arguments:

    Flags - Pointer to variable containing current mask.

    sFlag  - Flags to set in target

    CFlag  - Flags to clear in target

Return Value:

    ULONG - Old value of mask before modification

--*/

{

    ULONG NewFlags, OldFlags;

    OldFlags = *Flags;
    NewFlags = (OldFlags | sFlag) & ~cFlag;
    while (NewFlags != OldFlags) {
        NewFlags = InterlockedCompareExchange ((PLONG) Flags, (LONG) NewFlags, (LONG) OldFlags);
        if (NewFlags == OldFlags) {
            break;
        }

        OldFlags = NewFlags;
        NewFlags = (NewFlags | sFlag) & ~cFlag;
    }

    return OldFlags;
}

