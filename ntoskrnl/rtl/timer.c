/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    timer.c

Abstract:

    This module defines functions for the timer thread pool.

Author:

    Gurdeep Singh Pall (gurdeep) Nov 13, 1997

Revision History:

    lokeshs - extended/modified threadpool.
    Rob Earhart (earhart) September 29, 2000
      Split off from threads.c

Environment:

    These routines are statically linked in the caller's executable
    and are callable only from user mode. They make use of Nt system
    services.


--*/

#include <ntos.h>
#include <ntrtl.h>
#include <wow64t.h>
#include "ntrtlp.h"
#include "threads.h"

/* Timer Thread Pool */
ULONG StartedTimerInitialization ;
ULONG CompletedTimerInitialization ;
HANDLE TimerThreadHandle ;
ULONG TimerThreadId ;
LIST_ENTRY TimerQueues ;
HANDLE TimerHandle ;
HANDLE TimerThreadStartedEvent ;
ULONG NumTimerQueues ;
RTL_CRITICAL_SECTION TimerCriticalSection ;
LARGE_INTEGER Last64BitTickCount ;
LARGE_INTEGER Resync64BitTickCount ;
LARGE_INTEGER Firing64BitTickCount ;

#if DBG
ULONG RtlpDueTimeMax = 0;
#endif

#if DBG1
ULONG NextTimerDbgId;
#endif

#define RtlpGetResync64BitTickCount()  Resync64BitTickCount.QuadPart
#define RtlpSetFiring64BitTickCount(Timeout) \
            Firing64BitTickCount.QuadPart = (Timeout)

__inline
LONGLONG
RtlpGet64BitTickCount(
    LARGE_INTEGER *Last64BitTickCount
    )
{
    LARGE_INTEGER liCurTime ;
    liCurTime.QuadPart = NtGetTickCount() + Last64BitTickCount->HighPart ;
    if (liCurTime.LowPart < Last64BitTickCount->LowPart) {
        liCurTime.HighPart++ ;
    }
    return (Last64BitTickCount->QuadPart = liCurTime.QuadPart) ;
}

__inline
LONGLONG
RtlpResync64BitTickCount(
    )
{
    return Resync64BitTickCount.QuadPart =
                    RtlpGet64BitTickCount(&Last64BitTickCount);
}
