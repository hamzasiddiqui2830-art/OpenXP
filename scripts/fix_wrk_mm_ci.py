from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def edit(relpath, transform):
    # This compatibility script is deliberately restricted to the active
    # kernel tree. ntos-old is reference material and must never be changed.
    if not relpath.startswith("ntoskrnl/"):
        raise RuntimeError(f"refusing to edit non-active source: {relpath}")
    path = ROOT / relpath
    text = path.read_text(encoding="utf-8")
    new_text = transform(text)
    if new_text != text:
        path.write_text(new_text, encoding="utf-8", newline="")
        print(f"changed {relpath}")
    else:
        print(f"unchanged {relpath}")


def edit_mi386(text):
    # The previous compatibility patch put the WRK definitions under an
    # unrelated MM_SESSION_SPACE_DEFAULT guard. On this tree that symbol is
    # already defined, so the block is skipped. Move the definitions into the
    # real _MI386_ guard and make every primitive independently guarded.
    bad = re.search(
        r"\n#ifndef MM_SESSION_SPACE_DEFAULT\n"
        r"#define MM_SESSION_SPACE_DEFAULT.*?\n#endif\n\n"
        r"extern ULONG_PTR MmBootImageSize;.*?"
        r"extern PVOID MmHyperSpaceEnd;\s*\Z",
        text,
        re.S,
    )
    if bad:
        text = text[:bad.start()] + "\n"

    marker = "#endif /* _MI386_ */"
    if marker not in text:
        raise RuntimeError("_MI386_ closing guard not found")

    definitions = r'''#ifndef MM_SYSTEM_SPACE_START
#define MM_SYSTEM_SPACE_START ((ULONG_PTR)MmSystemCacheWorkingSetList)
#endif
#ifndef MI_MAXIMUM_PAGEFILE_SIZE
#define MI_MAXIMUM_PAGEFILE_SIZE (((UINT64)1 * 1024 * 1024 - 1) * PAGE_SIZE)
#endif
#ifndef MI_WRITE_INVALID_PTE_WITHOUT_WS
#define MI_WRITE_INVALID_PTE_WITHOUT_WS MI_WRITE_INVALID_PTE
#endif
#ifndef MI_CHECK_PAGE_ALIGNMENT
#define MI_CHECK_PAGE_ALIGNMENT(PAGE,PPTE) ((void)0)
#endif
#ifndef MI_GET_NEXT_COLOR
#define MI_GET_NEXT_COLOR(COLOR) ((COLOR + 1) & MM_COLOR_MASK)
#endif
#ifndef MI_GET_MODIFIED_PAGE_BY_COLOR
#define MI_GET_MODIFIED_PAGE_BY_COLOR(PAGE,COLOR) \
    PAGE = MmModifiedPageListByColor[COLOR].Flink
#endif
#ifndef MI_GET_MODIFIED_PAGE_ANY_COLOR
#define MI_GET_MODIFIED_PAGE_ANY_COLOR(PAGE,COLOR) \
    { \
        if (MmTotalPagesForPagingFile == 0) { \
            PAGE = MM_EMPTY_LIST; \
        } else { \
            PAGE = MmModifiedPageListByColor[COLOR].Flink; \
        } \
    }
#endif
#ifndef MI_SET_PAGING_FILE_INFO
#define MI_SET_PAGING_FILE_INFO(OUTPTE,PPTE,FILEINFO,OFFSET) \
    (OUTPTE).u.Long = (PPTE).u.Long; \
    (OUTPTE).u.Soft.PageFileHigh = (OFFSET); \
    (OUTPTE).u.Soft.PageFileLow = (FILEINFO)
#endif
#ifndef MI_IS_PAGE_TABLE_ADDRESS
#define MI_IS_PAGE_TABLE_ADDRESS(VA) \
    ((PVOID)(VA) >= (PVOID)PTE_BASE && (PVOID)(VA) <= (PVOID)PTE_TOP)
#endif
#ifndef MI_IS_KERNEL_PAGE_TABLE_ADDRESS
#define MI_IS_KERNEL_PAGE_TABLE_ADDRESS(VA) \
    ((PVOID)(VA) >= (PVOID)MiGetPteAddress(MmSystemRangeStart) && \
     (PVOID)(VA) <= (PVOID)PTE_TOP)
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
#define MI_BARRIER_STAMP_ZEROED_PAGE(PointerTimeStamp) ((void)0)
#endif
#ifndef MI_MAKE_TRANSITION_KERNELPTE_VALID
#define MI_MAKE_TRANSITION_KERNELPTE_VALID(OUTPTE,PPTE) \
    ASSERT (((PPTE)->u.Hard.Valid == 0) && \
            ((PPTE)->u.Trans.Prototype == 0) && \
            ((PPTE)->u.Trans.Transition == 1)); \
    (OUTPTE).u.Long = (((PPTE)->u.Long & ~0xFFF) | \
                       (MmProtectToPteMask[(PPTE)->u.Trans.Protection]) | \
                       (MI_PTE_OWNER_KERNEL << 2)); \
    (OUTPTE).u.Hard.Global = 1; \
    (OUTPTE).u.Hard.Valid = 1; \
    (OUTPTE).u.Hard.Accessed = 1
#endif
#ifndef MI_MAKE_TRANSITION_PROTOPTE_VALID
#define MI_MAKE_TRANSITION_PROTOPTE_VALID(OUTPTE,PPTE) \
    ASSERT (((PPTE)->u.Hard.Valid == 0) && \
            ((PPTE)->u.Trans.Prototype == 0) && \
            ((PPTE)->u.Trans.Transition == 1)); \
    (OUTPTE).u.Long = (((PPTE)->u.Long & ~0xFFF) | \
                       (MmProtectToPteMask[(PPTE)->u.Trans.Protection]) | \
                       (MmPteGlobal.u.Long)); \
    (OUTPTE).u.Hard.Valid = 1; \
    (OUTPTE).u.Hard.Accessed = 1
#endif

extern ULONG_PTR MmBootImageSize;
extern ULONG MiMaximumWorkingSet;
extern ULONG_PTR MiUseMaximumSystemSpace;
extern ULONG_PTR MiUseMaximumSystemSpaceEnd;
extern ULONG MiMaximumSystemCacheSizeExtra;
extern MMPTE MmPteGlobal;
extern PVOID MmHyperSpaceEnd;
'''

    if "#ifndef MI_CHECK_PAGE_ALIGNMENT" not in text:
        text = text.replace(marker, definitions + "\n" + marker, 1)
    else:
        for symbol in (
            "MI_MAXIMUM_PAGEFILE_SIZE",
            "MI_WRITE_INVALID_PTE_WITHOUT_WS",
            "MI_GET_NEXT_COLOR",
            "MI_GET_MODIFIED_PAGE_BY_COLOR",
            "MI_GET_MODIFIED_PAGE_ANY_COLOR",
            "MI_SET_PAGING_FILE_INFO",
            "MI_IS_PAGE_TABLE_ADDRESS",
            "MI_IS_KERNEL_PAGE_TABLE_ADDRESS",
            "MI_IS_WRITE_COMBINE_ENABLED",
            "MI_BARRIER_STAMP_ZEROED_PAGE",
            "MI_MAKE_TRANSITION_KERNELPTE_VALID",
            "MI_MAKE_TRANSITION_PROTOPTE_VALID",
        ):
            if symbol not in text:
                text = text.replace(marker, definitions + "\n" + marker, 1)
                break
    return text


