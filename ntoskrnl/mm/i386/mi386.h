/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    mi386.h

Abstract:

    This module contains the private data structures and procedure
    prototypes for the hardware dependent portion of the
    memory management system.

    This module is specifically tailored for the X86.

Author:

    Lou Perazzoli (loup) 6-Jan-1990

Revision History:

--*/

#ifndef _MI386_
#define _MI386_

#if !defined (_X86PAE_)
#define PTE_KBASE 0xC0000000
#define PTE_UTOP  0xBFFFFFFF
#else
#define PTE_KBASE 0xC0000000
#define PTE_UTOP  0xBFFFFFFF
#endif

#define MI_PTE_BASE_FOR_LOWEST_KERNEL_ADDRESS ((PMMPTE)PTE_KBASE)
#define MI_PTE_BASE_FOR_LOWEST_SESSION_ADDRESS ((PMMPTE)0x0)

#define PAGE_SHIFT 12L
#define PAGE_SIZE 0x1000
#define PAGE_MASK 0xFFF
#define PTE_PER_PAGE_BITS 10
#define PDE_PER_PAGE ((ULONG)1024)
#define PTE_PER_PAGE ((ULONG)1024)
#define PD_PER_SYSTEM ((ULONG)1)

#define MM_EMPTY_LIST ((ULONG)0xFFFFFFFF)
#define MM_EMPTY_PTE_LIST ((ULONG)0xFFFFF)
#define MM_VIRTUAL_PAGE_FILLER 0
#define MM_VIRTUAL_PAGE_SIZE 20
#define MM_KSEG0_BASE ((ULONG)0x80000000)
#define MM_KSEG2_BASE ((ULONG)0xA0000000)
#define MM_SYSTEM_SPACE_END ((ULONG)0xFFFFFFFF)
#define MM_USER_ADDRESS_RANGE_LIMIT ((ULONG)0xFFFFFFFF)
#define MM_MAXIMUM_ZERO_BITS 21
#define MM_PROTO_PTE_ALIGNMENT ((ULONG)PAGE_SIZE)
#define PAGE_DIRECTORY_MASK ((ULONG)0x003FFFFF)
#define MM_VA_MAPPED_BY_PDE ((ULONG)0x400000)
#define MM_MINIMUM_VA_FOR_LARGE_PAGE MM_VA_MAPPED_BY_PDE
#define PTE_SHIFT 2
#define PHYSICAL_ADDRESS_BITS 32
#define MM_MAXIMUM_NUMBER_OF_COLORS (1)
#define MM_NUMBER_OF_COLORS (1)
#define MM_COLOR_MASK (0)
#define MM_COLOR_ALIGNMENT (0)
#define MM_COLOR_MASK_VIRTUAL (0)
#define MM_SECONDARY_COLORS_DEFAULT (64)
#define MM_SECONDARY_COLORS_MIN (8)
#define MM_SECONDARY_COLORS_MAX (1024)
#define MAX_PAGE_FILES 16
#define GUARD_PAGE_SIZE PAGE_SIZE
#define MM_USER_PAGE_TABLE_PAGES (768)
#define MM_USER_PAGE_DIRECTORY_PAGES (1)
#define MM_USER_PAGE_DIRECTORY_PARENT_PAGES (1)
#define MM_SYSTEM_CACHE_WORKING_SET ((ULONG)0xC0C00000)
#define MM_SYSTEM_CACHE_START ((ULONG)0xC1000000)
#define MM_SYSTEM_CACHE_END ((ULONG)0xE1000000)
#define MM_SYSTEM_VIEW_START ((ULONG)0xA0000000)
#define MM_SYSTEM_VIEW_SIZE (16 * 1024 * 1024)
#define MM_DEFAULT_PAGED_POOL_START ((ULONG)0xE1000000)
#define MM_LOWEST_NONPAGED_SYSTEM_START ((PVOID)0xEB000000)
#define MM_NONPAGED_POOL_END ((PVOID)0xFFBE0000)
#define MM_CRASH_DUMP_VA ((PVOID)0xFFBE0000)
#define MM_DEBUG_VA ((PVOID)0xFFBFF000)
#define NON_PAGED_SYSTEM_END ((ULONG)0xFFFFFFF0)
#define MM_MINIMUM_SYSTEM_PTES 7000
#define MM_MAXIMUM_SYSTEM_PTES 50000
#define MM_DEFAULT_SYSTEM_PTES 11000
#define MM_MAX_INITIAL_NONPAGED_POOL ((ULONG)(128 * 1024 * 1024))
#define MM_MAX_ADDITIONAL_NONPAGED_POOL ((ULONG)(128 * 1024 * 1024))

