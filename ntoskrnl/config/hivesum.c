/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    hivesum.c

Abstract:

    Hive header checksum module.

--*/

#include    "cmp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,HvpHeaderCheckSum)
#endif

ULONG
HvpHeaderCheckSum(
    PHBASE_BLOCK    BaseBlock
    )
/*++

Routine Description:

    Compute the checksum for a hive disk header.

Arguments:

    BaseBlock - supplies pointer to the header to checksum

Return Value:

    the check sum.

--*/
{
    ULONG   sum;
    ULONG   i;

    sum = 0;
    for (i = 0; i < 127; i++) {
        sum ^= ((PULONG)BaseBlock)[i];
    }
    if (sum == (ULONG)-1) {
        sum = (ULONG)-2;
    }
    if (sum == 0) {
        sum = 1;
    }
    return sum;
}
