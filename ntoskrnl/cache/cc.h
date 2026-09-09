/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    cc.h

Abstract:

    This module is a header file for the Memory Management based cache
    management routines for the common Cache subsystem.

--*/

#ifndef _CCh_
#define _CCh_

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses

#include <ntdef.h>
#include <ntexapi.h>
#include <ntos.h>
#include <NtIoLogc.h>
#include <ntkeapi.h>

//
// Define ALIGNED_SPINLOCK type
//
typedef KSPIN_LOCK ALIGNED_SPINLOCK;

//
// Define macros to acquire and release cache manager locks.
//

#define CcAcquireMasterLock( OldIrql ) \
    *( OldIrql ) = KeAcquireQueuedSpinLock( LockQueueMasterLock )

#define CcReleaseMasterLock( OldIrql ) \
    KeReleaseQueuedSpinLock( LockQueueMasterLock, OldIrql )

#define CcAcquireMasterLockAtDpcLevel() \
    KeAcquireQueuedSpinLockAtDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueMasterLock] )

#define CcReleaseMasterLockFromDpcLevel() \
    KeReleaseQueuedSpinLockFromDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueMasterLock] )

#define CcAcquireVacbLock( OldIrql ) \
    *( OldIrql ) = KeAcquireQueuedSpinLock( LockQueueVacbLock )

#define CcReleaseVacbLock( OldIrql ) \
    KeReleaseQueuedSpinLock( LockQueueVacbLock, OldIrql )

#define CcAcquireVacbLockAtDpcLevel() \
    KeAcquireQueuedSpinLockAtDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueVacbLock] )

#define CcReleaseVacbLockFromDpcLevel() \
    KeReleaseQueuedSpinLockFromDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueVacbLock] )

#define CcAcquireWorkQueueLock( OldIrql ) \
    *( OldIrql ) = KeAcquireQueuedSpinLock( LockQueueWorkQueueLock )

#define CcReleaseWorkQueueLock( OldIrql ) \
    KeReleaseQueuedSpinLock( LockQueueWorkQueueLock, OldIrql )

#define CcAcquireWorkQueueLockAtDpcLevel() \
    KeAcquireQueuedSpinLockAtDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueWorkQueueLock] )

#define CcReleaseWorkQueueLockFromDpcLevel() \
    KeReleaseQueuedSpinLockFromDpcLevel( &KeGetCurrentPrcb()->LockQueue[LockQueueWorkQueueLock] )

#include <FsRtl.h>
