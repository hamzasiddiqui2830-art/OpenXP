/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    dpclock.c

Abstract:

    This module contains the implementation for threaded DPC spin lock
    acquire and release functions.

--*/

#include "ki.h"

KIRQL
FASTCALL
KeAcquireSpinLockForDpc (
    __inout PKSPIN_LOCK SpinLock
    )

/*++

Routine Description:

    This function conditionally raises IRQL to DISPATCH_LEVEL and acquires
    the specified spin lock.

    N.B. The conditional IRQL raise is predicated on whether a thread DPC 
         is enabled.

Arguments:

    SpinLock - Supplies the address of a spin lock.

Return Value:

    If the IRQL is raised, then the previous IRQL is returned. Otherwise, zero
    is returned.

--*/

{

    return KiAcquireSpinLockForDpc(SpinLock);
}

VOID
FASTCALL
KeReleaseSpinLockForDpc (
    __inout PKSPIN_LOCK SpinLock,
    __in KIRQL OldIrql
    )

/*++

Routine Description:

    This function releases the specified spin lock and conditionally lowers
    IRQL to its previous value.

    N.B. The conditional IRQL raise is predicated on whether a thread DPC 
         is enabled.

Arguments:

    SpinLock - Supplies the address of a spin lock.

    OldIrql - Supplies the previous IRQL.

Return Value:

    None.

--*/

{

    KiReleaseSpinLockForDpc(SpinLock, OldIrql);
    return;
}


VOID
FASTCALL
KeAcquireInStackQueuedSpinLockForDpc (
    __inout PKSPIN_LOCK SpinLock,
    __out PKLOCK_QUEUE_HANDLE LockHandle
    )

/*++

Routine Description:

    This function conditionally raises IRQL to DISPATCH_LEVEL and acquires
    the specified in-stack spin lock.

    N.B. The conditional IRQL raise is predicated on whether a thread DPC 
         is enabled.

Arguments:

    SpinLock - Supplies the address of a spin lock.

    LockHandle - Supplies the address of a lock handle.

Return Value:

    None.

--*/

{

    KiAcquireInStackQueuedSpinLockForDpc(SpinLock, LockHandle);
    return;
}

VOID
FASTCALL
KeReleaseInStackQueuedSpinLockForDpc (
    __in PKLOCK_QUEUE_HANDLE LockHandle
    )

/*++

Routine Description:

    This function releases the specified in-stack spin lock and conditionally
    lowers IRQL to its previous value.

    N.B. The conditional IRQL raise is predicated on whether a thread DPC 
         is enabled.

Arguments:

    LockHandle - Supplies the address of a lock handle.

Return Value:

    None.

--*/

{

    KiReleaseInStackQueuedSpinLockForDpc(LockHandle);
    return;
}

