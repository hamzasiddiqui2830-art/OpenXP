#ifndef _WRK_MSVС_COMPAT_H_
#define _WRK_MSVС_COMPAT_H_

/* NOTE: This file is generated/maintained by the OpenXP build compatibility layer. */
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
#define MiCompareTbFlushTimeStamp MiCompareTbFlushTimeStamp_X86
#include "../mm/i386/mi386.h"
#undef MiCompareTbFlushTimeStamp

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

#ifndef COMPLUS_PACKAGE_KEYPATH
#define COMPLUS_PACKAGE_KEYPATH L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\.NETFramework"
#endif
#ifndef COMPLUS_PACKAGE_ENABLE64BIT
#define COMPLUS_PACKAGE_ENABLE64BIT L"Enable64Bit"
#endif
#ifndef COMPLUS_PACKAGE_INVALID
#define COMPLUS_PACKAGE_INVALID ((ULONG)-1)
#endif
#ifndef _WRK_COMPLUS_PACKAGE_ROUTINES_DECLARED
#define _WRK_COMPLUS_PACKAGE_ROUTINES_DECLARED
NTSTATUS ExpReadComPlusPackage(VOID);
NTSTATUS ExpUpdateComPlusPackage(IN ULONG ComPlusPackageStatus);
#endif

#if defined(_MSC_VER) && defined(_X86_)
#pragma warning(disable: 4142)
#endif

#include "cmdata.h"

#ifndef _WRK_CM_HASH_TABLE_ENTRIES_DEFINED
#define _WRK_CM_HASH_TABLE_ENTRIES_DEFINED
typedef struct _CM_KEY_HASH_TABLE_ENTRY {
    EX_PUSH_LOCK Lock;
    PKTHREAD Owner;
    PCM_KEY_HASH Entry;
} CM_KEY_HASH_TABLE_ENTRY, *PCM_KEY_HASH_TABLE_ENTRY;
typedef struct _CM_NAME_HASH_TABLE_ENTRY {
    EX_PUSH_LOCK Lock;
    PCM_NAME_HASH Entry;
} CM_NAME_HASH_TABLE_ENTRY, *PCM_NAME_HASH_TABLE_ENTRY;
#endif
#ifndef _WRK_EXP_WATCH_EXPIRATION_DATA_WORK_DECLARED
#define _WRK_EXP_WATCH_EXPIRATION_DATA_WORK_DECLARED
static VOID ExpWatchExpirationDataWork(IN PVOID Context);
#endif
#ifndef ProbeForWriteUlongAligned32
#define ProbeForWriteUlongAligned32(_Address) \
    ProbeForWriteSmallStructure((PVOID)(_Address), sizeof(ULONG), sizeof(ULONG))
#endif
#ifndef _WRK_EX_RUNDOWN_REF_CACHE_AWARE_DEFINED
#define _WRK_EX_RUNDOWN_REF_CACHE_AWARE_DEFINED
typedef struct _EX_RUNDOWN_REF_CACHE_AWARE {
    PEX_RUNDOWN_REF RunRefs;
    PVOID PoolToFree;
    ULONG RunRefSize;
    ULONG Number;
} EX_RUNDOWN_REF_CACHE_AWARE, *PEX_RUNDOWN_REF_CACHE_AWARE;
#endif
#ifndef _WRK_EX_RUNDOWN_CACHE_AWARE_API_DECLARED
#define _WRK_EX_RUNDOWN_CACHE_AWARE_API_DECLARED
PEX_RUNDOWN_REF_CACHE_AWARE ExAllocateCacheAwareRundownProtection(IN POOL_TYPE PoolType, IN ULONG PoolTag);
SIZE_T ExSizeOfRundownProtectionCacheAware(VOID);
VOID ExInitializeRundownProtectionCacheAware(IN PEX_RUNDOWN_REF_CACHE_AWARE RunRefCacheAware, IN SIZE_T Size);
VOID ExFreeCacheAwareRundownProtection(IN PEX_RUNDOWN_REF_CACHE_AWARE RunRefCacheAware);
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
#ifndef MmUnlockPageableImageSection
#define MmUnlockPageableImageSection MmUnlockPagableImageSection
#endif
#ifndef QuantumReset
#define QuantumReset Spare4
#endif
#ifndef Hand
#define Hand Size
#endif
#ifndef OBJ_VALID_PRIVATE_ATTRIBUTES
#define OBJ_VALID_PRIVATE_ATTRIBUTES 0x00010000L
#endif
#ifndef OBJ_ALL_VALID_ATTRIBUTES
#define OBJ_ALL_VALID_ATTRIBUTES (OBJ_VALID_PRIVATE_ATTRIBUTES | OBJ_VALID_ATTRIBUTES)
#endif
#ifndef OBJ_KERNEL_EXCLUSIVE
#define OBJ_KERNEL_EXCLUSIVE 0x00010000L
#endif
#ifndef ProbeAndReadUnicodeStringEx
FORCEINLINE
VOID
ProbeAndReadUnicodeStringEx(OUT PUNICODE_STRING Destination, IN PUNICODE_STRING Source)
{
    ProbeForRead(Source, sizeof(UNICODE_STRING), sizeof(ULONG));
    *Destination = *Source;
}
#endif