def edit_ntosdef(text):
    if "typedef struct _KERNEL_STACK_SEGMENT" in text:
        return text
    marker = "typedef CCHAR KPROCESSOR_MODE;"
    block = '''typedef struct _KERNEL_STACK_SEGMENT {
    ULONG_PTR StackLimit;
    ULONG_PTR KernelStack;
    ULONG_PTR InitialStack;
    ULONG_PTR ActualLimit;
} KERNEL_STACK_SEGMENT, *PKERNEL_STACK_SEGMENT;

'''
    if marker not in text:
        raise RuntimeError("KPROCESSOR_MODE marker not found in ntosdef.h")
    return text.replace(marker, block + marker, 1)


def edit_ps(text):
    # INITIAL_PEB is an internal NTOS hand-off structure, not the user-mode
    # PEB ABI. Reuse its existing SpareBool byte so its size/layout is stable.
    text = text.replace(
        "BOOLEAN SpareBool;                  //\n    HANDLE Mutant;",
        "BOOLEAN ImageUsesLargePages;        //\n    HANDLE Mutant;",
        1,
    )
    old = '''NTSTATUS
PsMapSystemDll (
    IN PEPROCESS Process,
    OUT PVOID *DllBase OPTIONAL
    );'''
    new = '''NTSTATUS
PsMapSystemDll (
    IN PEPROCESS Process,
    OUT PVOID *DllBase OPTIONAL,
    IN LOGICAL UseLargePages
    );'''
    if old in text:
        text = text.replace(old, new, 1)
    return text


def edit_mm_h(text):
    if "MmGrowKernelStackEx (" not in text:
        marker = "#ifdef ALLOC_PRAGMA"
        block = '''NTSTATUS
MmGrowKernelStackEx (
    __in PVOID CurrentStack,
    __in SIZE_T CommitSize
    );

'''
        if marker not in text:
            raise RuntimeError("ALLOC_PRAGMA marker not found in mm.h")
        text = text.replace(marker, block + marker, 1)
    return text


def edit_procsup(text):
    # The missing KERNEL_STACK_SEGMENT typedef makes the compiler lose the
    # declaration grammar in this region. Keep the actual WRK implementation
    # in place and only add the needed forward declaration for the helper.
    needle = "VOID\nMiOutPageSingleKernelStack ("
    if needle in text:
        pos = text.find(needle)
        prefix = text[:pos]
        if "MiOutPageSingleKernelStack (" not in prefix:
            decl = '''VOID
MiOutPageSingleKernelStack (
    IN PKTHREAD Thread,
    IN PKERNEL_STACK_SEGMENT StackInfo,
    IN PMMPTE_FLUSH_LIST PteFlushList
    );

'''
            text = prefix + decl + text[pos:]
    return text


def edit_pfsup(text):
    # Active ntoskrnl uses the WRK v1.2 one-PFN locked-page-charge helpers.
    # Remove only the obsolete SP0 diagnostic CallerId arguments.
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


# Only active ntoskrnl files are ever touched. ntos-old is intentionally absent.
edit("ntoskrnl/mm/i386/mi386.h", edit_mi386)
edit("ntoskrnl/inc/ntosdef.h", edit_ntosdef)
edit("ntoskrnl/inc/ps.h", edit_ps)
edit("ntoskrnl/inc/mm.h", edit_mm_h)
edit("ntoskrnl/mm/procsup.c", edit_procsup)
edit("ntoskrnl/mm/pfsup.c", edit_pfsup)

print("WRK v1.2/SP0 compatibility edits applied to active ntoskrnl only")
