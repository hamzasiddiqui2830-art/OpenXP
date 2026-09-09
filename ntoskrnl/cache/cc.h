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

//
//  Peek at number of available pages.
//

extern PFN_NUMBER MmAvailablePages;

//
//  Define our node type codes.
//

#define CACHE_NTC_SHARED_CACHE_MAP       (0x2FF)
#define CACHE_NTC_PRIVATE_CACHE_MAP      (0x2FE)
#define CACHE_NTC_BCB                    (0x2FD)
#define CACHE_NTC_DEFERRED_WRITE         (0x2FC)
#define CACHE_NTC_MBCB                   (0x2FB)
#define CACHE_NTC_OBCB                   (0x2FA)
#define CACHE_NTC_MBCB_GRANDE            (0x2F9)

//
//  The following definitions are used to generate meaningful blue bugcheck
//  screens.  On a bugcheck the file system can output 4 ulongs of useful
//  information.  The first ulong will have encoded in it a source file id
//  (in the high word) and the line number of the bugcheck (in the low word).
