/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    luid.c

Abstract:

    This module implements the NT locally unique identifier services.

--*/

#include "exp.h"

//
//  Global variables needed to support locally unique IDs.
//
// The first 1000 values are reserved for static definition. This
// value can be increased with later releases with no adverse impact.
//
// N.B. The LUID source always refers to the "next" allocatable LUID.
//

LARGE_INTEGER ExpLuid = {1001,0};
const LARGE_INTEGER ExpLuidIncrement = {1,0};

#pragma alloc_text(INIT, ExLuidInitialization)
#pragma alloc_text(PAGE, NtAllocateLocallyUniqueId)

BOOLEAN
ExLuidInitialization (
    VOID
    )

/*++

Routine Description:

    This function initializes the locally unique identifier allocation.

    NOTE:  THE LUID ALLOCATION SERVICES ARE NEEDED BY SECURITY IN PHASE 0
           SYSTEM INITIALIZATION.  FOR THIS REASON, LUID INITIALIZATION IS
           PERFORMED AS PART OF PHASE 0 SECURITY INITIALIZATION.

Arguments:

    None.

Return Value:

    A value of TRUE is returned if the initialization is successfully
    completed.  Otherwise, a value of FALSE is returned.

--*/

{
    return TRUE;
}

NTSTATUS
NtAllocateLocallyUniqueId (
    __out PLUID Luid
    )

/*++

Routine Description:

    This function returns an LUID value that is unique since the system
    was last rebooted.  It is unique on the system it is generated on
    only (not network wide).

    There are no restrictions on who can allocate LUIDs.  The LUID space
    is large enough that this will never present a problem.  If one LUID
    is allocated every 100ns, they will not be exhausted for roughly
    15,000 years (100ns * 2^63).

Arguments:

    Luid - Supplies the address of a variable that will receive the
        new LUID.

Return Value:

    STATUS_SUCCESS is returned if the service is successfully executed.

    STATUS_ACCESS_VIOLATION is returned if the output parameter for the
        LUID cannot be written.

--*/

{

    KPROCESSOR_MODE PreviousMode;

    //
    // Get previous processor mode and probe argument if necessary.
    //

    try {
        PreviousMode = KeGetPreviousMode();
        if (PreviousMode != KernelMode) {
            ProbeForWriteSmallStructure((PVOID)Luid, sizeof(LUID), sizeof(ULONG));
        }

        //
        // Allocate and store a locally unique Id.
        //

        ExAllocateLocallyUniqueId(Luid);

    } except (ExSystemExceptionFilter()) {
        return GetExceptionCode();
    }

    return STATUS_SUCCESS;
}

