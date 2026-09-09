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

typedef struct _MMPTE_SOFTWARE {
    ULONG Valid : 1;
    ULONG PageFileLow : 4;
    ULONG Protection : 5;
    ULONG Prototype : 1;
    ULONG Transition : 1;
    ULONG PageFileHigh : 20;
} MMPTE_SOFTWARE;

typedef struct _MMPTE_TRANSITION {
    ULONG Valid : 1;
    ULONG Prototype : 1;
    ULONG Protection : 5;
    ULONG Transition : 1;
    ULONG Reserved0 : 3;
    ULONG PageFrameNumber : 20 - PAGE_SHIFT;
    ULONG Reserved : 11;
} MMPTE_TRANSITION;

typedef struct _MMPTE_PROTOTYPE {
    ULONG Valid : 1;
    ULONG Prototype : 1;
    ULONG ReadOnly : 1;
    ULONG Reserved : 9;
    ULONG ProtoAddress : 20;
} MMPTE_PROTOTYPE;

typedef struct _MMPTE_SUBSECTION {
    ULONG Valid : 1;
    ULONG Prototype : 1;
    ULONG Protection : 5;
    ULONG WhichPool : 1;
    ULONG Reserved : 4;
    ULONG SubsectionAddress : 20;
} MMPTE_SUBSECTION;

typedef struct _MMPTE_LIST {
    ULONG Valid : 1;
    ULONG Prototype : 1;
    ULONG Protection : 5;
    ULONG Transition : 1;
    ULONG OneEntry : 1;
    ULONG filler : 3;
    ULONG NextEntry : 20;
} MMPTE_LIST;

#define _HARDWARE_PTE_WORKING_SET_BITS  11

typedef struct _MMPTE_HARDWARE {
    ULONG Valid : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG LargePage : 1;
    ULONG Global : 1;
    ULONG CopyOnWrite : 1;
    ULONG Prototype : 1;
    ULONG reserved : 1;
    ULONG PageFrameNumber : 20;
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

typedef struct _MMPTE_LARGEPAGE {
    ULONG Valid : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG LargePage : 1;
    ULONG Global : 1;
    ULONG CopyOnWrite : 1;
    ULONG Prototype : 1;
    ULONG reserved : 1;
    ULONG PageFrameNumber : 20;
} MMPTE_LARGEPAGE, *PMMPTE_LARGEPAGE;

#else

typedef struct _MMPTE_SOFTWARE {
    ULONGLONG Valid : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG Protection : 5;
    ULONGLONG Transition : 1;
    ULONGLONG Reserved0 : 3;
    ULONGLONG UsedPageTableEntries : PTE_PER_PAGE_BITS;
    ULONGLONG Reserved : 16 - PTE_PER_PAGE_BITS;
    ULONGLONG PageFileLow : 4;
    ULONGLONG PageFileHigh : 32;
} MMPTE_SOFTWARE;

typedef struct _MMPTE_TRANSITION {
    ULONGLONG Valid : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG Protection : 5;
    ULONGLONG Transition : 1;
    ULONGLONG Reserved0 : 3;
    ULONGLONG PageFrameNumber : 26 - PAGE_SHIFT;
    ULONGLONG Reserved : 24;
} MMPTE_TRANSITION;

typedef struct _MMPTE_PROTOTYPE {
    ULONGLONG Valid : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG ReadOnly : 1;
    ULONGLONG Reserved : 9;
    ULONGLONG ProtoAddress : 52;
} MMPTE_PROTOTYPE;

typedef struct _MMPTE_SUBSECTION {
    ULONGLONG Valid : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG Protection : 5;
    ULONGLONG WhichPool : 1;
    ULONGLONG Reserved : 4;
    ULONGLONG SubsectionAddress : 52;
} MMPTE_SUBSECTION;

typedef struct _MMPTE_LIST {
    ULONGLONG Valid : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG Protection : 5;
    ULONGLONG Transition : 1;
    ULONGLONG OneEntry : 1;
    ULONGLONG filler : 3;
    ULONGLONG NextEntry : 52;
} MMPTE_LIST;

#define _HARDWARE_PTE_WORKING_SET_BITS  11

typedef struct _MMPTE_HARDWARE {
    ULONGLONG Valid : 1;
    ULONGLONG Write : 1;
    ULONGLONG Owner : 1;
    ULONGLONG WriteThrough : 1;
    ULONGLONG CacheDisable : 1;
    ULONGLONG Accessed : 1;
    ULONGLONG Dirty : 1;
    ULONGLONG LargePage : 1;
    ULONGLONG Global : 1;
    ULONGLONG CopyOnWrite : 1;
    ULONGLONG Prototype : 1;
    ULONGLONG reserved0 : 1;
    ULONGLONG PageFrameNumber : 26;
    ULONGLONG reserved1 : 25;
    ULONGLONG SoftwareWsIndex : _HARDWARE_PTE_WORKING_SET_BITS;
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

typedef MMPTE_HARDWARE MMPTE_LARGEPAGE;

#endif

typedef struct _MMPTE {
    union {
        ULONG Long;
#if defined (_X86PAE_)
        ULONGLONG LongLong;
#endif
        MMPTE_HARDWARE Hard;
        MMPTE_LARGEPAGE Large;
        HARDWARE_PTE Flush;
        MMPTE_PROTOTYPE Proto;
        MMPTE_SOFTWARE Soft;
        MMPTE_TRANSITION Trans;
        MMPTE_SUBSECTION Subsect;
        MMPTE_LIST List;
    } u;
} MMPTE;

typedef MMPTE *PMMPTE;

#if !defined (_X86PAE_)
#define InterlockedCompareExchangePte(_PointerPte, _NewContents, _OldContents) \
        InterlockedCompareExchange ((PLONG)(_PointerPte), (LONG)(_NewContents), (LONG)(_OldContents))
#define InterlockedExchangePte(_PointerPte, _NewContents) \
        InterlockedExchange ((PLONG)(_PointerPte), (LONG)(_NewContents))
#else
#define InterlockedCompareExchangePte(_PointerPte, _NewContents, _OldContents) \
        InterlockedCompareExchange64 ((PLONGLONG)(_PointerPte), (LONGLONG)(_NewContents), (LONGLONG)(_OldContents))
#define InterlockedExchangePte(_PointerPte, _NewContents) \
        InterlockedExchange64 ((PLONGLONG)(_PointerPte), (LONGLONG)(_NewContents))
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

#define MI_GET_PAGE_FRAME_FROM_PTE(PTE) ((ULONG)((PTE)->u.Hard.PageFrameNumber))
#define MI_GET_PAGE_FRAME_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.PageFrameNumber))
#define MI_GET_PROTECTION_FROM_SOFT_PTE(PTE) ((ULONG)((PTE)->u.Soft.Protection))
#define MI_GET_PROTECTION_FROM_TRANSITION_PTE(PTE) ((ULONG)((PTE)->u.Trans.Protection))

extern PMMPTE MiFirstReservedZeroingPte;
extern PMMPTE MiLargePageHyperPte;
extern PMMPTE MiInitialSystemPageDirectory;

#endif // _MI386_
