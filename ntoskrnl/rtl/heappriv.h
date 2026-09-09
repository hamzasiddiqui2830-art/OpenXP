/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    heappriv.h

Abstract:

    Private include file used by the kernel heap allocator.

--*/

#ifndef _RTL_HEAP_PRIVATE_
#define _RTL_HEAP_PRIVATE_

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

#define HEAP_DEBUG_FLAGS 0
#define DEBUG_HEAP(F) FALSE
#define SET_LAST_STATUS(S) NOTHING;
#define HeapDebugPrint(_x_) {DbgPrint _x_;}
#define HeapDebugBreak(_x_) {if (KdDebuggerEnabled) DbgBreakPoint();}
#define RtlpHeapFreeVirtualMemory(P,A,S,F) ZwFreeVirtualMemory(P,A,S,F)

/*
 * These definitions are present in the legacy heap private header only for
 * the user-mode heap. heap.c nevertheless references the common operations
 * in the kernel build, so provide the kernel no-op forms here.
 */
#define HEAP_OP_COUNT 2
#define HEAP_OP_ALLOC 0
#define HEAP_OP_FREE 1
#define HEAP_PERF_DECLARE_TIMER() UINT64 _HeapPerfStartTimer, _HeapPerfEndTimer;
#define HEAP_PERF_START_TIMER(H) do { _HeapPerfStartTimer = 0; } while (0)
#define HEAP_PERF_STOP_TIMER(H,OP) do { UNREFERENCED_PARAMETER((H)); UNREFERENCED_PARAMETER((OP)); } while (0)
#define RtlpRegisterOperation(H,S,Op) do { UNREFERENCED_PARAMETER((H)); UNREFERENCED_PARAMETER((S)); UNREFERENCED_PARAMETER((Op)); } while (0)

#define IS_HEAP_TAGGING_ENABLED(H) FALSE
#define RtlpSetSmallTagIndex(H,B,I) do { UNREFERENCED_PARAMETER((H)); UNREFERENCED_PARAMETER((B)); UNREFERENCED_PARAMETER((I)); } while (0)

#define RtlFindFirstSetRightMember(Set) \
    (((Set) & 0xFFFF) ? \
        (((Set) & 0xFF) ? \
            RtlpBitsClearLow[(Set) & 0xFF] : \
            RtlpBitsClearLow[((Set) >> 8) & 0xFF] + 8) : \
        ((((Set) >> 16) & 0xFF) ? \
            RtlpBitsClearLow[((Set) >> 16) & 0xFF] + 16 : \
            RtlpBitsClearLow[(Set) >> 24] + 24))

/*
 * The legacy index/LFH implementation is user-mode-only. The kernel heap
 * still uses the common call sites, so retain the legacy call signatures with
 * conservative kernel implementations.
 */
#define RtlpInsertFreeBlockDirect(H,FB,SIZE) RtlpInsertFreeBlock((H),(FB),(SIZE))
#define RtlpFastInsertFreeBlockDirect(H,FB,SIZE) RtlpInsertFreeBlockDirect((H),(FB),(SIZE))
#define RtlpFastInsertDedicatedFreeBlockDirect(H,FB,SIZE) RtlpInsertFreeBlockDirect((H),(FB),(SIZE))
#define RtlpFastInsertNonDedicatedFreeBlockDirect(H,FB,SIZE) RtlpInsertFreeBlockDirect((H),(FB),(SIZE))
#define RtlpFindEntry(H,S) ((PLIST_ENTRY)NULL)
#define RtlpRemoveFreeBlock(H,FB) do { UNREFERENCED_PARAMETER((H)); UNREFERENCED_PARAMETER((FB)); } while (0)
#define RtlpRemoveEntryList(E) RemoveEntryList((E))
#define RtlpFastRemoveFreeBlock(H,FB) RtlpRemoveFreeBlock((H),(FB))
#define RtlpFastRemoveDedicatedFreeBlock(H,FB) RtlpFastRemoveFreeBlock((H),(FB))
#define RtlpFastRemoveNonDedicatedFreeBlock(H,FB) RtlpFastRemoveFreeBlock((H),(FB))
#define RtlpHeapRemoveEntryList(E) RtlpRemoveEntryList((E))
#define RtlpQuickValidateBlock(H,B) TRUE
#define RtlpGetAllocationUnits(H,B) ((B)->Size)
#define RtlpGetUnusedBytes(H,B) ((B)->UnusedBytes)
#define RtlpSetUnusedBytes(H,B,N) do { (B)->UnusedBytes = (UCHAR)(((N) > 0xFF) ? 0xFF : (N)); } while (0)
#define RtlpGetExtraStuffPointer(B) ((PHEAP_ENTRY_EXTRA)((PHEAP_ENTRY)(B) + 1))
#define RtlpHeapReportCorruption(E) do { UNREFERENCED_PARAMETER((E)); } while (0)

