from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def edit(relpath, transform):
    path = ROOT / relpath
    text = path.read_text(encoding="utf-8")
    new_text = transform(text)
    if new_text != text:
        path.write_text(new_text, encoding="utf-8", newline="")
        print(f"changed {relpath}")
    else:
        print(f"unchanged {relpath}")


def edit_mi386(text):
    additions = """

#define MM_SYSTEM_SPACE_START ((ULONG_PTR)MmSystemCacheWorkingSetList)
#define MI_MAXIMUM_PAGEFILE_SIZE (((UINT64)1 * 1024 * 1024 - 1) * PAGE_SIZE)
#define MI_WRITE_INVALID_PTE_WITHOUT_WS MI_WRITE_INVALID_PTE
#define MI_GET_NEXT_COLOR(COLOR) ((COLOR + 1) & MM_COLOR_MASK)
#define MI_GET_MODIFIED_PAGE_BY_COLOR(PAGE,COLOR) \\
            PAGE = MmModifiedPageListByColor[COLOR].Flink
#define MI_GET_MODIFIED_PAGE_ANY_COLOR(PAGE,COLOR) \\
            { \\
                if (MmTotalPagesForPagingFile == 0) { \\
                    PAGE = MM_EMPTY_LIST; \\
                } else { \\
                    PAGE = MmModifiedPageListByColor[COLOR].Flink; \\
                } \\
            }
#define MI_SET_PAGING_FILE_INFO(OUTPTE,PPTE,FILEINFO,OFFSET) \\
       (OUTPTE).u.Long = (PPTE).u.Long; \\
       (OUTPTE).u.Soft.PageFileHigh = (OFFSET); \\
       (OUTPTE).u.Soft.PageFileLow = (FILEINFO);

"""
    if "#define MM_SYSTEM_SPACE_START" not in text:
        marker = "#define MM_SYSTEM_SPACE_END (0xFFFFFFFF)"
        if marker in text:
            text = text.replace(marker, additions + marker, 1)

    if "extern ULONG_PTR MmBootImageSize;" not in text:
        text += """

extern ULONG_PTR MmBootImageSize;
extern ULONG MiMaximumWorkingSet;
extern ULONG_PTR MiUseMaximumSystemSpace;
extern ULONG_PTR MiUseMaximumSystemSpaceEnd;
extern ULONG MiMaximumSystemCacheSizeExtra;
extern MMPTE MmPteGlobal;
extern PVOID MmHyperSpaceEnd;
"""
    return text


def edit_i386(text):
    if "KF_GLOBAL_32BIT_EXECUTE" not in text:
        marker = "#define CR0_PG  0x80000000"
        additions = """#define KF_DTS              0x00020000
#define KF_NOEXECUTE        0x20000000
#define KF_GLOBAL_32BIT_EXECUTE 0x40000000
#define KF_GLOBAL_32BIT_NOEXECUTE 0x80000000

"""
        if marker in text:
            text = text.replace(marker, additions + marker, 1)
    return text


def edit_ke(text):
    if "typedef struct _KEXECUTE_OPTIONS" not in text:
        marker = "typedef struct _KPROCESS {"
        typedef = """typedef struct _KEXECUTE_OPTIONS {
    UCHAR ExecuteDisable : 1;
    UCHAR ExecuteEnable : 1;
    UCHAR DisableThunkEmulation : 1;
    UCHAR Permanent : 1;
    UCHAR ExecuteDispatchEnable : 1;
    UCHAR ImageDispatchEnable : 1;
    UCHAR Spare : 2;
} KEXECUTE_OPTIONS, *PKEXECUTE_OPTIONS;

"""
        if marker in text:
            text = text.replace(marker, typedef + marker, 1)

    old = """    USHORT StackCount;
    SCHAR BasePriority;
    SCHAR ThreadQuantum;
    BOOLEAN AutoAlignment;
    UCHAR State;
    UCHAR ThreadSeed;
    BOOLEAN DisableBoost;
    UCHAR PowerState;
    BOOLEAN DisableQuantum;
    UCHAR IdealNode;
    UCHAR Spare;"""
    new = """    USHORT StackCount;
    SCHAR BasePriority;
    SCHAR QuantumReset;
    UCHAR State;
    UCHAR ThreadSeed;
    UCHAR PowerState;
    UCHAR IdealNode;
    BOOLEAN Visited;
    union {
        KEXECUTE_OPTIONS Flags;
        UCHAR ExecuteOptions;
    };"""
    if old in text:
        text = text.replace(old, new, 1)

    if "KeZeroSinglePage (" not in text:
        marker = "\n#endif"
        pos = text.rfind(marker)
        if pos >= 0:
            text = text[:pos] + """

VOID
KeZeroSinglePage (
    IN PVOID PageBase
    );
""" + text[pos:]
    return text


