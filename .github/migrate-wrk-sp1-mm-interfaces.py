from pathlib import Path
import re


def read(path):
    return Path(path).read_text(encoding='utf-8')


def write(path, text):
    Path(path).write_text(text, encoding='utf-8', newline='')


# Restore the WRK v1.2 spelling in active OpenXP sources.
for root in (Path('ntoskrnl'),):
    for path in root.rglob('*.c'):
        text = read(path)
        new = text.replace('MI_NONPAGEABLE_MEMORY_AVAILABLE', 'MI_NONPAGABLE_MEMORY_AVAILABLE')
        if new != text:
            write(path, new)
    for path in root.rglob('*.h'):
        text = read(path)
        new = text.replace('MI_NONPAGEABLE_MEMORY_AVAILABLE', 'MI_NONPAGABLE_MEMORY_AVAILABLE')
        if new != text:
            write(path, new)

mi = Path('ntoskrnl/mm/mi.h')
text = read(mi)
if '#define MI_NONPAGABLE_MEMORY_AVAILABLE()' not in text:
    raise SystemExit('WRK MI_NONPAGABLE_MEMORY_AVAILABLE definition not found')

mmh = Path('ntoskrnl/inc/mm.h')
text = read(mmh)
flush_block = '''
// WRK v1.2/SP1 MM flush flags.
#ifndef MM_FLUSH_ACQUIRE_FILE
#define MM_FLUSH_ACQUIRE_FILE       0x1
#endif
#ifndef MM_FLUSH_FAIL_COLLISIONS
#define MM_FLUSH_FAIL_COLLISIONS    0x2
#endif
#ifndef MM_FLUSH_IN_PARALLEL
#define MM_FLUSH_IN_PARALLEL        0x4
#endif
#ifndef MM_FLUSH_ASYNCHRONOUS
#define MM_FLUSH_ASYNCHRONOUS       0x8
#endif
#ifndef MM_FLUSH_SEG_DEREF
#define MM_FLUSH_SEG_DEREF          0x80000000
#endif
'''
if '#define MM_FLUSH_ACQUIRE_FILE' not in text:
    pos = text.rfind('#endif')
    if pos < 0:
        raise SystemExit('could not locate mm.h closing #endif')
    text = text[:pos] + flush_block + '\n' + text[pos:]
    write(mmh, text)

# Restore the WRK v1.2 x86 PTE/prototype-PTE primitives.
i386 = Path('ntoskrnl/mm/i386/mi386.h')
text = read(i386)
i386_block = '''

// WRK v1.2 prototype-PTE and COW PTE primitives.
#define MiProtoAddressForPte(proto_va) \\
   ((((((ULONG)proto_va - MmProtopte_Base) >> 1) & (ULONG)0x000000FE) | \\
    (((((ULONG)proto_va - MmProtopte_Base) << 2) & (ULONG)0xfffff800))) | \\
    MM_PTE_PROTOTYPE_MASK)

#define MiProtoAddressForKernelPte(proto_va) MiProtoAddressForPte(proto_va)

#define MI_MAKE_VALID_PTE_WRITE_COPY(PPTE) \\
    if ((PPTE)->u.Hard.Write == 1) { \\
        (PPTE)->u.Hard.CopyOnWrite = 1; \\
        (PPTE)->u.Hard.Write = 0; \\
    }

#define MI_MAKE_PROTECT_WRITE_COPY(PTE) \\
    if ((PTE).u.Soft.Protection & MM_PROTECTION_WRITE_MASK) { \\
        (PTE).u.Long |= MM_PROTECTION_COPY_MASK << MM_PROTECT_FIELD_SHIFT; \\
    }

#define MI_MAKE_TRANSITION_PTE_VALID(OUTPTE,PPTE) \\
    ASSERT (((PPTE)->u.Hard.Valid == 0) && \\
            ((PPTE)->u.Trans.Prototype == 0) && \\
            ((PPTE)->u.Trans.Transition == 1)); \\
    (OUTPTE).u.Long = (((PPTE)->u.Long & ~0xFFF) | \\
                       (MmProtectToPteMask[(PPTE)->u.Trans.Protection]) | \\
                       MiDetermineUserGlobalPteMask((PMMPTE)PPTE));
'''
if '#define MiProtoAddressForPte(proto_va)' not in text:
    pos = text.rfind('#endif')
    if pos < 0:
        raise SystemExit('could not locate mi386.h closing #endif')
    text = text[:pos] + i386_block + '\n' + text[pos:]
    write(i386, text)

# MiFlushPteList is the current one-argument interface. Update legacy calls
# while preserving unrelated calls and old source under ntos-old.
for path in Path('ntoskrnl/mm').rglob('*.c'):
    text = read(path)
    new = re.sub(r'(MiFlushPteList\s*\(\s*[^,;\n()]+)\s*,\s*(?:FALSE|TRUE)\s*\)', r'\1)', text)
    if new != text:
        write(path, new)

# Verify no active two-argument call remains.
for path in Path('ntoskrnl/mm').rglob('*.c'):
    text = read(path)
    if re.search(r'MiFlushPteList\s*\([^;\n]*,\s*(?:FALSE|TRUE)\s*\)', text):
        raise SystemExit(f'old two-argument MiFlushPteList call remains: {path}')

for path in Path('ntoskrnl').rglob('*'):
    if path.is_file() and path.suffix in {'.c', '.h'}:
        if 'MI_NONPAGEABLE_MEMORY_AVAILABLE' in read(path):
            raise SystemExit(f'old NONPAGEABLE spelling remains: {path}')

print('WRK SP1 MM interface migration completed.')
