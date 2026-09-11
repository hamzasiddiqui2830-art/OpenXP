#ifndef _WRK_MSVC_COMPAT_H_
#define _WRK_MSVC_COMPAT_H_

/*
 * The WRK build environment historically supplied these symbols while
 * ntos.h also owns them as header guards. Clear the command-line copies
 * before the translation unit's normal WRK header inclusion order runs.
 */
#ifdef _NTOS_
#undef _NTOS_
#endif
#ifdef _NTIFS_
#undef _NTIFS_
#endif
#ifdef _NTDDK_
#undef _NTDDK_
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4324) /* structure was padded due to alignment specifier */
#endif

#include "ntos.h"

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

#ifdef PROCESSOR_FEATURE_MAX
#undef PROCESSOR_FEATURE_MAX
#endif
#define PROCESSOR_FEATURE_MAX 64UL

#if defined(_MSC_VER) && defined(_X86_)

/*
 * These are compiler-facing compatibility definitions only.  Do not include
 * mi386.h here: that header depends on the memory-manager private type layer
 * (including PFN_NUMBER) and must be reached through the WRK source's normal
 * include order.
 */
#ifndef MI_FAULT_STATUS_INDICATES_EXECUTION
#define MI_FAULT_STATUS_INDICATES_EXECUTION(_FaultStatus) 0
#endif
#ifndef MI_FAULT_STATUS_INDICATES_WRITE
#define MI_FAULT_STATUS_INDICATES_WRITE(_FaultStatus) (_FaultStatus & 0x1)
#endif
#ifndef MI_CLEAR_FAULT_STATUS
#define MI_CLEAR_FAULT_STATUS(_FaultStatus) (_FaultStatus = 0)
#endif
#ifndef MI_IS_PTE_EXECUTABLE
#define MI_IS_PTE_EXECUTABLE(_TempPte) (1)
#endif
#ifndef MI_NO_FAULT_FOUND
#define MI_NO_FAULT_FOUND(FAULTSTATUS, PPTE, VA, PFNHELD) \
    if ((MI_FAULT_STATUS_INDICATES_WRITE(FAULTSTATUS)) && ((PPTE)->u.Hard.Dirty == 0)) { \
        MiSetDirtyBit ((VA), (PPTE), (PFNHELD)); \
    }
#endif
#ifndef MI_SET_PTE_IN_WORKING_SET
#define MI_SET_PTE_IN_WORKING_SET(PTE, WSINDEX)
#endif
#ifndef MI_SET_GLOBAL_STATE
#define MI_SET_GLOBAL_STATE(PTE, STATE) if (STATE) { (PTE).u.Hard.Global = 1; } else { (PTE).u.Hard.Global = 0; }
#endif
#ifndef MI_BARRIER_SYNCHRONIZE
#define MI_BARRIER_SYNCHRONIZE(TimeStamp)
#endif
#ifndef MI_MAKE_VALID_USER_PTE
#define MI_MAKE_VALID_USER_PTE(OUTPTE,FRAME,PMASK,PPTE) \
       ASSERT (PPTE <= MiHighestUserPte); \
       (OUTPTE).u.Long = 0; \
       (OUTPTE).u.Hard.Valid = 1; \
       (OUTPTE).u.Hard.Accessed = 1; \
       (OUTPTE).u.Hard.PageFrameNumber = FRAME; \
       (OUTPTE).u.Long |= MmProtectToPteMask[PMASK]; \
       (OUTPTE).u.Hard.Owner = MI_PTE_OWNER_USER;