def edit_mmh(text):
    if "MmGetExecuteOptions (" not in text:
        marker = "#ifdef ALLOC_PRAGMA"
        declarations = """NTSTATUS
MmGetExecuteOptions (
    IN PULONG ExecuteOptions
    );

VOID
MmGetImageInformation (
    OUT PSECTION_IMAGE_INFORMATION ImageInformation
    );

"""
        if marker in text:
            text = text.replace(marker, declarations + marker, 1)
    return text


def edit_mmfault(text):
    block = """NTSTATUS
MmGetExecuteOptions (
    IN PULONG ExecuteOptions
    );

VOID
MmGetImageInformation (
    OUT PSECTION_IMAGE_INFORMATION ImageInformation
    );

"""
    pragma = "#ifdef ALLOC_PRAGMA\n#pragma alloc_text(PAGE, MmGetExecuteOptions)"
    if pragma in text and text.find(block) > text.find(pragma):
        text = text.replace(block, "", 1)
        text = text.replace("#ifdef ALLOC_PRAGMA", block + "#ifdef ALLOC_PRAGMA", 1)
    return text


def edit_mirror(text):
    return (text
            .replace("KeAcquireGuardedMutex (&MmDynamicMemoryMutex);", "MI_LOCK_DYNAMIC_MEMORY_EXCLUSIVE();")
            .replace("KeReleaseGuardedMutex (&MmDynamicMemoryMutex);", "MI_UNLOCK_DYNAMIC_MEMORY_EXCLUSIVE();"))


def edit_mih(text):
    if "#define MM_MIRRORING_ENABLED" not in text:
        marker = "extern EX_PUSH_LOCK MmDynamicMemoryLock;"
        definitions = """

extern ULONG MmMirroring;
#define MM_MIRRORING_ENABLED    0x1
#define MM_MIRRORING_VERIFYING  0x2
extern PRTL_BITMAP MiMirrorBitMap;
extern PRTL_BITMAP MiMirrorBitMap2;
extern LOGICAL MiMirroringActive;
extern ULONG MmTotalPagesForPagingFile;"""
        if marker in text:
            text = text.replace(marker, marker + definitions, 1)
    return text


def edit_miglobal(text):
    if "ULONG MmTotalPagesForPagingFile = 0;" not in text:
        marker = "#if defined(_X86_)\nMMPTE MmPteGlobal = {0x0};\nPVOID MmHyperSpaceEnd;"
        replacement = """#if defined(_X86_)
MMPTE MmPteGlobal = {0x0};
PVOID MmHyperSpaceEnd;
ULONG_PTR MmBootImageSize;
ULONG MiMaximumWorkingSet;
ULONG_PTR MiUseMaximumSystemSpace;
ULONG_PTR MiUseMaximumSystemSpaceEnd;
ULONG MiMaximumSystemCacheSizeExtra;
ULONG MmTotalPagesForPagingFile = 0;"""
        if marker in text:
            text = text.replace(marker, replacement, 1)
    return text


def edit_mminit(text):
    if "MI_INITIALIZE_DYNAMIC_MEMORY_LOCK();" not in text:
        needle = "KeInitializeGuardedMutex (&MmSectionBasedMutex);"
        if needle in text:
            text = text.replace(needle, needle + "\n    MI_INITIALIZE_DYNAMIC_MEMORY_LOCK();", 1)
    return text


