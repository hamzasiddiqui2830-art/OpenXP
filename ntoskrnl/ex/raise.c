/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    raise.c

Abstract:

    This module implements routines to raise datatype misalignment and
    access violation for probe code.

    N.B. These routines are provided as function to save space in the
        probe macros.

    N.B. Since these routines are *only* called from the probe macros,
        it is assumed that the calling code is pageable.

--*/

#include "exp.h"

//
// Define function sections.
//

#pragma alloc_text(PAGE, ExRaiseAccessViolation)
#pragma alloc_text(PAGE, ExRaiseDatatypeMisalignment)

DECLSPEC_NOINLINE
VOID
ExRaiseAccessViolation (
    VOID
    )

/*++

Routine Description:

    This function raises an access violation exception.

    N.B. There is not return from this function.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ExRaiseStatus(STATUS_ACCESS_VIOLATION);
}

DECLSPEC_NOINLINE
VOID
ExRaiseDatatypeMisalignment (
    VOID
    )

/*++

Routine Description:

    This function raises a datatype misalignment exception.

    N.B. There is not return from this function.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ExRaiseStatus(STATUS_DATATYPE_MISALIGNMENT);
}

