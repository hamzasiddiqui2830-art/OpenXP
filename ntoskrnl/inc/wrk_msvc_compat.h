#ifndef _WRK_MSVC_COMPAT_H_
#define _WRK_MSVC_COMPAT_H_

#include "ntos.h"

/*
 * The WRK headers are shared between consumers and the kernel itself.
 * During a kernel build these APIs are being defined, not imported from
 * another DLL. Clear the legacy import annotations before private headers
 * such as ex.h are parsed by the source files.
 */
#ifdef NTHALAPI
#undef NTHALAPI
#endif
#define NTHALAPI
#ifdef NTKERNELAPI
#undef NTKERNELAPI
#endif
#define NTKERNELAPI
#ifdef NTSYSAPI
#undef NTSYSAPI
#endif
#define NTSYSAPI

#if defined(_MSC_VER) && defined(_X86_)
#define MiCompareTbFlushTimeStamp MiCompareTbFlushTimeStamp_X86
#include "../mm/i386/mi386.h"
#undef MiCompareTbFlushTimeStamp
#endif

#if defined(_MSC_VER) && !defined(__cplusplus)
#ifndef try
#define try __try
#endif
#ifndef except
#define except __except
#endif
#ifndef finally
#define finally __finally
#endif
/* WRK uses `leave;` to exit a try body while still executing finally. */
#ifndef leave
#define leave __leave
#endif
#endif

#ifndef MAX_PAGE_FILES
#define MAX_PAGE_FILES 16
#endif
#ifndef MM_MINIMUM_VA_FOR_LARGE_PAGE
#define MM_MINIMUM_VA_FOR_LARGE_PAGE 0x400000UL
#endif
#ifndef MM_PTE_EXECUTE
#define MM_PTE_EXECUTE 0x0UL
#endif
#ifndef MM_PTE_EXECUTE_READ
#define MM_PTE_EXECUTE_READ 0x0UL
#endif
#ifndef MM_PTE_EXECUTE_READWRITE
#define MM_PTE_EXECUTE_READWRITE 0x800UL
#endif
#ifndef MM_PTE_EXECUTE_WRITECOPY
#define MM_PTE_EXECUTE_WRITECOPY 0x200UL
#endif
#ifndef GET_PAGING_FILE_NUMBER
#define GET_PAGING_FILE_NUMBER(_PteContents) ((_PteContents).u.Soft.PageFileLow)
#endif
#ifndef GET_PAGING_FILE_OFFSET
#define GET_PAGING_FILE_OFFSET(_PteContents) ((_PteContents).u.Soft.PageFileHigh)
#endif
#ifndef MI_WRITE_ZERO_PTE
#define MI_WRITE_ZERO_PTE(_PointerPte) RtlZeroMemory((_PointerPte), sizeof(MMPTE))
#endif
#ifndef MI_WRITE_VALID_PTE
#define MI_WRITE_VALID_PTE(_PointerPte, _TempPte) (*(_PointerPte) = (_TempPte))
#endif
#ifndef MI_WRITE_INVALID_PTE
#define MI_WRITE_INVALID_PTE(_PointerPte, _TempPte) (*(_PointerPte) = (_TempPte))
#endif
#ifndef MI_SET_PFN_DELETED
#define MI_SET_PFN_DELETED(_Pfn) ((_Pfn)->PteAddress = (PMMPTE)((ULONG_PTR)(_Pfn)->PteAddress | 1))
#endif
#ifndef MI_MAKE_VALID_PTE_TRANSITION
#define MI_MAKE_VALID_PTE_TRANSITION(_Pte, _Protect) do { \
    (_Pte).u.Soft.Transition = 1; \
    (_Pte).u.Soft.Valid = 0; \
    (_Pte).u.Soft.Prototype = 0; \
    (_Pte).u.Soft.Protection = (_Protect); \
} while (0)
#endif
#ifndef MI_CAPTURE_DIRTY_BIT_TO_PFN
#define MI_CAPTURE_DIRTY_BIT_TO_PFN(_PointerPte, _Pfn) ((void)0)
#endif
#ifndef MI_IS_PHYSICAL_ADDRESS
#define MI_IS_PHYSICAL_ADDRESS(_Address) (FALSE)
#endif

#if defined(_X86_)
#ifndef SecondaryColorMask
#define SecondaryColorMask PageColor
#endif
#ifndef NodeShiftedColor
#define NodeShiftedColor PageColor
#endif
#endif

#ifndef OwnsSystemWorkingSetExclusive
#define OwnsSystemWorkingSetExclusive MemoryMaker
#endif
#ifndef OwnsSystemWorkingSetShared
#define OwnsSystemWorkingSetShared KeyedEventInUse
#endif
#ifndef OwnsSessionWorkingSetExclusive
#define OwnsSessionWorkingSetExclusive ActiveExWorker
#endif
#ifndef OwnsSessionWorkingSetShared
#define OwnsSessionWorkingSetShared ExWorkerCanWaitUser
#endif
#ifndef OwnsProcessWorkingSetExclusive
#define OwnsProcessWorkingSetExclusive MemoryMaker
#endif
#ifndef OwnsProcessWorkingSetShared
#define OwnsProcessWorkingSetShared KeyedEventInUse
#endif

#ifndef KeLoopTbFlushTimeStampUnlocked
static __inline VOID
KeLoopTbFlushTimeStampUnlocked(VOID)
{
    while (KeReadTbFlushTimeStamp() & 1) {
        YieldProcessor();
    }
}
#endif

#ifndef WRK_MM_CHECK_SYSTEM_IMAGE_DECLARED
#define WRK_MM_CHECK_SYSTEM_IMAGE_DECLARED
NTSTATUS MmCheckSystemImage(IN HANDLE ImageFileHandle);
#endif
#ifndef WRK_MM_LOCK_PAGEABLE_SECTION_BY_HANDLE_DECLARED
#define WRK_MM_LOCK_PAGEABLE_SECTION_BY_HANDLE_DECLARED
VOID MmLockPageableSectionByHandle(IN PVOID ImageSectionHandle);
#endif

/*
 * The reconstructed kernel headers retain the WRK field layouts but some
 * active sources use the older member spellings. On x86 these names refer
 * to the same byte-sized storage: KTHREAD::Spare4 is the quantum-reset byte,
 * while DISPATCHER_HEADER::Size is the timer-table hand byte.
 */
#ifndef QuantumReset
#define QuantumReset Spare4
#endif
#ifndef Hand
#define Hand Size
#endif

#endif /* _WRK_MSVC_COMPAT_H_ */