#endif
#ifndef MI_MAKE_VALID_KERNEL_PTE
#define MI_MAKE_VALID_KERNEL_PTE(OUTPTE,FRAME,PMASK,PPTE) \
       ASSERT (PPTE > MiHighestUserPte); \
       ASSERT (!MI_IS_SESSION_PTE (PPTE)); \
       ASSERT ((PPTE < (PMMPTE)PDE_BASE) || (PPTE > (PMMPTE)PDE_TOP)); \
       (OUTPTE).u.Long = 0; \
       (OUTPTE).u.Hard.Valid = 1; \
       (OUTPTE).u.Hard.Accessed = 1; \
       (OUTPTE).u.Hard.PageFrameNumber = FRAME; \
       (OUTPTE).u.Long |= MmPteGlobal.u.Long; \
       (OUTPTE).u.Long |= MmProtectToPteMask[PMASK];
#endif
#ifndef MI_WRITE_VALID_PTE_NEW_PAGE
#define MI_WRITE_VALID_PTE_NEW_PAGE(_PointerPte, _TempPte) \
       InterlockedExchangePte ((_PointerPte), (_TempPte).u.Long)
#endif
#ifndef MI_IS_PROCESS_SPACE_ADDRESS
#define MI_IS_PROCESS_SPACE_ADDRESS(VA) \
       (((PVOID)(VA) <= (PVOID)MM_HIGHEST_USER_ADDRESS) || \
        ((PVOID)(VA) >= (PVOID)PTE_BASE && (PVOID)(VA) <= (PVOID)MmHyperSpaceEnd))
#endif
#ifndef MI_PREPARE_FOR_NONCACHED
#define MI_PREPARE_FOR_NONCACHED(_CacheAttribute) \
       if (_CacheAttribute != MiCached) { \
           MI_FLUSH_ENTIRE_TB (0x20); \
           KeInvalidateAllCaches (); \
       }
#endif
#ifndef MI_DISABLE_LARGE_PTE_CACHING
#define MI_DISABLE_LARGE_PTE_CACHING(PTE) MI_DISABLE_CACHING(PTE)
#endif
#ifndef MI_SET_LARGE_PTE_WRITE_COMBINE
#define MI_SET_LARGE_PTE_WRITE_COMBINE(PTE) MI_SET_PTE_WRITE_COMBINE(PTE)
#endif

extern MMPTE MmPteGlobal;
extern PVOID MmHyperSpaceEnd;

#ifndef HARDWARE_PTE_DIRTY_MASK
#define HARDWARE_PTE_DIRTY_MASK MM_PTE_DIRTY_MASK
#endif
#ifndef MiPteToProto
#define MiPteToProto(lpte) \
    (PMMPTE)((PMMPTE)(((((lpte)->u.Long) >> 11) << 9) + \
    (((((lpte)->u.Long)) << 24) >> 23) + MmProtopte_Base))
#endif
#ifndef MI_DETERMINE_OWNER
#define MI_DETERMINE_OWNER(PPTE) ((PPTE)->u.Hard.Owner)
#endif
#ifndef MI_MAKE_TRANSITION_PTE
#define MI_MAKE_TRANSITION_PTE(OUTPTE, PAGEFRAME, PROTECT, PPTE) do { \
    (OUTPTE).u.Trans.PageFrameNumber = (PAGEFRAME); \
    (OUTPTE).u.Trans.Prototype = 0; \
    (OUTPTE).u.Trans.Transition = 1; \
    (OUTPTE).u.Trans.Protection = (PROTECT); \
    (OUTPTE).u.Trans.Owner = MI_DETERMINE_OWNER(PPTE); \
} while (0)
#endif

