/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    delay.c

Abstract:

   This module implements the executive delay execution system service.

--*/

#include "exp.h"

#pragma alloc_text(PAGE, NtDelayExecution)

NTSTATUS
NtDelayExecution (
    __in BOOLEAN Alertable,
    __in PLARGE_INTEGER DelayInterval
    )

/*++

Routine Description:

    This function delays the execution of the current thread for the specified
    interval of time.

Arguments:

    Alertable - Supplies a boolean value that specifies whether the delay
        is alertable.

    DelayInterval - Supplies the absolute of relative time over which the
        delay is to occur.

Return Value:

    NTSTATUS.

--*/

{

    LARGE_INTEGER Interval;
    KPROCESSOR_MODE PreviousMode;

    //
    // Get previous processor mode and probe delay interval address if
    // necessary.
    //

    PreviousMode = KeGetPreviousMode();
    if (PreviousMode != KernelMode) {
        try {
            ProbeForReadSmallStructure(DelayInterval, sizeof(LARGE_INTEGER), sizeof(ULONG));
            Interval = *DelayInterval;

        } except(EXCEPTION_EXECUTE_HANDLER) {
            return GetExceptionCode();
        }

    } else {
        Interval = *DelayInterval;
    }

    //
    // Delay execution for the specified amount of time.
    //

    return KeDelayExecutionThread(PreviousMode, Alertable, &Interval);
}

