/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    Close.c

Abstract:

    This module implements the File Close routine for Raw called by the
    dispatch driver.

--*/

#include "RawProcs.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, RawClose)
#endif

NTSTATUS
RawClose (
    IN PVCB Vcb,
    IN PIRP Irp,
    IN PIO_STACK_LOCATION IrpSp
    )

/*++

Routine Description:

    This is the routine for closing a volume.

Arguments:

    Vcb - Supplies the volume being queried.

    Irp - Supplies the Irp being processed.

    IrpSp - Supplies parameters describing the read

Return Value:

    NTSTATUS - The return status for the operation

--*/

{
    NTSTATUS Status;
    BOOLEAN DeleteVolume = FALSE;

    PAGED_CODE();

    //
    //  This is a close operation.  If it is the last one, dismount.
    //

    //
    // Skip stream files as they are unopened fileobjects.
    // This might be a close from IopInvalidateVolumesForDevice
    // 
    if (IrpSp->FileObject->Flags & FO_STREAM_FILE) {
        RawCompleteRequest( Irp, STATUS_SUCCESS );
        return STATUS_SUCCESS;
    }

    Status = KeWaitForSingleObject( &Vcb->Mutex,
                                   Executive,
                                   KernelMode,
                                   FALSE,
                                   (PLARGE_INTEGER) NULL );
    ASSERT( NT_SUCCESS( Status ) );

    Vcb->OpenCount -= 1;

    if (Vcb->OpenCount == 0) {

        DeleteVolume = RawCheckForDismount( Vcb, FALSE );
    }

    if (!DeleteVolume) {
        (VOID)KeReleaseMutex( &Vcb->Mutex, FALSE );
    }

    RawCompleteRequest( Irp, STATUS_SUCCESS );

    return STATUS_SUCCESS;
}