#if defined(_MSC_VER) && defined(_X86_)
#ifndef _WRK_MSCV_MI_COMPAT_INCLUDED
#define _WRK_MSCV_MI_COMPAT_INCLUDED
#include "../mm/mi.h"

#ifndef Writable
#define Writable Write
#endif
#ifndef MI_IS_SYSTEM_CACHE_ADDRESS
#define MI_IS_SYSTEM_CACHE_ADDRESS(VA) \
    ((((PVOID)(VA) >= (PVOID)MmSystemCacheStart) && \
      ((PVOID)(VA) <= (PVOID)MmSystemCacheEnd)) || \
     (((PVOID)(VA) >= (PVOID)MiSystemCacheStartExtra) && \
      ((PVOID)(VA) <= (PVOID)MiSystemCacheEndExtra)))
#endif
#ifndef MiIsVirtualAddressOnPdeBoundary
#define MiIsVirtualAddressOnPdeBoundary(VA) \
    (((ULONG_PTR)(VA) & PAGE_DIRECTORY_MASK) == 0)
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
#define MI_MAKE_VALID_PTE(OUTPTE, FRAME, PMASK, PPTE) \
    (OUTPTE).u.Long = ((FRAME << 12) | \
                       (MmProtectToPteMask[PMASK]) | \
                       MiDetermineUserGlobalPteMask((PMMPTE)(PPTE)))
#endif
#ifndef MI_DISABLE_CACHING
#define MI_DISABLE_CACHING(PTE) do { \
    (PTE).u.Hard.CacheDisable = 1; \
    (PTE).u.Hard.WriteThrough = 1; \
} while (0)
#endif
#ifndef MI_SET_PTE_WRITE_COMBINE
#define MI_SET_PTE_WRITE_COMBINE(PTE) do { \
    if (MiWriteCombiningPtes == TRUE) { \
        (PTE).u.Hard.CacheDisable = 0; \
        (PTE).u.Hard.WriteThrough = 1; \
    } else { \
        (PTE).u.Hard.CacheDisable = 1; \
        (PTE).u.Hard.WriteThrough = 0; \
    } \
} while (0)
#endif
#ifndef MiFillMemoryPte
#define MiFillMemoryPte(Destination, Length, Pattern) \
    RtlFillMemoryUlong((Destination), (Length) * sizeof(MMPTE), (Pattern))
#endif
#ifndef MiGetSubsectionAddressForPte
#define MiGetSubsectionAddressForPte(VA) \
    (((ULONG)(VA) < (ULONG)MmSubsectionBase + 128*1024*1024) ? \
        ((((((ULONG)(VA) - (ULONG)MmSubsectionBase) >> 2) & (ULONG)0x0000001E) | \
          ((((ULONG)(VA) - (ULONG)MmSubsectionBase) << 4) & (ULONG)0x7ffff800)) | \
         0x80000000) : \
        (((((ULONG)MmNonPagedPoolEnd - (ULONG)(VA)) >> 2) & (ULONG)0x0000001E) | \
          ((((ULONG)MmNonPagedPoolEnd - (ULONG)(VA)) << 4) & (ULONG)0x7ffff800)))
