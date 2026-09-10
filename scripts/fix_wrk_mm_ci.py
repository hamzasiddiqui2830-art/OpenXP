from pathlib import Path
import re

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


def insert_before_final_guard(text, block):
    """Insert a block inside the header guard, immediately before its final #endif."""
    pos = text.rfind("#endif")
    if pos < 0:
        raise RuntimeError("cannot safely patch header: final #endif not found")
    return text[:pos].rstrip() + "\n\n" + block.rstrip() + "\n\n" + text[pos:]


def edit_mi386(text):
    # These are WRK v1.2 x86 MM private definitions.  They belong in the
    # architecture-private header included by mi.h, not in a forced-include
    # compatibility shim.
    definitions = """#ifndef MM_SYSTEM_SPACE_START
#define MM_SYSTEM_SPACE_START ((ULONG_PTR)MmSystemCacheWorkingSetList)
#endif
#ifndef MI_MAXIMUM_PAGEFILE_SIZE
#define MI_MAXIMUM_PAGEFILE_SIZE (((UINT64)1 * 1024 * 1024 - 1) * PAGE_SIZE)
#endif
#ifndef MI_WRITE_INVALID_PTE_WITHOUT_WS
#define MI_WRITE_INVALID_PTE_WITHOUT_WS MI_WRITE_INVALID_PTE
#endif
#ifndef MI_CHECK_PAGE_ALIGNMENT
#define MI_CHECK_PAGE_ALIGNMENT(PAGE,PPTE)
#endif
#ifndef MI_GET_NEXT_COLOR
#define MI_GET_NEXT_COLOR(COLOR) ((COLOR + 1) & MM_COLOR_MASK)
#endif
#ifndef MI_GET_MODIFIED_PAGE_BY_COLOR
#define MI_GET_MODIFIED_PAGE_BY_COLOR(PAGE,COLOR) \\
            PAGE = MmModifiedPageListByColor[COLOR].Flink
#endif
#ifndef MI_GET_MODIFIED_PAGE_ANY_COLOR
#define MI_GET_MODIFIED_PAGE_ANY_COLOR(PAGE,COLOR) \\
            { \\
                if (MmTotalPagesForPagingFile == 0) { \\
                    PAGE = MM_EMPTY_LIST; \\
                } else { \\
                    PAGE = MmModifiedPageListByColor[COLOR].Flink; \\
                } \\
            }
#endif
#ifndef MI_SET_PAGING_FILE_INFO
#define MI_SET_PAGING_FILE_INFO(OUTPTE,PPTE,FILEINFO,OFFSET) \\
       (OUTPTE).u.Long = (PPTE).u.Long; \\
       (OUTPTE).u.Soft.PageFileHigh = (OFFSET); \\
       (OUTPTE).u.Soft.PageFileLow = (FILEINFO);
#endif
#ifndef MI_MAKE_TRANSITION_PROTOPTE_VALID
#define MI_MAKE_TRANSITION_PROTOPTE_VALID(OUTPTE,PPTE) \\
        ASSERT (((PPTE)->u.Hard.Valid == 0) && \\
                ((PPTE)->u.Trans.Prototype == 0) && \\
                ((PPTE)->u.Trans.Transition == 1)); \\
        (OUTPTE).u.Long = (((PPTE)->u.Long & ~0xFFF) | \\
                           (MmProtectToPteMask[(PPTE)->u.Trans.Protection]) | \\
                           (MmPteGlobal.u.Long)); \\
        (OUTPTE).u.Hard.Valid = 1; \\
        (OUTPTE).u.Hard.Accessed = 1
#endif
#ifndef MI_IS_PAGE_TABLE_ADDRESS
#define MI_IS_PAGE_TABLE_ADDRESS(VA) \\
            ((PVOID)(VA) >= (PVOID)PTE_BASE && (PVOID)(VA) <= (PVOID)PTE_TOP)
#endif
#ifndef MI_IS_KERNEL_PAGE_TABLE_ADDRESS
#define MI_IS_KERNEL_PAGE_TABLE_ADDRESS(VA) \\
            ((PVOID)(VA) >= (PVOID)MiGetPteAddress(MmSystemRangeStart) && (PVOID)(VA) <= (PVOID)PTE_TOP)
#endif
#ifndef MI_IS_WRITE_COMBINE_ENABLED
__forceinline
LOGICAL
MI_IS_WRITE_COMBINE_ENABLED (
    IN PMMPTE PointerPte
    )
{
    if (MiWriteCombiningPtes == TRUE) {
        if ((PointerPte->u.Hard.CacheDisable == 0) &&
            (PointerPte->u.Hard.WriteThrough == 1)) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif
#ifndef MI_BARRIER_STAMP_ZEROED_PAGE
#define MI_BARRIER_STAMP_ZEROED_PAGE(PointerTimeStamp)
#endif"""

    # The previous patch put these definitions under an unrelated
    # MM_SESSION_SPACE_DEFAULT guard.  That guard is already satisfied by
    # other WRK headers on this build, so the definitions were silently
    # skipped.  Install the WRK primitives independently and idempotently.
    if "#define MI_CHECK_PAGE_ALIGNMENT" not in text:
        marker = "#define MM_SYSTEM_SPACE_END ((ULONG)0xFFFFFFFF)"
        if marker in text:
            text = text.replace(marker, marker + "\n\n" + definitions, 1)
        else:
            text = insert_before_final_guard(text, definitions)
    else:
        # A partially patched tree may already contain some definitions.
        # Add only the missing WRK primitives immediately before the guard.
        missing = []
        for symbol in (
            "MM_SYSTEM_SPACE_START",
            "MI_MAXIMUM_PAGEFILE_SIZE",
            "MI_WRITE_INVALID_PTE_WITHOUT_WS",
            "MI_GET_NEXT_COLOR",
            "MI_GET_MODIFIED_PAGE_BY_COLOR",
            "MI_GET_MODIFIED_PAGE_ANY_COLOR",
            "MI_SET_PAGING_FILE_INFO",
            "MI_MAKE_TRANSITION_PROTOPTE_VALID",
            "MI_IS_PAGE_TABLE_ADDRESS",
            "MI_IS_KERNEL_PAGE_TABLE_ADDRESS",
            "MI_IS_WRITE_COMBINE_ENABLED",
            "MI_BARRIER_STAMP_ZEROED_PAGE",
        ):
            if symbol not in text:
                missing.append(symbol)
        if missing:
            raise RuntimeError("mi386.h is partially patched; missing WRK definitions: " + ", ".join(missing))

    # Remove the obsolete aggregate guard left by the old compatibility patch.
    text = text.replace("#ifndef MM_SESSION_SPACE_DEFAULT\n" + definitions + "\n#endif", "", 1)

    externs = """extern ULONG_PTR MmBootImageSize;
extern ULONG MiMaximumWorkingSet;
extern ULONG_PTR MiUseMaximumSystemSpace;
extern ULONG_PTR MiUseMaximumSystemSpaceEnd;
extern ULONG MiMaximumSystemCacheSizeExtra;
extern MMPTE MmPteGlobal;
extern PVOID MmHyperSpaceEnd;"""

    # The old implementation appended these after the include guard, which
    # is not safe header hygiene.  Keep them inside the guard and idempotent.
    if "extern ULONG_PTR MmBootImageSize;" not in text:
        text = insert_before_final_guard(text, externs)

    required = (
        "#define MM_SYSTEM_SPACE_START",
        "#define MI_MAXIMUM_PAGEFILE_SIZE",
        "#define MI_WRITE_INVALID_PTE_WITHOUT_WS",
        "#define MI_CHECK_PAGE_ALIGNMENT",
        "#define MI_GET_NEXT_COLOR",
        "#define MI_GET_MODIFIED_PAGE_BY_COLOR",
        "#define MI_GET_MODIFIED_PAGE_ANY_COLOR",
        "#define MI_SET_PAGING_FILE_INFO",
        "#define MI_MAKE_TRANSITION_PROTOPTE_VALID",
        "#define MI_IS_PAGE_TABLE_ADDRESS",
        "#define MI_IS_KERNEL_PAGE_TABLE_ADDRESS",
        "MI_IS_WRITE_COMBINE_ENABLED (",
        "#define MI_BARRIER_STAMP_ZEROED_PAGE",
    )
    for marker in required:
        if marker not in text:
            raise RuntimeError(f"failed to install WRK x86 MM primitive: {marker}")

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
    # MSVC requires every function named by #pragma alloc_text to have
    # been declared before the pragma.  WRK v1.2 declares these entry
    # points before the ALLOC_PRAGMA block; keep that ordering in the
    # SP0 source instead of hiding the problem with an alias or pragma.
    declarations = """NTSTATUS
MmGetExecuteOptions (
    IN PULONG ExecuteOptions
    );

VOID
MmGetImageInformation (
    OUT PSECTION_IMAGE_INFORMATION ImageInformation
    );

"""
    pragma = "#ifdef ALLOC_PRAGMA\n#pragma alloc_text(PAGE, MmGetExecuteOptions)"

    if pragma in text:
        before_pragma = text[:text.find("#ifdef ALLOC_PRAGMA")]
        if "MmGetExecuteOptions (" not in before_pragma:
            text = text.replace("#ifdef ALLOC_PRAGMA", declarations + "#ifdef ALLOC_PRAGMA", 1)

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


