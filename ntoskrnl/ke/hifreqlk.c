/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    hifreqlk.c

Abstract:

    This module contains the declaration and allocation of high frequency
    spin locks.

--*/

#include "ki.h"

#if !defined(NT_UP)

#pragma data_seg("SPINLOCK")

#endif

//
// Cache manager spin locks.
//

ALIGNED_SPINLOCK CcMasterSpinLock = 0;
ALIGNED_SPINLOCK CcWorkQueueSpinLock = 0;
ALIGNED_SPINLOCK CcVacbSpinLock = 0;
ALIGNED_SPINLOCK CcDeferredWriteSpinLock = 0;
ALIGNED_SPINLOCK CcDebugTraceLock = 0;
ALIGNED_SPINLOCK CcBcbSpinLock = 0;

//
// Execute spin locks.
//

ALIGNED_SPINLOCK NonPagedPoolLock = 0;
ALIGNED_SPINLOCK ExpResourceSpinLock = 0;

//
// I/O spin locks.
//

ALIGNED_SPINLOCK IopCompletionLock = 0;
ALIGNED_SPINLOCK IopCancelSpinLock = 0;
ALIGNED_SPINLOCK IopVpbSpinLock = 0;
ALIGNED_SPINLOCK IopDatabaseLock = 0;
ALIGNED_SPINLOCK IopErrorLogLock = 0;
ALIGNED_SPINLOCK IopTimerLock = 0;
ALIGNED_SPINLOCK IoStatisticsLock = 0;

//
// Kernel debugger spin locks.
//

ALIGNED_SPINLOCK KdpDebuggerLock = 0;

//
// Kernel spin locks.
//

ALIGNED_SPINLOCK KiDispatcherLock = 0;
ALIGNED_SPINLOCK KiFreezeExecutionLock = 0;
ALIGNED_SPINLOCK KiFreezeLockBackup = 0;
ALIGNED_SPINLOCK KiProcessListLock = 0;
ALIGNED_SPINLOCK KiProfileLock = 0;
ALIGNED_SPINLOCK KiReverseStallIpiLock = 0;
ALIGNED_SPINLOCK_STRUCT KiTimerTableLock[LOCK_QUEUE_TIMER_TABLE_LOCKS] = {0};

#if defined(_AMD64_)

ALIGNED_SPINLOCK KiNMILock = 0;

#endif

//
// Memory management spin locks.
//

ALIGNED_SPINLOCK MmPfnLock = 0;
ALIGNED_SPINLOCK MmSystemSpaceLock = 0;
ALIGNED_SPINLOCK MmNonPagedPoolLock = 0;

//
// Process structure spin locks.
//

ALIGNED_SPINLOCK PsLoadedModuleSpinLock = 0;

//
// Ntfs spin locks.
//

ALIGNED_SPINLOCK NtfsStructLock = 0;

//
// Afd spin locks.
//

ALIGNED_SPINLOCK AfdWorkQueueSpinLock = 0;

//
// Dummy lock for alignment.
//

ALIGNED_SPINLOCK KiDummyLock = 0;

#pragma data_seg()

//
// Hardware trigger.
//

volatile LONG KiHardwareTrigger = 0;

