/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfutil.h

Abstract:

    This header contains prototypes for various functions required to do driver
    verification.

--*/

typedef enum {

    VFMP_INSTANT = 0,
    VFMP_INSTANT_NONPAGED

} MEMORY_PERSISTANCE;

BOOLEAN
VfUtilIsMemoryRangeReadable(
    IN PVOID                Location,
    IN size_t               Length,
    IN MEMORY_PERSISTANCE   Persistance
    );

VOID
VfSetVerifierEnabled (
    LOGICAL Value
    );