def edit_mmpatch(text):
    return text.replace("\n#define NTOS_KERNEL_RUNTIME\n", "\n", 1)


def edit_i386_aliases(text):
    # WRK v1.2 defines these aliases only in the WDM block near the
    # MM_LOWEST_SYSTEM_ADDRESS definitions.  SP0 already has the
    # misspelled MmLockPagableCodeSection alias there, while OpenXP's
    # compatibility edits also added the same aliases to the spinlock
    # block.  Keep the WRK-v1.2 layout and remove the duplicate block.
    duplicate = """// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPageableCodeSection(Address) MmLockPageableDataSection(Address)
#define MmLockPagableCodeSection(Address) MmLockPagableDataSection(Address)
#define MmLockPagableDataSection(Address) MmLockPageableDataSection(Address)
// begin_ntddk begin_ntosp
"""
    spinlock_duplicate = """// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPageableCodeSection(Address) MmLockPageableDataSection(Address)
#define MmLockPagableCodeSection(Address) MmLockPageableDataSection(Address)
#define MmLockPagableDataSection(Address) MmLockPageableDataSection(Address)
// begin_ntddk begin_ntosp

#define ExAcquireSpinLock"""
    if spinlock_duplicate in text:
        text = text.replace(spinlock_duplicate, "// begin_wdm\n\n// begin_ntddk begin_ntosp\n\n#define ExAcquireSpinLock", 1)

    # If the aliases were inserted without the exact surrounding text,
    # remove just the duplicate definitions from the earlier spinlock block.
    early_marker = "#define ExAcquireSpinLock(Lock, OldIrql) KeAcquireSpinLock((Lock), (OldIrql))"
    early_pos = text.find(early_marker)
    if early_pos > 0:
        block_start = text.rfind("// begin_wdm", 0, early_pos)
        if block_start >= 0:
            block = text[block_start:early_pos]
            for line in (
                "#define MmGetProcedureAddress(Address) (Address)\n",
                "#define MmLockPageableCodeSection(Address) MmLockPageableDataSection(Address)\n",
                "#define MmLockPagableCodeSection(Address) MmLockPageableDataSection(Address)\n",
                "#define MmLockPagableDataSection(Address) MmLockPageableDataSection(Address)\n",
            ):
                block = block.replace(line, "")
            text = text[:block_start] + block + text[early_pos:]

    # Ensure the complete WRK-v1.2 alias set exists in the real WDM block.
    aliases = """// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPageableCodeSection(Address) MmLockPageableDataSection(Address)
#define MmLockPagableCodeSection(Address) MmLockPageableDataSection(Address)
#define MmLockPagableDataSection(Address) MmLockPageableDataSection(Address)

// end_ntddk end_wdm"""
    if aliases not in text:
        marker = "// begin_wdm\n\n#define MmGetProcedureAddress(Address) (Address)\n#define MmLockPagableCodeSection(Address) MmLockPagableDataSection(Address)\n\n// end_ntddk end_wdm"
        if marker in text:
            text = text.replace(marker, aliases, 1)
    return text


edit("ntoskrnl/mm/i386/mi386.h", edit_mi386)
edit("ntoskrnl/inc/i386.h", edit_i386)
edit("ntoskrnl/inc/i386.h", edit_i386_aliases)
edit("ntoskrnl/inc/ke.h", edit_ke)
edit("ntoskrnl/inc/mm.h", edit_mmh)
edit("ntoskrnl/mm/mmfault.c", edit_mmfault)
edit("ntoskrnl/mm/mirror.c", edit_mirror)
edit("ntoskrnl/mm/mi.h", edit_mih)
edit("ntoskrnl/mm/miglobal.c", edit_miglobal)
edit("ntoskrnl/mm/mminit.c", edit_mminit)
edit("ntoskrnl/mm/mmpatch.c", edit_mmpatch)
