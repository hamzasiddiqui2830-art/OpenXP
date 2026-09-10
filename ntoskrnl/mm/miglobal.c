/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

   miglobal.c

Abstract:

    This module contains the private global storage for the memory
    management subsystem.

--*/
#include "mi.h"

#if !defined(_WIN64)

//
// Virtual bias applied during the loading of the kernel image.
//

ULONG_PTR MmVirtualBias;

#if defined(_X86_)
MMPTE MmPteGlobal = {0x0};
PVOID MmHyperSpaceEnd;
#endif

#endif

//
// The starting color index seed, incremented at each process creation.
//

ULONG MmProcessColorSeed = 0x12345678;

PFN_COUNT MmNumberOfPhysicalPages;
PFN_NUMBER MmLowestPhysicalPage = (PFN_NUMBER)-1;
PFN_NUMBER MmHighestPossiblePhysicalPage;
PFN_NUMBER DECLSPEC_CACHEALIGN MmAvailablePages;
PFN_NUMBER MmThrottleTop;
PFN_NUMBER MmThrottleBottom;
ULONG MiLastVadBit = 1;
SPFN_NUMBER MmResidentAvailablePages;
PFN_NUMBER DECLSPEC_CACHEALIGN MmPagesAboveWsMinimum;
PFN_NUMBER MmPagesAboveWsThreshold = 37;
PFN_NUMBER MmHiberPages = 768;
ULONG MmLargePageMinimum;

