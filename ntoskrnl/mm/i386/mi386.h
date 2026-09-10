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

#define _MI_PAGING_LEVELS 2

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
#define MM_PAGES_IN_KSEG0 ((MM_KSEG2_BASE - MM_KSEG0_BASE) >> PAGE_SHIFT)
#define CODE_START MM_KSEG0_BASE
#define CODE_END MM_KSEG2_BASE
#define MM_SYSTEM_SPACE_END ((ULONG)0xFFFFFFFF)
#define HYPER_SPACE ((PVOID)0xC0400000)
#define HYPER_SPACE_END ((ULONG)0xC07FFFFF)
#define MM_SYSTEM_VIEW_START ((ULONG)0xA0000000)
#define MM_SYSTEM_VIEW_SIZE (16 * 1024 * 1024)
#define MM_LOWEST_4MB_START ((32 * 1024 * 1024) / PAGE_SIZE)
#define MM_DEFAULT_4MB_START (((1024 * 1024) / PAGE_SIZE) * 4096)
#define MM_HIGHEST_4MB_START (((1024 * 1024) / PAGE_SIZE) * 4096)
#define MM_USER_ADDRESS_RANGE_LIMIT ((ULONG)0xFFFFFFFF)
#define MM_MAXIMUM_ZERO_BITS 21
#define MM_SYSTEM_CACHE_WORKING_SET ((ULONG)0xC0C00000)
#define MM_SYSTEM_CACHE_START ((ULONG)0xC1000000)
#define MM_SYSTEM_CACHE_END ((ULONG)0xE1000000)
#define MM_SYSTEM_CACHE_END_EXTRA ((ULONG)0xC0000000)
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
#define MM_PROTO_PTE_ALIGNMENT ((ULONG)PAGE_SIZE)
#define PAGE_DIRECTORY_MASK ((ULONG)0x003FFFFF)
#define MM_VA_MAPPED_BY_PDE ((ULONG)0x400000)
#define MM_MINIMUM_VA_FOR_LARGE_PAGE MM_VA_MAPPED_BY_PDE
#define LOWEST_IO_ADDRESS 0xA0000
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
#define FIRST_MAPPING_PTE ((ULONG)0xC0400000)
#define NUMBER_OF_MAPPING_PTES 255
#define LAST_MAPPING_PTE ((ULONG)(FIRST_MAPPING_PTE + (NUMBER_OF_MAPPING_PTES * PAGE_SIZE)))
#define COMPRESSION_MAPPING_PTE ((PMMPTE)(LAST_MAPPING_PTE + PAGE_SIZE))
#define NUMBER_OF_ZEROING_PTES 32
#define VAD_BITMAP_SPACE ((PVOID)((ULONG)COMPRESSION_MAPPING_PTE + PAGE_SIZE))
#define WORKING_SET_LIST MmWorkingSetList
#define MM_WORKING_SET_END ((ULONG)0xC07FF000)

#ifndef MM_SECONDARY_COLORS
#define MM_SECONDARY_COLORS (MM_SECONDARY_COLORS_DEFAULT)
#endif

#ifndef MM_PAGED_POOL_START
#define MM_PAGED_POOL_START (MmPagedPoolStart)
#endif
#ifndef MmProtopte_Base
#define MmProtopte_Base ((ULONG)MmPagedPoolStart)
#endif
#ifndef MM_MAX_PAGED_POOL
#define MM_MAX_PAGED_POOL ((ULONG)MM_NONPAGED_POOL_END - (ULONG)MM_PAGED_POOL_START)
#endif
#ifndef MM_MAX_TOTAL_POOL
#define MM_MAX_TOTAL_POOL (((ULONG)MM_NONPAGED_POOL_END) - ((ULONG)MM_PAGED_POOL_START))
#endif

typedef struct _MMCOLOR_TABLES {
    PFN_NUMBER Flink;
    PVOID Blink;
    PFN_NUMBER Count;
} MMCOLOR_TABLES, *PMMCOLOR_TABLES;

extern PMMCOLOR_TABLES MmFreePagesByColor[2];
extern ULONG MmSecondaryColorMask;

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
typedef struct _MMPTE_SOFTWARE { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG Reserved0 : 3; ULONGLONG UsedPageTableEntries : PTE_PER_PAGE_BITS; ULONGLONG Reserved : 16 - PTE_PER_PAGE_BITS; ULONGLONG PageFileLow : 4; ULONGLONG PageFileHigh : 32; } MMPTE_SOFTWARE;
typedef struct _MMPTE_TRANSITION { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG Reserved0 : 3; ULONGLONG PageFrameNumber : 26 - PAGE_SHIFT; ULONGLONG Reserved : 24; } MMPTE_TRANSITION;
typedef struct _MMPTE_PROTOTYPE { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG ReadOnly : 1; ULONGLONG Reserved : 9; ULONGLONG ProtoAddress : 52; } MMPTE_PROTOTYPE;
typedef struct _MMPTE_SUBSECTION { ULONGLONG Valid : 1; ULONGLONG Prototype : 1; ULONGLONG Protection : 5; ULONGLONG Transition : 1; ULONGLONG Reserved0 : 3; ULONGLONG SubsectionAddress : 52; } MMPTE_SUBSECTION;
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

