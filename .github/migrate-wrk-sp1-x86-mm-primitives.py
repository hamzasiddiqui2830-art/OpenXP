from pathlib import Path


def read(p):
    return Path(p).read_text(encoding='utf-8')


def write(p, s):
    Path(p).write_text(s, encoding='utf-8', newline='')

# These are WRK/SP1 architecture primitives missing from the active x86 header.
p = Path('ntoskrnl/mm/i386/mi386.h')
s = read(p)
block = r'''

/* WRK v1.2/SP1 x86 MM primitives. */
#ifndef MI_SET_PTE_IN_WORKING_SET
#define MI_SET_PTE_IN_WORKING_SET(PTE, WORKING_SET_INDEX) ((void)0)
#endif
#ifndef MI_SET_GLOBAL_STATE
#define MI_SET_GLOBAL_STATE(PTE, STATE) ((PTE).u.Hard.Global = (STATE))
#endif
#ifndef MI_DISABLE_LARGE_PTE_CACHING
#define MI_DISABLE_LARGE_PTE_CACHING(PTE) MI_DISABLE_CACHING(PTE)
#endif
#ifndef MI_SET_LARGE_PTE_WRITE_COMBINE
#define MI_SET_LARGE_PTE_WRITE_COMBINE(PTE) MI_SET_PTE_WRITE_COMBINE(PTE)
#endif
#ifndef MI_PREPARE_FOR_NONCACHED
#define MI_PREPARE_FOR_NONCACHED(CacheAttribute) ((void)(CacheAttribute))
#endif
#ifndef MI_NO_FAULT_FOUND
#define MI_NO_FAULT_FOUND(FAULTSTATUS, PPTE, VA, PFNHELD) \\
    if (MI_FAULT_STATUS_INDICATES_WRITE(FAULTSTATUS) && ((PPTE)->u.Hard.Dirty == 0)) { \\
        MiSetDirtyBit((VA), (PPTE), (PFNHELD)); \\
    }
#endif
#ifndef MI_BARRIER_SYNCHRONIZE
#define MI_BARRIER_SYNCHRONIZE(TimeStamp) do { (void)(TimeStamp); KeMemoryBarrier(); } while (0)
#endif
'''
if '/* WRK v1.2/SP1 x86 MM primitives. */' not in s:
    pos = s.rfind('#endif')
    if pos < 0:
        raise SystemExit('mi386.h closing #endif not found')
    s = s[:pos] + block + '\n' + s[pos:]
    write(p, s)

# These are public MM allocation flags used by WRK SP1 iosup.c.
p = Path('ntoskrnl/inc/mm.h')
s = read(p)
flags = r'''

#ifndef MM_DONT_ZERO_ALLOCATION
#define MM_DONT_ZERO_ALLOCATION                  0x00000001
#endif
#ifndef MM_ALLOCATE_FROM_LOCAL_NODE_ONLY
#define MM_ALLOCATE_FROM_LOCAL_NODE_ONLY         0x00000002
#endif
#ifndef MM_ALLOCATE_FULLY_REQUIRED
#define MM_ALLOCATE_FULLY_REQUIRED               0x00000004
#endif
#ifndef MM_ALLOCATE_NO_WAIT
#define MM_ALLOCATE_NO_WAIT                      0x00000008
#endif
#ifndef MM_ALLOCATE_PREFER_CONTIGUOUS
#define MM_ALLOCATE_PREFER_CONTIGUOUS            0x00000010
#endif
#ifndef MM_ALLOCATE_REQUIRE_CONTIGUOUS_CHUNKS
#define MM_ALLOCATE_REQUIRE_CONTIGUOUS_CHUNKS    0x00000020
#endif
'''
if '#define MM_DONT_ZERO_ALLOCATION' not in s:
    pos = s.rfind('#endif')
    s = s[:pos] + flags + '\n' + s[pos:]
    write(p, s)

# The alias was hiding two real WRK exports behind one preprocessor name.
p = Path('ntoskrnl/inc/wrk_msvc_compat.h')
s = read(p)
s = s.replace('#ifndef MmUnlockPageableImageSection\n#define MmUnlockPageableImageSection MmUnlockPagableImageSection\n#endif\n', '')
write(p, s)

# ALLOC_PRAGMA requires declarations before the pragma list.
p = Path('ntoskrnl/mm/iosup.c')
s = read(p)
needle = 'VOID\nMmLockPagableSectionByHandle (\n    __in PVOID ImageSectionHandle\n    );\n'
insert = '''PVOID\nMmLockPageableDataSection (\n    IN PVOID AddressWithinSection\n    );\n\nPMDL\nMmAllocatePagesForMdlEx (\n    IN PHYSICAL_ADDRESS LowAddress,\n    IN PHYSICAL_ADDRESS HighAddress,\n    IN PHYSICAL_ADDRESS SkipBytes,\n    IN SIZE_T TotalBytes,\n    IN MEMORY_CACHING_TYPE CacheType,\n    IN ULONG Flags\n    );\n\n'''
if 'MmLockPageableDataSection (\n    IN PVOID AddressWithinSection' not in s:
    if needle not in s:
        raise SystemExit('iosup declaration anchor not found')
    s = s.replace(needle, insert + needle, 1)
    write(p, s)

print('WRK SP1 x86 MM primitive migration completed.')