MMPFNLIST MmZeroedPageListHead = {0, ZeroedPageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmFreePageListHead = {0, FreePageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmStandbyPageListHead = {0, StandbyPageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmStandbyPageListByPriority[MI_PFN_PRIORITIES];
MMPFNLIST MmModifiedPageListHead = {0, ModifiedPageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmModifiedNoWritePageListHead = {0, ModifiedNoWritePageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmBadPageListHead = {0, BadPageList, MM_EMPTY_LIST, MM_EMPTY_LIST};
MMPFNLIST MmRomPageListHead = {0, StandbyPageList, MM_EMPTY_LIST, MM_EMPTY_LIST};

PMMPFNLIST MmPageLocationList[NUMBER_OF_PAGE_LISTS] = {
    &MmZeroedPageListHead, &MmFreePageListHead, &MmStandbyPageListHead,
    &MmModifiedPageListHead, &MmModifiedNoWritePageListHead, &MmBadPageListHead,
    NULL, NULL
};

PMMPTE MiHighestUserPte;
PMMPTE MiHighestUserPde;
#if (_MI_PAGING_LEVELS >= 4)
PMMPTE MiHighestUserPpe;
PMMPTE MiHighestUserPxe;
#endif
PMMPTE MiSessionBasePte;
PMMPTE MiSessionLastPte;
PMMPTE MmFirstReservedMappingPte;
PMMPTE MmLastReservedMappingPte;
KEVENT MmAvailablePagesEvent;
KEVENT MmZeroingPageEvent;
BOOLEAN MmZeroingPageThreadActive;
PFN_NUMBER MmMinimumFreePagesToZero = 8;
PVOID MmNonPagedSystemStart;
LOGICAL MmProtectFreedNonPagedPool;
PFN_NUMBER MmDynamicPfn;

#ifdef MM_BUMP_COUNTER_MAX
SIZE_T MmResTrack[MM_BUMP_COUNTER_MAX];
#endif
#ifdef MM_COMMIT_COUNTER_MAX
SIZE_T MmTrackCommit[MM_COMMIT_COUNTER_MAX];
#endif

LOGICAL MmTrackLockedPages;
LOGICAL MmSnapUnloads = TRUE;
#if DBG
PETHREAD MiExpansionLockOwner;
#endif

SIZE_T MmSizeOfNonPagedPoolInBytes;
SIZE_T MmMaximumNonPagedPoolInBytes;
PFN_NUMBER MmMaximumNonPagedPoolInPages;
ULONG MmMaximumNonPagedPoolPercent;
SIZE_T MmMinimumNonPagedPoolSize = 256 * 1024;
ULONG MmMinAdditionNonPagedPoolPerMb = 32 * 1024;
SIZE_T MmDefaultMaximumNonPagedPool = 1024 * 1024;
ULONG MmMaxAdditionNonPagedPoolPerMb = 400 * 1024;
SIZE_T MmSizeOfPagedPoolInBytes = 32 * 1024 * 1024;
PFN_NUMBER MmSizeOfPagedPoolInPages = (32 * 1024 * 1024) / PAGE_SIZE;
ULONG MmNumberOfSystemPtes;
ULONG MiRequestedSystemPtes;
PMMPTE MmFirstPteForPagedPool;
PMMPTE MmLastPteForPagedPool;
PFN_NUMBER MmNumberOfFreeNonPagedPool;
MMPTE MmFirstFreeSystemPte[MaximumPtePoolTypes];

PMMWSL MmSystemCacheWorkingSetList = (PMMWSL)MM_SYSTEM_CACHE_WORKING_SET;
MMSUPPORT MmSystemCacheWs;
PMMWSLE MmSystemCacheWsle;
PVOID MmSystemCacheStart = (PVOID)MM_SYSTEM_CACHE_START;
PVOID MmSystemCacheEnd;
PFN_NUMBER MmSizeOfSystemCacheInPages;
PFN_NUMBER MmSystemCacheWsMinimum = 288;
ULONG MmMaximumDeadKernelStacks = 5;
SLIST_HEADER MmDeadStackSListHead;
SLIST_HEADER MmEventCountSListHead;
SLIST_HEADER MmInPageSupportSListHead;
PMMPTE MmSystemPteBase;
MM_AVL_TABLE MmSectionBasedRoot;
PVOID MmHighSectionBase;
POBJECT_TYPE MmSectionObjectType;
KGUARDED_MUTEX MmSectionCommitMutex;
KGUARDED_MUTEX MmSectionBasedMutex;
ERESOURCE MmSectionExtendResource;
ERESOURCE MmSectionExtendSetResource;
KGUARDED_MUTEX MmPageFileCreationLock;
MMDEREFERENCE_SEGMENT_HEADER MmDereferenceSegmentHeader;
LIST_ENTRY MmUnusedSegmentList;
LIST_ENTRY MmUnusedSubsectionList;
KEVENT MmUnusedSegmentCleanup;
ULONG MmUnusedSegmentCount;
ULONG MmUnusedSubsectionCount;
ULONG MmUnusedSubsectionCountPeak;
SIZE_T MiUnusedSubsectionPagedPool;
SIZE_T MiUnusedSubsectionPagedPoolPeak;
ULONG MmConsumedPoolPercentage;
MMWORKING_SET_EXPANSION_HEAD MmWorkingSetExpansionHead;
MMPAGE_FILE_EXPANSION MmAttemptForCantExtend;
MMMOD_WRITER_LISTHEAD MmPagingFileHeader;
MMMOD_WRITER_LISTHEAD MmMappedFileHeader;
LIST_ENTRY MmFreePagingSpaceLow;
ULONG MmNumberOfActiveMdlEntries;
PMMPAGING_FILE MmPagingFile[MAX_PAGE_FILES];
ULONG MmNumberOfPagingFiles;
KEVENT MmModifiedPageWriterEvent;
KEVENT MmWorkingSetManagerEvent;
KEVENT MmCollidedFlushEvent;
SIZE_T MmTotalCommittedPages;
#if DBG
SPFN_NUMBER MiLockedCommit;
#endif
SIZE_T MmTotalCommitLimit;
SIZE_T MmTotalCommitLimitMaximum;
ULONG MiChargeCommitmentFailures[3];
PFN_NUMBER MmMinimumFreePages = 26;
PFN_NUMBER MmFreeGoal = 100;
PFN_NUMBER MmModifiedPageMaximum;
ULONG MmMinimumFreeDiskSpace = 1024 * 1024;
ULONG MmPageFileExtension = 256;
ULONG MmMinimumPageFileReduction = 256;
ULONG MmModifiedWriteClusterSize = MM_MAXIMUM_WRITE_CLUSTER;
ULONG MmReadClusterSize = 7;
const ULONG MMSECT = 'tSmM';
KSPIN_LOCK MmExpansionLock;
PFN_NUMBER MmSystemProcessWorkingSetMin = 50;
PFN_NUMBER MmSystemProcessWorkingSetMax = 450;
WSLE_NUMBER MmMaximumWorkingSetSize;
PFN_NUMBER MmMinimumWorkingSetSize = 20;
ULONG MmSystemPageColor;
const LARGE_INTEGER MmSevenMinutes = {0, -1};
const LARGE_INTEGER MmOneSecond = {(ULONG)(-1 * 1000 * 1000 * 10), -1};
const LARGE_INTEGER MmTwentySeconds = {(ULONG)(-20 * 1000 * 1000 * 10), -1};
const LARGE_INTEGER MmSeventySeconds = {(ULONG)(-70 * 1000 * 1000 * 10), -1};
const LARGE_INTEGER MmShortTime = {(ULONG)(-10 * 1000 * 10), -1};
const LARGE_INTEGER MmHalfSecond = {(ULONG)(-5 * 100 * 1000 * 10), -1};
const LARGE_INTEGER Mm30Milliseconds = {(ULONG)(-30 * 1000 * 10), -1};
LARGE_INTEGER MmCriticalSectionTimeout;
SIZE_T MmHeapSegmentReserve = 1024 * 1024;
SIZE_T MmHeapSegmentCommit = PAGE_SIZE * 2;
SIZE_T MmHeapDeCommitTotalFreeThreshold = 64 * 1024;
SIZE_T MmHeapDeCommitFreeBlockThreshold = PAGE_SIZE;
ULONG MmDisablePagingExecutive;
BOOLEAN Mm64BitPhysicalAddress;
#if DBG
ULONG MmDebug;
#endif

ULONG MmProtectToValue[32] = {
    PAGE_NOACCESS, PAGE_READONLY, PAGE_EXECUTE, PAGE_EXECUTE_READ,
    PAGE_READWRITE, PAGE_WRITECOPY, PAGE_EXECUTE_READWRITE, PAGE_EXECUTE_WRITECOPY,
    PAGE_NOACCESS, PAGE_NOCACHE | PAGE_READONLY, PAGE_NOCACHE | PAGE_EXECUTE,
    PAGE_NOCACHE | PAGE_EXECUTE_READ, PAGE_NOCACHE | PAGE_READWRITE,
    PAGE_NOCACHE | PAGE_WRITECOPY, PAGE_NOCACHE | PAGE_EXECUTE_READWRITE,
    PAGE_NOCACHE | PAGE_EXECUTE_WRITECOPY, PAGE_NOACCESS, PAGE_GUARD | PAGE_READONLY,
    PAGE_GUARD | PAGE_EXECUTE, PAGE_GUARD | PAGE_EXECUTE_READ,
    PAGE_GUARD | PAGE_READWRITE, PAGE_GUARD | PAGE_WRITECOPY,
    PAGE_GUARD | PAGE_EXECUTE_READWRITE, PAGE_GUARD | PAGE_EXECUTE_WRITECOPY,
    PAGE_NOACCESS, PAGE_WRITECOMBINE | PAGE_READONLY, PAGE_WRITECOMBINE | PAGE_EXECUTE,
    PAGE_WRITECOMBINE | PAGE_EXECUTE_READ, PAGE_WRITECOMBINE | PAGE_READWRITE,
    PAGE_WRITECOMBINE | PAGE_WRITECOPY, PAGE_WRITECOMBINE | PAGE_EXECUTE_READWRITE,
    PAGE_WRITECOMBINE | PAGE_EXECUTE_WRITECOPY
};

#if defined(_WIN64) || defined(_X86PAE_)
ULONGLONG
#else
ULONG
#endif
MmProtectToPteMask[32] = {
    MM_PTE_NOACCESS, MM_PTE_READONLY | MM_PTE_CACHE, MM_PTE_EXECUTE | MM_PTE_CACHE,
    MM_PTE_EXECUTE_READ | MM_PTE_CACHE, MM_PTE_READWRITE | MM_PTE_CACHE,
    MM_PTE_WRITECOPY | MM_PTE_CACHE, MM_PTE_EXECUTE_READWRITE | MM_PTE_CACHE,
    MM_PTE_EXECUTE_WRITECOPY | MM_PTE_CACHE, MM_PTE_NOACCESS, MM_PTE_NOCACHE | MM_PTE_READONLY,
    MM_PTE_NOCACHE | MM_PTE_EXECUTE, MM_PTE_NOCACHE | MM_PTE_EXECUTE_READ,
    MM_PTE_NOCACHE | MM_PTE_READWRITE, MM_PTE_NOCACHE | MM_PTE_WRITECOPY,
    MM_PTE_NOCACHE | MM_PTE_EXECUTE_READWRITE, MM_PTE_NOCACHE | MM_PTE_EXECUTE_WRITECOPY,
    MM_PTE_NOACCESS, MM_PTE_GUARD | MM_PTE_READONLY | MM_PTE_CACHE,
    MM_PTE_GUARD | MM_PTE_EXECUTE | MM_PTE_CACHE, MM_PTE_GUARD | MM_PTE_EXECUTE_READ | MM_PTE_CACHE,
    MM_PTE_GUARD | MM_PTE_READWRITE | MM_PTE_CACHE, MM_PTE_GUARD | MM_PTE_WRITECOPY | MM_PTE_CACHE,
    MM_PTE_GUARD | MM_PTE_EXECUTE_READWRITE | MM_PTE_CACHE,
    MM_PTE_GUARD | MM_PTE_EXECUTE_WRITECOPY | MM_PTE_CACHE, MM_PTE_NOACCESS,
    MM_PTE_WRITECOMBINE | MM_PTE_READONLY, MM_PTE_WRITECOMBINE | MM_PTE_EXECUTE,
    MM_PTE_WRITECOMBINE | MM_PTE_EXECUTE_READ, MM_PTE_WRITECOMBINE | MM_PTE_READWRITE,
    MM_PTE_WRITECOMBINE | MM_PTE_WRITECOPY, MM_PTE_WRITECOMBINE | MM_PTE_EXECUTE_READWRITE,
    MM_PTE_WRITECOMBINE | MM_PTE_EXECUTE_WRITECOPY
};

ULONG MmMakeProtectNotWriteCopy[32] = {
    MM_NOACCESS, MM_READONLY, MM_EXECUTE, MM_EXECUTE_READ, MM_READWRITE, MM_READWRITE,
    MM_EXECUTE_READWRITE, MM_EXECUTE_READWRITE, MM_NOACCESS, MM_NOCACHE | MM_READONLY,
    MM_NOCACHE | MM_EXECUTE, MM_NOCACHE | MM_EXECUTE_READ, MM_NOCACHE | MM_READWRITE,
    MM_NOCACHE | MM_READWRITE, MM_NOCACHE | MM_EXECUTE_READWRITE, MM_NOCACHE | MM_EXECUTE_READWRITE,
    MM_NOACCESS, MM_GUARD_PAGE | MM_READONLY, MM_GUARD_PAGE | MM_EXECUTE,
    MM_GUARD_PAGE | MM_EXECUTE_READ, MM_GUARD_PAGE | MM_READWRITE, MM_GUARD_PAGE | MM_READWRITE,
    MM_GUARD_PAGE | MM_EXECUTE_READWRITE, MM_GUARD_PAGE | MM_EXECUTE_READWRITE, MM_NOACCESS,
    MM_WRITECOMBINE | MM_READONLY, MM_WRITECOMBINE | MM_EXECUTE, MM_WRITECOMBINE | MM_EXECUTE_READ,
    MM_WRITECOMBINE | MM_READWRITE, MM_WRITECOMBINE | MM_READWRITE,
    MM_WRITECOMBINE | MM_EXECUTE_READWRITE, MM_WRITECOMBINE | MM_EXECUTE_READWRITE
};

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("PAGEDATA")
#endif
ACCESS_MASK MmMakeSectionAccess[8] = {SECTION_MAP_READ, SECTION_MAP_READ, SECTION_MAP_EXECUTE,
    SECTION_MAP_EXECUTE | SECTION_MAP_READ, SECTION_MAP_WRITE, SECTION_MAP_READ,
    SECTION_MAP_EXECUTE | SECTION_MAP_WRITE, SECTION_MAP_EXECUTE | SECTION_MAP_READ};
ACCESS_MASK MmMakeFileAccess[8] = {FILE_READ_DATA, FILE_READ_DATA, FILE_EXECUTE,
    FILE_EXECUTE | FILE_READ_DATA, FILE_WRITE_DATA | FILE_READ_DATA, FILE_READ_DATA,
    FILE_EXECUTE | FILE_WRITE_DATA | FILE_READ_DATA, FILE_EXECUTE | FILE_READ_DATA};
#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg()
#endif

MM_PAGED_POOL_INFO MmPagedPoolInfo;
ULONG_PTR MmSessionBase;
PMM_SESSION_SPACE MmSessionSpace;
ULONG_PTR MiSessionSpaceWs;
SIZE_T MmSessionSize;
LIST_ENTRY MiSessionWsList;
ULONG_PTR MiSystemViewStart;
SIZE_T MmSystemViewSize;
ULONG_PTR MiSessionPoolStart;
ULONG_PTR MiSessionPoolEnd;
ULONG_PTR MiSessionSpaceEnd;
ULONG_PTR MiSessionViewStart;
ULONG MiSessionSpacePageTables;
SIZE_T MmSessionViewSize;
SIZE_T MmSessionPoolSize;
ULONG_PTR MiSessionImageStart;
ULONG_PTR MiSessionImageEnd;
PMMPTE MiSessionImagePteStart;
PMMPTE MiSessionImagePteEnd;
SIZE_T MmSessionImageSize;
MI_PFN_CACHE_ATTRIBUTE MiPlatformCacheAttributes[2 * MmMaximumCacheType] = {
    MiNonCached, MiCached, MiWriteCombined, MiCached, MiNonCached, MiWriteCombined,
    MiNonCached, MiCached, MiWriteCombined, MiCached, MiNonCached, MiWriteCombined
};
ULONG MiFlushTbForAttributeChange;
ULONG MiFlushCacheForAttributeChange;
ULONG MiIoRetryMask = 0x1f;
ULONG MiFaultRetryMask = 0x1f;
ULONG MiUserFaultRetryMask = 0xF;
#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("INIT")
#endif
WCHAR MmVerifyDriverBuffer[MI_SUSPECT_DRIVER_BUFFER_LENGTH] = {0};
ULONG MmVerifyDriverBufferType = REG_NONE;
ULONG MmVerifyDriverLevel = (ULONG)-1;
ULONG MmCritsectTimeoutSeconds = 2592000;
ULONG MmLargePageDriverBufferType = REG_NONE;
#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg()
#endif
WCHAR MmLargePageDriverBuffer[MI_LARGE_PAGE_DRIVER_BUFFER_LENGTH] = {0};
ULONG MmVerifyDriverBufferLength = sizeof(MmVerifyDriverBuffer);
ULONG MmLargePageDriverBufferLength = sizeof(MmLargePageDriverBuffer);
