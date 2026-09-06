/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    yield.c

Abstract:

    This module implements the function to yield execution for one quantum
    to any other runnable thread.

--*/

#include "ki.h"

NTSTATUS
NtYieldExecution (
    VOID
    )

/*++

Routine Description:

    This function yields execution to any ready thread for up to one
    quantum.

Arguments:

    None.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;
    PKTHREAD NewThread;
    PRKPRCB Prcb;
    NTSTATUS Status;
    PKTHREAD Thread;

    //
    // If no other threads are ready, then return immediately. Otherwise,
    // attempt to yield execution.
    //
    // N.B. The test for ready threads is made outside any synchonization.
    //      Since this code cannot be perfectly synchronized under any
    //      conditions the lack of synchronization is of no consequence.
    //      

    if (KiGetCurrentReadySummary() == 0) {
        return STATUS_NO_YIELD_PERFORMED;

    } else {
        Status = STATUS_NO_YIELD_PERFORMED;
        Thread = KeGetCurrentThread();
        OldIrql = KeRaiseIrqlToSynchLevel();
        Prcb = KeGetCurrentPrcb();
        if (Prcb->ReadySummary != 0) {
    
            //
            // Acquire the thread lock and the PRCB lock.
            //
            // If a thread has not already been selected for execution, then
            // attempt to select another thread for execution.
            //
    
            KiAcquireThreadLock(Thread);
            KiAcquirePrcbLock(Prcb);
            if (Prcb->NextThread == NULL) {
                Prcb->NextThread = KiSelectReadyThread(1, Prcb);
            }
    
            //
            // If a new thread has been selected for execution, then switch
            // immediately to the selected thread.
            //
    
            if ((NewThread = Prcb->NextThread) != NULL) {
                Thread->Quantum = Thread->QuantumReset;
    
                //
                // Compute the new thread priority.
                //
                // N.B. The new priority will never be greater than the previous
                //      priority.
                //
    
                Thread->Priority = KiComputeNewPriority(Thread, 1);
    
                //
                // Release the thread lock, set swap busy for the old thread,
                // set the next thread to NULL, set the current thread to the
                // new thread, set the new thread state to running, set the
                // wait reason, queue the old running thread, and release the
                // PRCB lock, and swp context to the new thread.
                //
    
                KiReleaseThreadLock(Thread);
                KiSetContextSwapBusy(Thread);
                Prcb->NextThread = NULL;
                Prcb->CurrentThread = NewThread;
                NewThread->State = Running;
                Thread->WaitReason = WrYieldExecution;
                KxQueueReadyThread(Thread, Prcb);
                Thread->WaitIrql = APC_LEVEL;
    
                ASSERT(OldIrql <= DISPATCH_LEVEL);
    
                KiSwapContext(Thread, NewThread);
                Status = STATUS_SUCCESS;
    
            } else {
                KiReleasePrcbLock(Prcb);
                KiReleaseThreadLock(Thread);
            }
        }
    
        //
        // Lower IRQL to its previous level and return.
        //
    
        KeLowerIrql(OldIrql);
        return Status;
    }
}