#endif
#ifndef MiGetSubsectionAddress
#define MiGetSubsectionAddress(lpte) \
    (((lpte)->u.Long & 0x80000000) ? \
        ((PSUBSECTION)((PCHAR)MmSubsectionBase + \
            ((((lpte)->u.Long & 0x7ffff800) >> 4) | \
             (((lpte)->u.Long << 2) & 0x78)))) : \
        ((PSUBSECTION)((PCHAR)MmNonPagedPoolEnd - \
            (((((lpte)->u.Long) >> 11) << 7) | \
             (((lpte)->u.Long << 2) & 0x78)))))
#endif

#ifdef LOCK_WORKING_SET
#undef LOCK_WORKING_SET
#endif
#define WRK_LOCK_WORKING_SET_1(WSINFO) WRK_LOCK_WORKING_SET_2(PsGetCurrentThread(), (WSINFO))
#define WRK_LOCK_WORKING_SET_2(THREAD, WSINFO) do { \
    KeEnterGuardedRegionThread((PKTHREAD)(THREAD)); \
    ASSERT(MI_IS_SESSION_ADDRESS(WSINFO) == FALSE); \
    ASSERT(!MM_ANY_WS_LOCK_HELD(THREAD)); \
    ExAcquirePushLockExclusive(&(WSINFO)->WorkingSetMutex); \
    if ((WSINFO) == &MmSystemCacheWs) { \
        ASSERT(((THREAD)->OwnsSystemWorkingSetExclusive == 0) && ((THREAD)->OwnsSystemWorkingSetShared == 0)); \
        (THREAD)->OwnsSystemWorkingSetExclusive = 1; \
    } else if ((WSINFO)->Flags.SessionSpace == 1) { \
        ASSERT(((THREAD)->OwnsSessionWorkingSetExclusive == 0) && ((THREAD)->OwnsSessionWorkingSetShared == 0)); \
        (THREAD)->OwnsSessionWorkingSetExclusive = 1; \
    } else { \
        ASSERT(((THREAD)->OwnsProcessWorkingSetExclusive == 0) && ((THREAD)->OwnsProcessWorkingSetShared == 0)); \
        (THREAD)->OwnsProcessWorkingSetExclusive = 1; \
    } \
} while (0)
#define WRK_SELECT_LOCK_WORKING_SET(_1, _2, NAME, ...) NAME
#define LOCK_WORKING_SET(...) WRK_SELECT_LOCK_WORKING_SET(__VA_ARGS__, WRK_LOCK_WORKING_SET_2, WRK_LOCK_WORKING_SET_1)(__VA_ARGS__)

#ifdef UNLOCK_WORKING_SET
#undef UNLOCK_WORKING_SET
#endif
#define WRK_UNLOCK_WORKING_SET_1(WSINFO) WRK_UNLOCK_WORKING_SET_2(PsGetCurrentThread(), (WSINFO))
#define WRK_UNLOCK_WORKING_SET_2(THREAD, WSINFO) do { \
    ASSERT(MI_IS_SESSION_ADDRESS(WSINFO) == FALSE); \
    MM_WS_LOCK_ASSERT(WSINFO); \
    if ((WSINFO) == &MmSystemCacheWs) { \
        (THREAD)->OwnsSystemWorkingSetExclusive = 0; \
    } else if ((WSINFO)->Flags.SessionSpace == 1) { \
        (THREAD)->OwnsSessionWorkingSetExclusive = 0; \
    } else { \
        (THREAD)->OwnsProcessWorkingSetExclusive = 0; \
    } \
    ExReleasePushLockExclusive(&(WSINFO)->WorkingSetMutex); \
    KeLeaveGuardedRegionThread((PKTHREAD)(THREAD)); \
} while (0)
#define WRK_SELECT_UNLOCK_WORKING_SET(_1, _2, NAME, ...) NAME
#define UNLOCK_WORKING_SET(...) WRK_SELECT_UNLOCK_WORKING_SET(__VA_ARGS__, WRK_UNLOCK_WORKING_SET_2, WRK_UNLOCK_WORKING_SET_1)(__VA_ARGS__)
#endif
#endif

#endif /* _WRK_MSVС_COMPAT_H_ */