#ifndef MM_SECONDARY_COLORS
#define MM_SECONDARY_COLORS (MM_SECONDARY_COLORS_DEFAULT)
#endif

typedef struct _MMCOLOR_TABLES {
    PFN_NUMBER Flink;
    PVOID Blink;
    PFN_NUMBER Count;
} MMCOLOR_TABLES, *PMMCOLOR_TABLES;

extern PMMCOLOR_TABLES MmFreePagesByColor[2];

#define MI_PTE_OWNER_USER       1
#define MI_PTE_OWNER_KERNEL     0
#define MI_SET_OWNER_IN_PTE(PPTE,OWNER)  ((PPTE)->u.Hard.Owner = (OWNER))

#if !defined (_X86PAE_)
#define MI_PTE_LOOKUP_NEEDED ((ULONG)0xffffffff)
#else
#define MI_PTE_LOOKUP_NEEDED ((ULONG64)0xffffffff)
#endif

#define MM_PTE_CACHE_ENABLED     0
#define MM_PTE_CACHE_DISABLED    1

#if !defined (_X86PAE_)

typedef struct _MMPTE_SOFTWARE { ULONG Valid : 1; ULONG PageFileLow : 4; ULONG Protection : 5; ULONG Prototype : 1; ULONG Transition : 1; ULONG PageFileHigh : 20; } MMPTE_SOFTWARE;
typedef struct _MMPTE_TRANSITION { ULONG Valid : 1; ULONG Prototype : 1; ULONG Protection : 5; ULONG Transition : 1; ULONG Reserved0 : 3; ULONG PageFrameNumber : 20 - PAGE_SHIFT; ULONG Reserved : 11; } MMPTE_TRANSITION;
typedef struct _MMPTE_PROTOTYPE { ULONG Valid : 1; ULONG Prototype : 1; ULONG ReadOnly : 1; ULONG Reserved : 9; ULONG ProtoAddress : 20; } MMPTE_PROTOTYPE;
typedef struct _MMPTE_SUBSECTION { ULONG Valid : 1; ULONG Prototype : 1; ULONG Protection : 5; ULONG WhichPool : 1; ULONG Reserved : 4; ULONG SubsectionAddress : 20; } MMPTE_SUBSECTION;
typedef struct _MMPTE_LIST { ULONG Valid : 1; ULONG Prototype : 1; ULONG Protection : 5; ULONG Transition : 1; ULONG OneEntry : 1; ULONG filler : 3; ULONG NextEntry : 20; } MMPTE_LIST;
#define _HARDWARE_PTE_WORKING_SET_BITS 11
typedef struct _MMPTE_HARDWARE { ULONG Valid : 1; ULONG Write : 1; ULONG Owner : 1; ULONG WriteThrough : 1; ULONG CacheDisable : 1; ULONG Accessed : 1; ULONG Dirty : 1; ULONG LargePage : 1; ULONG Global : 1; ULONG CopyOnWrite : 1; ULONG Prototype : 1; ULONG reserved : 1; ULONG PageFrameNumber : 20; } MMPTE_HARDWARE, *PMMPTE_HARDWARE;
typedef struct _MMPTE_LARGEPAGE { ULONG Valid : 1; ULONG Write : 1; ULONG Owner : 1; ULONG WriteThrough : 1; ULONG CacheDisable : 1; ULONG Accessed : 1; ULONG Dirty : 1; ULONG LargePage : 1; ULONG Global : 1; ULONG CopyOnWrite : 1; ULONG Prototype : 1; ULONG reserved : 1; ULONG PageFrameNumber : 20; } MMPTE_LARGEPAGE, *PMMPTE_LARGEPAGE;
#else
/* PAE definitions retained from the existing header. */
typedef struct _MMPTE_SOFTWARE { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG Reserved0 : 3; ULONGLONG UsedPageTableEntries : PTE_PER_PAGE_BITS; ULONGLONG Reserved : 16 - PTE_PER_PAGE_BITS; ULONGLONG PageFileLow : 4; ULONGLONG PageFileHigh : 32; } MMPTE_SOFTWARE;
typedef struct _MMPTE_TRANSITION { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG Reserved0 : 3; ULONGLONG PageFrameNumber : 26 - PAGE_SHIFT; ULONGLONG Reserved : 24; } MMPTE_TRANSITION;
typedef struct _MMPTE_PROTOTYPE { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG ReadOnly : 1; ULONGLONG Reserved : 9; ULONGLONG ProtoAddress : 52; } MMPTE_PROTOTYPE;
typedef struct _MMPTE_SUBSECTION { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG WhichPool : 1; ULONGLONG Reserved : 4; ULONGLONG SubsectionAddress : 52; } MMPTE_SUBSECTION;
typedef struct _MMPTE_LIST { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG OneEntry : 1; ULONGLONG filler : 3; ULONGLONG NextEntry : 52; } MMPTE_LIST;
#define _HARDWARE_PTE_WORKING_SET_BITS 11
typedef struct _MMPTE_HARDWARE { ULONGLONG Valid : 1; ULONGLONG Write : 1; ULONGLONG Owner : 1; ULONGLONG WriteThrough : 1; ULONGLONG CacheDisable : 1; ULONGLONG Accessed : 1; ULONGLONG Dirty : 1; ULONGLONG LargePage : 1; ULONGLONG Global : 1; ULONGLONG CopyOnWrite : 1; ULONGLONG Prototype : 1; ULONGLONG reserved0 : 1; ULONGLONG PageFrameNumber : 26; ULONGLONG reserved1 : 25; ULONGLONG SoftwareWsIndex : _HARDWARE_PTE_WORKING_SET_BITS; } MMPTE_HARDWARE, *PMMPTE_HARDWARE;
typedef MMPTE_HARDWARE MMPTE_LARGEPAGE;
#endif