FORCEINLINE
BOOLEAN
MiCompareTbFlushTimeStamp (
    IN ULONG OldStamp,
    IN ULONG Mask
    )
{
    ULONG NewStamp;
    ULONG Diff;

    NewStamp = KeReadTbFlushTimeStamp ();
    Diff = ((NewStamp - OldStamp) & Mask);

#if defined(NT_UP)
    if (Diff != 0) {
        return FALSE;
    }
#else
    if (Diff > 2) {
        return FALSE;
    }
    if (((OldStamp & 1) == 0) && (Diff >= 2)) {
        return FALSE;
    }
#endif
    return TRUE;
}

#if !defined(_X86PAE_)
#ifndef PTE_BASE
#define PTE_BASE 0xC0000000
#endif
#ifndef PTE_TOP
#define PTE_TOP  0xC03FFFFF
#endif
#ifndef PDE_BASE
#define PDE_BASE 0xC0300000
#endif
#ifndef PDE_TOP
#define PDE_TOP  0xC0300FFF
#endif

#define MiGetPteAddress(va) ((PMMPTE)(((((ULONG)(va)) >> 12) << 2) + PTE_BASE))
#define MiGetPdeAddress(va) ((PMMPTE)(((((ULONG)(va)) >> 22) << 2) + PDE_BASE))
#define MiGetPteOffset(va) ((((ULONG)(va)) << 10) >> 22)
#define MiGetPdeOffset(va) (((ULONG)(va)) >> 22)
#define MiGetPteIndex(va) MiGetPteOffset(va)
#define MiGetPdeIndex(va) MiGetPdeOffset(va)
#define MiGetVirtualAddressMappedByPte(PTE) ((PVOID)((((ULONG)(PTE) - PTE_BASE) << 10)))
#define MiGetVirtualAddressMappedByPde(PDE) ((PVOID)((((ULONG)(PDE) - PDE_BASE) << 20)))
#define MiPteToAddress(PTE) MiGetVirtualAddressMappedByPte(PTE)
#define MiPdeToAddress(PDE) MiGetVirtualAddressMappedByPde(PDE)
#define MI_IS_PAGE_TABLE_OR_HYPER_ADDRESS(VA) ((PVOID)(VA) >= (PVOID)PTE_BASE && (PVOID)(VA) <= (PVOID)HYPER_SPACE_END)
#define MI_IS_PAGE_DIRECTORY_ADDRESS(VA) ((PVOID)(VA) >= (PVOID)PDE_BASE && (PVOID)(VA) <= (PVOID)PDE_TOP)
#define MI_CONVERT_PHYSICAL_TO_PFN(Va) ((PFN_NUMBER)(MiGetPdeAddress(Va)->u.Hard.PageFrameNumber) + (MiGetPteOffset((ULONG)Va)))
#endif

#define MI_PDE_MAPS_LARGE_PAGE(PDE) ((PDE)->u.Hard.LargePage == 1)
#define MI_MAKE_PDE_MAP_LARGE_PAGE(PDE) ((PDE)->u.Hard.LargePage = 1)
#define MI_GET_ACCESSED_IN_PTE(PPTE) ((PPTE)->u.Hard.Accessed)
#define MI_SET_ACCESSED_IN_PTE(PPTE,ACCESSED) ((PPTE)->u.Hard.Accessed = (ACCESSED))
#define MI_IS_PTE_DIRTY(PTE) ((PTE).u.Hard.Dirty != 0)
#define MI_SET_PTE_CLEAN(PTE) ((PTE).u.Long &= ~HARDWARE_PTE_DIRTY_MASK)
#define MiIsPteOnPdeBoundary(PTE) ((((ULONG_PTR)(PTE)) & (PAGE_SIZE - 1)) == 0)

