#ifndef _WRK_MSVC_COMPAT_H_
#define _WRK_MSVC_COMPAT_H_

/* WRK uses compiler-neutral exception keywords that MSVC C does not expose. */
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
#endif

/* Architecture-independent WRK memory-manager constants missing from the
 * reduced OpenXP architecture headers. */
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

/* The reduced PRCB does not carry the newer color bookkeeping fields. Keep
 * the existing PageColor as the backing value so the WRK inline remains
 * compilable on this x86 layout. */
#if defined(_X86_)
#ifndef SecondaryColorMask
#define SecondaryColorMask PageColor
#endif
#ifndef NodeShiftedColor
#define NodeShiftedColor PageColor
#endif
#endif

/* The reduced thread object has no separate working-set ownership bits.
 * Reuse the passive thread flags as storage for these legacy WRK markers. */
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

/* WRK waits for a TB timestamp update while the timestamp is locked. */
#ifndef KeLoopTbFlushTimeStampUnlocked
static __inline VOID
KeLoopTbFlushTimeStampUnlocked(VOID)
{
    while (KeReadTbFlushTimeStamp() & 1) {
        YieldProcessor();
    }
}
#endif

/* sysload.c uses the WRK routine before its implementation was ported. */
#ifndef WRK_MM_CHECK_SYSTEM_IMAGE_DECLARED
#define WRK_MM_CHECK_SYSTEM_IMAGE_DECLARED
NTSTATUS
MmCheckSystemImage(
    IN HANDLE ImageFileHandle
    );
#endif

#endif /* _WRK_MSVC_COMPAT_H_ */