typedef struct _MMPTE { union { ULONG Long;
#if defined (_X86PAE_)
ULONGLONG LongLong;
#endif
MMPTE_HARDWARE Hard; MMPTE_LARGEPAGE Large; HARDWARE_PTE Flush; MMPTE_PROTOTYPE Proto; MMPTE_SOFTWARE Soft; MMPTE_TRANSITION Trans; MMPTE_SUBSECTION Subsect; MMPTE_LIST List; } u; } MMPTE;
typedef MMPTE *PMMPTE;

#if defined(_WIN64)
#define InterlockedExchangeAddSizeT(a,b) InterlockedExchangeAdd64((PLONGLONG)(a),(b))
#define InterlockedIncrementSizeT(a) InterlockedIncrement64((PLONGLONG)(a))
#define InterlockedDecrementSizeT(a) InterlockedDecrement64((PLONGLONG)(a))
#else
#define InterlockedExchangeAddSizeT(a,b) InterlockedExchangeAdd((PLONG)(a),(LONG)(b))
#define InterlockedIncrementSizeT(a) InterlockedIncrement((PLONG)(a))
#define InterlockedDecrementSizeT(a) InterlockedDecrement((PLONG)(a))
#endif

#if !defined (_X86PAE_)
#define InterlockedCompareExchangePte(_PointerPte, _NewContents, _OldContents) InterlockedCompareExchange ((PLONG)(_PointerPte), (LONG)(_NewContents), (LONG)(_OldContents))
#define InterlockedExchangePte(_PointerPte, _NewContents) InterlockedExchange ((PLONG)(_PointerPte), (LONG)(_NewContents))
#else
#define InterlockedCompareExchangePte(_PointerPte, _NewContents, _OldContents) InterlockedCompareExchange64 ((PLONGLONG)(_PointerPte), (LONGLONG)(_NewContents), (LONGLONG)(_OldContents))
#define InterlockedExchangePte(_PointerPte, _NewContents) InterlockedExchange64 ((PLONGLONG)(_PointerPte), (LONGLONG)(_NewContents))
#endif

FORCEINLINE BOOLEAN MiCompareTbFlushTimeStamp (IN ULONG OldStamp, IN ULONG Mask) { ULONG NewStamp; ULONG Diff; NewStamp = KeReadTbFlushTimeStamp (); Diff = ((NewStamp - OldStamp) & Mask); #if defined(NT_UP) if (Diff != 0) return FALSE; #else if (Diff > 2) return FALSE; if (((OldStamp & 1) == 0) && (Diff >= 2)) return FALSE; #endif return TRUE; }

#define MI_GET_PAGE_FRAME_FROM_PTE(PTE) ((ULONG)((PTE)->u.Hard.PageFrameNumber))
#define MI_GET_PAGE_FRAME_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.PageFrameNumber))
#define MI_GET_PROTECTION_FROM_SOFT_PTE(PTE) ((ULONG)((PTE)->u.Soft.Protection))
#define MI_GET_PROTECTION_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.Protection))

extern PMMPTE MiFirstReservedZeroingPte;
extern PMMPTE MiLargePageHyperPte;
extern PMMPTE MiInitialSystemPageDirectory;

#endif // _MI386_