#ifndef Writable
#define Writable Write
#endif
#ifndef MI_IS_SYSTEM_CACHE_ADDRESS
#define MI_IS_SYSTEM_CACHE_ADDRESS(VA) ((((PVOID)(VA) >= (PVOID)MmSystemCacheStart) && ((PVOID)(VA) <= (PVOID)MmSystemCacheEnd)) || (((PVOID)(VA) >= (PVOID)MiSystemCacheStartExtra) && ((PVOID)(VA) <= (PVOID)MiSystemCacheEndExtra)))
#endif
#ifndef MiIsVirtualAddressOnPdeBoundary
#define MiIsVirtualAddressOnPdeBoundary(VA) (((ULONG_PTR)(VA) & PAGE_DIRECTORY_MASK) == 0)
#endif
#ifndef IS_PTE_NOT_DEMAND_ZERO
#define IS_PTE_NOT_DEMAND_ZERO(PTE) ((PTE).u.Long & (ULONG)0xFFFFFC01)
#endif
#ifndef MI_IS_PFN_DELETED
#define MI_IS_PFN_DELETED(PPFN) ((ULONG_PTR)(PPFN)->PteAddress & 0x1)
#endif
#ifndef MI_SET_PTE_DIRTY
#define MI_SET_PTE_DIRTY(PTE) ((PTE).u.Long |= HARDWARE_PTE_DIRTY_MASK)
#endif
#ifndef MI_MAKE_VALID_PTE
#define MI_MAKE_VALID_PTE(OUTPTE, FRAME, PMASK, PPTE) (OUTPTE).u.Long = ((FRAME << 12) | (MmProtectToPteMask[PMASK]) | MiDetermineUserGlobalPteMask((PMMPTE)(PPTE)))
#endif
#ifndef MI_DISABLE_CACHING
#define MI_DISABLE_CACHING(PTE) do { (PTE).u.Hard.CacheDisable = 1; (PTE).u.Hard.WriteThrough = 1; } while (0)
#endif
#ifndef MI_SET_PTE_WRITE_COMBINE
#define MI_SET_PTE_WRITE_COMBINE(PTE) do { if (MiWriteCombiningPtes == TRUE) { (PTE).u.Hard.CacheDisable = 0; (PTE).u.Hard.WriteThrough = 1; } else { (PTE).u.Hard.CacheDisable = 1; (PTE).u.Hard.WriteThrough = 0; } } while (0)
#endif
#ifndef MiFillMemoryPte
#define MiFillMemoryPte(Destination, Length, Pattern) RtlFillMemoryUlong((Destination), (Length) * sizeof(MMPTE), (Pattern))
#endif
#ifndef MiGetSubsectionAddressForPte
#define MiGetSubsectionAddressForPte(VA) (((ULONG)(VA) < (ULONG)MmSubsectionBase + 128*1024*1024) ? ((((((ULONG)(VA) - (ULONG)MmSubsectionBase) >> 2) & (ULONG)0x0000001E) | ((((ULONG)(VA) - (ULONG)MmSubsectionBase) << 4) & (ULONG)0x7ffff800)) | 0x80000000) : (((((ULONG)(MmNonPagedPoolEnd) - (ULONG)(VA)) >> 2) & (ULONG)0x0000001E) | ((((ULONG)(VA) - (ULONG)MmSubsectionBase) << 4) & (ULONG)0x7ffff800)))
#endif
#ifndef MiGetSubsectionAddress
#define MiGetSubsectionAddress(lpte) (((lpte)->u.Long & 0x80000000) ? ((PSUBSECTION)((PCHAR)MmSubsectionBase + ((((lpte)->u.Long & 0x7ffff800) >> 4) | (((lpte)->u.Long << 2) & 0x78)))) : ((PSUBSECTION)((PCHAR)MmNonPagedPoolEnd - (((((lpte)->u.Long) >> 11) << 7) | (((lpte)->u.Long << 2) & 0x78)))))
#endif
extern PVOID MiSystemCacheStartExtra;
extern PVOID MiSystemCacheEndExtra;
extern BOOLEAN MiWriteCombiningPtes;
extern ULONG MiDetermineUserGlobalPteMask(IN PMMPTE PointerPte);
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
#ifndef leave
#define leave __leave
#endif
#endif

NTSTATUS MmCheckSystemImage(IN HANDLE ImageFileHandle, IN LOGICAL PurgeSection);

#endif /* _WRK_MSVC_COMPAT_H_ */