#define MM_PTE_VALID_MASK         0x1
#if defined(NT_UP)
#define MM_PTE_WRITE_MASK         0x2
#else
#define MM_PTE_WRITE_MASK         0x800
#endif
#define MM_PTE_OWNER_MASK         0x4
#define MM_PTE_WRITE_THROUGH_MASK 0x8
#define MM_PTE_CACHE_DISABLE_MASK 0x10
#define MM_PTE_ACCESS_MASK        0x20
#if defined(NT_UP)
#define MM_PTE_DIRTY_MASK         0x40
#else
#define MM_PTE_DIRTY_MASK         0x42
#endif
#define MM_PTE_LARGE_PAGE_MASK    0x80
#define MM_PTE_GLOBAL_MASK         0x100
#define MM_PTE_COPY_ON_WRITE_MASK 0x200
#define MM_PTE_PROTOTYPE_MASK     0x400
#define MM_PTE_TRANSITION_MASK    0x800
#define MM_PTE_NOACCESS           0x0
#define MM_PTE_READONLY           0x0
#define MM_PTE_READWRITE          MM_PTE_WRITE_MASK
#define MM_PTE_WRITECOPY          0x200
#define MM_PTE_EXECUTE            0x0
#define MM_PTE_EXECUTE_READ       0x0
#define MM_PTE_EXECUTE_READWRITE  MM_PTE_WRITE_MASK
#define MM_PTE_EXECUTE_WRITECOPY  0x200
#define MM_PTE_NOCACHE            0x010
#define MM_PTE_WRITECOMBINE       0x010
#define MM_PTE_GUARD              0x0
#define MM_PTE_CACHE              0x0
#define MM_PROTECT_FIELD_SHIFT    5
#define MI_MAXIMUM_PTE_WORKING_SET_INDEX 0
#define MM_ZERO_PTE 0
#define MM_ZERO_KERNEL_PTE 0
#define MM_DEMAND_ZERO_WRITE_PTE (MM_READWRITE << MM_PROTECT_FIELD_SHIFT)
#define MM_KERNEL_DEMAND_ZERO_PTE (MM_READWRITE << MM_PROTECT_FIELD_SHIFT)
#define MM_KERNEL_NOACCESS_PTE (MM_NOACCESS << MM_PROTECT_FIELD_SHIFT)
#define MM_STACK_ALIGNMENT 0
#define MM_STACK_OFFSET 0

#define MI_GET_PREVIOUS_COLOR(COLOR) (0)
#define MI_GET_SECONDARY_COLOR(PAGE,PFN) ((PAGE) & MmSecondaryColorMask)
#define MI_GET_COLOR_FROM_SECONDARY(SECONDARY_COLOR) (0)

#define MiZeroMemoryPte(Destination, Length) RtlZeroMemory((Destination), (Length) * sizeof(MMPTE))

#define MI_WRITE_VALID_PTE_NEW_PROTECTION(PointerPte, PteContents) \
    InterlockedExchangePte((PointerPte), (PteContents).u.Long)

#define MI_FLUSH_TB_FOR_INDIVIDUAL_ATTRIBUTE_CHANGE(_PageFrameIndex,_CacheAttribute) \
    MiFlushTbForAttributeChange += 1; \
    MI_FLUSH_ENTIRE_TB (0x21)

#define MI_FLUSH_ENTIRE_TB_FOR_ATTRIBUTE_CHANGE(_CacheAttribute) \
    MiFlushTbForAttributeChange += 1; \
    MI_FLUSH_ENTIRE_TB (0x22)

#define MI_FLUSH_TB_FOR_CACHED_ATTRIBUTE() \
    MiFlushCacheForAttributeChange += 1; \
    MI_FLUSH_ENTIRE_TB (0x23)

#ifndef MM_MAXIMUM_FLUSH_COUNT
#define MM_MAXIMUM_FLUSH_COUNT (FLUSH_MULTIPLE_MAXIMUM + 1)
#endif

#ifndef MM_PAGING_FILE_PAGES_PER_PTE
#define MM_PAGING_FILE_PAGES_PER_PTE 1
#endif

#define MI_GET_PAGE_FRAME_FROM_PTE(PTE) ((ULONG)((PTE)->u.Hard.PageFrameNumber))
#define MI_GET_PAGE_FRAME_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.PageFrameNumber))
#define MI_GET_PROTECTION_FROM_SOFT_PTE(PTE) ((ULONG)((PTE)->u.Soft.Protection))
#define MI_GET_PROTECTION_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.Protection))

#ifndef WORKING_SET_LIST
#define WORKING_SET_LIST MmWorkingSetList
#endif
#ifndef MmWsle
#define MmWsle (MmWorkingSetList->Wsle)
#endif

#ifndef MM_MAXIMUM_WORKING_SET
#define MM_MAXIMUM_WORKING_SET (((ULONG_PTR)(HYPER_SPACE)) >> PAGE_SHIFT)
#endif

extern PMMPTE MiFirstReservedZeroingPte;
extern PMMPTE MiLargePageHyperPte;
extern PMMPTE MiInitialSystemPageDirectory;

#endif /* _MI386_ */