#else

#ifndef PREALLOCATE_EVENT_MASK
#define PREALLOCATE_EVENT_MASK 0x80000000
#endif
#define RtlInitializeLockRoutine(L) RtlInitializeCriticalSectionAndSpinCount((PRTL_CRITICAL_SECTION)(L),(PREALLOCATE_EVENT_MASK | 4000))
#define RtlAcquireLockRoutine(L) RtlEnterCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlReleaseLockRoutine(L) RtlLeaveCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlDeleteLockRoutine(L) RtlDeleteCriticalSection((PRTL_CRITICAL_SECTION)(L))
#define RtlOkayToLockRoutine(L) NtdllOkayToLockRoutine((PVOID)(L))
#define HEAP_DEBUG_FLAGS (HEAP_VALIDATE_PARAMETERS_ENABLED | HEAP_VALIDATE_ALL_ENABLED | HEAP_CAPTURE_STACK_BACKTRACES | HEAP_CREATE_ENABLE_TRACING | HEAP_FLAG_PAGE_ALLOCS)
#define DEBUG_HEAP(F) ((F & HEAP_DEBUG_FLAGS) && !(F & HEAP_SKIP_VALIDATION_CHECKS))
#define SET_LAST_STATUS(S) {NtCurrentTeb()->LastErrorValue = RtlNtStatusToDosError( NtCurrentTeb()->LastStatusValue = (ULONG)(S) );}
#define RtlpHeapFreeVirtualMemory(P,A,S,F) RtlpSecMemFreeVirtualMemory(P,A,S,F)
#endif

ULONG RtlpHeapExceptionFilter(NTSTATUS ExceptionCode);

BOOLEAN RtlpInitializeHeapSegment(IN PHEAP Heap, IN PHEAP_SEGMENT Segment, IN UCHAR SegmentIndex, IN ULONG Flags, IN PVOID BaseAddress, IN PVOID UnCommittedAddress, IN PVOID CommitLimitAddress);
PHEAP_FREE_ENTRY RtlpCoalesceFreeBlocks(IN PHEAP Heap, IN PHEAP_FREE_ENTRY FreeBlock, IN OUT PSIZE_T FreeSize, IN BOOLEAN RemoveFromFreeList);
VOID RtlpDeCommitFreeBlock(IN PHEAP Heap, IN PHEAP_FREE_ENTRY FreeBlock, IN SIZE_T FreeSize);
VOID RtlpInsertFreeBlock(IN PHEAP Heap, IN PHEAP_FREE_ENTRY FreeBlock, IN SIZE_T FreeSize);
PHEAP_FREE_ENTRY RtlpFindAndCommitPages(IN PHEAP Heap, IN PHEAP_SEGMENT Segment, IN OUT PSIZE_T Size, IN PVOID AddressWanted OPTIONAL);
PVOID RtlAllocateHeapSlowly(IN PVOID HeapHandle, IN ULONG Flags, IN SIZE_T Size);
BOOLEAN RtlFreeHeapSlowly(IN PVOID HeapHandle, IN ULONG Flags, IN PVOID BaseAddress);
SIZE_T RtlpGetSizeOfBigBlock(IN PHEAP_ENTRY BusyBlock);
PHEAP_ENTRY_EXTRA RtlpGetExtraStuffPointer(IN PHEAP_ENTRY BusyBlock);
BOOLEAN RtlpCheckBusyBlockTail(IN PHEAP_ENTRY BusyBlock);

VOID RtlpAddHeapToProcessList(IN PHEAP Heap);
VOID RtlpRemoveHeapFromProcessList(IN PHEAP Heap);
PHEAP_FREE_ENTRY RtlpCoalesceHeap(IN PHEAP Heap);
BOOLEAN RtlpCheckHeapSignature(IN PHEAP Heap, IN PCHAR Caller);
VOID RtlDetectHeapLeaks(VOID);
BOOLEAN RtlpValidateHeapEntry(IN PHEAP Heap, IN PHEAP_ENTRY BusyBlock, IN PCHAR Reason);
BOOLEAN RtlpValidateHeap(IN PHEAP Heap, IN BOOLEAN AlwaysValidate);
VOID RtlpUpdateHeapListIndex(USHORT OldIndex, USHORT NewIndex);
BOOLEAN RtlpValidateHeapHeaders(IN PHEAP Heap, IN BOOLEAN Recompute);

#endif