def edit_pfsup(text):
    # WRK v1.2 exposes these locked-page-charge helpers with a single PFN
    # argument.  This SP0-derived pfsup.c retained the older diagnostic
    # CallerId arguments, which MSVC correctly rejects against the WRK
    # v1.2-compatible definitions in mi.h.  Drop only the obsolete call-site
    # arguments so the implementation remains the WRK calling convention.
    text = re.sub(
        r"MI_ADD_LOCKED_PAGE_CHARGE\s*\(\s*([^,()]+)\s*,\s*TRUE\s*,\s*\d+\s*\)",
        r"MI_ADD_LOCKED_PAGE_CHARGE(\1)",
        text,
    )
    text = re.sub(
        r"MI_REMOVE_LOCKED_PAGE_CHARGE_AND_DECREF\s*\(\s*([^,()]+)\s*,\s*\d+\s*\)",
        r"MI_REMOVE_LOCKED_PAGE_CHARGE_AND_DECREF(\1)",
        text,
    )
    return text


edit("ntoskrnl/mm/i386/mi386.h", edit_mi386)
edit("ntoskrnl/inc/i386.h", edit_i386)
edit("ntoskrnl/inc/ke.h", edit_ke)
edit("ntoskrnl/inc/mm.h", edit_mmh)
edit("ntoskrnl/mm/mmfault.c", edit_mmfault)
edit("ntoskrnl/mm/mirror.c", edit_mirror)
edit("ntoskrnl/mm/mi.h", edit_mih)
edit("ntoskrnl/mm/miglobal.c", edit_miglobal)
edit("ntoskrnl/mm/mminit.c", edit_mminit)
edit("ntoskrnl/mm/mmpatch.c", edit_mmpatch)
edit("ntoskrnl/mm/pfsup.c", edit_pfsup)
