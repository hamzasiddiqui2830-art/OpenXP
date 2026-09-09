/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    heappriv.h

Abstract:

    Private include file used by the kernel heap allocator.

    The original WRK header also contains user-mode LFH, lookaside and
    performance-index declarations.  Those declarations are intentionally
    excluded from the NTOS kernel build because their types and APIs are
    user-mode-only.

--*/

#ifndef _RTL_HEAP_PRIVATE_
#define _RTL_HEAP_PRIVATE_

#include "heap.h"
#include "heappage.h"

#ifndef PRERELEASE
#define DISABLE_REGISTRY_TEST_HOOKS
#endif

#if DBG
#define HEAPASSERT(exp) if (!(exp)) RtlAssert( #exp, __FILE__, __LINE__, NULL )
#else
#define HEAPASSERT(exp)
#endif

#define MINIMUM_LOOKASIDE_DEPTH 4

extern const UCHAR CheckHeapFillPattern[ CHECK_HEAP_TAIL_SIZE ];

#ifdef NTOS_KERNEL_RUNTIME

#define RtlInitializeLockRoutine(L) ExInitializeResourceLite((PERESOURCE)(L))
#define RtlAcquireLockRoutine(L)    ExAcquireResourceExclusiveLite((PERESOURCE)(L),TRUE)
#define RtlReleaseLockRoutine(L)    ExReleaseResourceLite((PERESOURCE)(L))
#define RtlDeleteLockRoutine(L)     ExDeleteResourceLite((PERESOURCE)(L))
#define RtlOkayToLockRoutine(L)     ExOkayToLockRoutineLite((PERESOURCE)(L))

#define HEAP_DEBUG_FLAGS   0
#define DEBUG_HEAP(F)      FALSE
#define SET_LAST_STATUS(S) NOTHING;

#define HeapDebugPrint(_x_) {DbgPrint _x_;}
#define HeapDebugBreak(_x_) {if (KdDebuggerEnabled) DbgBreakPoint();}

#define RtlpHeapFreeVirtualMemory(P,A,S,F) ZwFreeVirtualMemory(P,A,S,F)

#else

#define RtlInitializeLockRoutine(L) RtlInitializeCriticalSectionAndSpinCount((PRTL_CRITICAL_SECTION)(L),(0x80000000 | 4000))
#define RtlAcquireLockRoutine(L)    RtlEnterCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlReleaseLockRoutine(L)    RtlLeaveCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlDeleteLockRoutine(L)     RtlDeleteCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlOkayToLockRoutine(L)     NtdllOkayToLockRoutine((PVOID)(L))

#define HEAP_DEBUG_FLAGS   (HEAP_VALIDATE_PARAMETERS_ENABLED | HEAP_VALIDATE_ALL_ENABLED | HEAP_CAPTURE_STACK_BACKTRACES | HEAP_CREATE_ENABLE_TRACING | HEAP_FLAG_PAGE_ALLOCS)
#define DEBUG_HEAP(F)      ((F & HEAP_DEBUG_FLAGS) && !(F & HEAP_SKIP_VALIDATION_CHECKS))
#define SET_LAST_STATUS(S) {NtCurrentTeb()->LastErrorValue = RtlNtStatusToDosError( NtCurrentTeb()->LastStatusValue = (ULONG)(S) );}

#define RtlpHeapFreeVirtualMemory(P,A,S,F) RtlpSecMemFreeVirtualMemory(P,A,S,F)

#endif

ULONG
RtlpHeapExceptionFilter (
    NTSTATUS ExceptionCode
    );

BOOLEAN
RtlpInitializeHeapSegment (
    IN PHEAP Heap,
    IN PHEAP_SEGMENT Segment,
    IN UCHAR SegmentIndex,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN PVOID UnCommittedAddress,
    IN PVOID CommitLimitAddress
    );

PHEAP_FREE_ENTRY
RtlpCoalesceFreeBlocks (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN OUT PSIZE_T FreeSize,
    IN BOOLEAN RemoveFromFreeList
    );

VOID
RtlpDeCommitFreeBlock (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN SIZE_T FreeSize
    );

VOID
RtlpInsertFreeBlock (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN SIZE_T FreeSize
    );

PHEAP_FREE_ENTRY
RtlpFindAndCommitPages (
    IN PHEAP Heap,
    IN PHEAP_SEGMENT Segment,
    IN OUT PSIZE_T Size,
    IN PVOID AddressWanted OPTIONAL
    );

PVOID
RtlAllocateHeapSlowly (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    );

BOOLEAN
RtlFreeHeapSlowly (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

SIZE_T
RtlpGetSizeOfBigBlock (
    IN PHEAP_ENTRY BusyBlock
    );

PHEAP_ENTRY_EXTRA
RtlpGetExtraStuffPointer (
    PHEAP_ENTRY BusyBlock
    );

BOOLEAN
RtlpCheckBusyBlockTail (
    IN PHEAP_ENTRY BusyBlock
    );

VOID
RtlpAddHeapToProcessList (
    IN PHEAP Heap
    );

VOID
RtlpRemoveHeapFromProcessList (
    IN PHEAP Heap
    );

PHEAP_FREE_ENTRY
RtlpCoalesceHeap (
    IN PHEAP Heap
    );

BOOLEAN
RtlpCheckHeapSignature (
    IN PHEAP Heap,
    IN PCHAR Caller
    );

VOID RtlDetectHeapLeaks(VOID);

BOOLEAN
RtlpValidateHeapEntry (
    IN PHEAP Heap,
    IN PHEAP_ENTRY BusyBlock,
    IN PCHAR Reason
    );

BOOLEAN
RtlpValidateHeap (
    IN PHEAP Heap,
    IN BOOLEAN AlwaysValidate
    );

VOID
RtlpUpdateHeapListIndex (
    USHORT OldIndex,
    USHORT NewIndex
    );

BOOLEAN
RtlpValidateHeapHeaders(
    IN PHEAP Heap,
    IN BOOLEAN Recompute
    );

#endif // _RTL_HEAP_PRIVATE_
