/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    interobj.c

Abstract:

    This module implements functions to acquire and release the spin lock
    associated with an interrupt object.

--*/

#include "ki.h"

KIRQL
KeAcquireInterruptSpinLock (
    __inout PKINTERRUPT Interrupt
    )

/*++

Routine Description:

    This function raises the IRQL to the interrupt synchronization level
    and acquires the actual spin lock associated with an interrupt object.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

Return Value:

    The previous IRQL is returned as the function value.

--*/

{

    KIRQL OldIrql;

    //
    // Raise IRQL to interrupt synchronization level and acquire the actual
    // spin lock associated with the interrupt object.
    //

    KeRaiseIrql(Interrupt->SynchronizeIrql, &OldIrql);
    KeAcquireSpinLockAtDpcLevel(Interrupt->ActualLock);
    return OldIrql;
}

VOID
KeReleaseInterruptSpinLock (
    __inout PKINTERRUPT Interrupt,
    __in KIRQL OldIrql
    )

/*++

Routine Description:

    This function releases the actual spin lock associated with an interrupt
    object and lowers the IRQL to its previous value.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

    OldIrql - Supplies the previous IRQL value.

Return Value:

    None.

--*/

{

    //
    // Release the actual spin lock associated with the interrupt object
    // and lower IRQL to its previous value.
    //

    KeReleaseSpinLockFromDpcLevel(Interrupt->ActualLock);
    KeLowerIrql(OldIrql);
    return;
}

