#!/usr/bin/env python3
"""Apply narrowly-scoped MASM 14.51 compatibility fixes to the WRK sources.

This script is intentionally fail-closed: it only changes the exact legacy
MASM constructs known to fail with modern MSVC/MASM and refuses to continue if
an expected source block is missing or duplicated.

It does not modify ks386.inc or any C source.
"""

from __future__ import annotations

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
CALLOUT = ROOT / "ntoskrnl/ke/i386/callout.asm"
CALLCONV = ROOT / "sdk/sdk/inc/callconv.inc"


def read(path: Path) -> str:
    if not path.is_file():
        raise FileNotFoundError(f"Required file not found: {path}")
    return path.read_text(encoding="utf-8")


def write(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8", newline="")


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(
            f"{label}: expected exactly 1 match, found {count}; refusing to patch"
        )
    return text.replace(old, new, 1)


def patch_callout() -> None:
    text = read(CALLOUT)

    # ks386.inc exposes these legacy WRK fields as EQU offsets, not MASM
    # STRUCT fields. MASM 14.51 therefore rejects [reg].Field with A2166.
    # Restrict the conversion to callout.asm and to actual x86 registers.
    pattern = re.compile(
        r"(\[(?:eax|ebx|ecx|edx|esi|edi|ebp|esp)\])\.([A-Za-z_][A-Za-z0-9_]*)"
    )
    text, count = pattern.subn(r"\1+\2", text)
    if count == 0:
        raise RuntimeError(
            "callout.asm: no legacy [register].Field operands found; refusing to patch"
        )

    write(CALLOUT, text)
    print(f"patched {CALLOUT}: converted {count} register/EQU field operands")


def patch_callconv() -> None:
    text = read(CALLCONV)

    old_extrnp = '''EXTRNP  macro   Func,N,Thunk,FastCall
        ifb    <N>
            IFNDEF  Func&@0
                extrn       Func&@0:NEAR
            ENDIF
        else
            ifb     <FastCall>
                ifb     <Thunk>
                    EXTRNP2     Func,%(N*4)
                else
                    EXTRNTHUNK  Func,%(N*4)
                endif
            else
                cFCall&@&Func equ   (N*4)
                ifb     <Thunk>
                    EXTRNP2     &@&Func,%(N*4)
                else
                    EXTRNTHUNK  &@&Func,%(N*4)
                endif
            endif
        endif
endm
'''

    new_extrnp = '''EXTRNP  macro   Func,N,Thunk,FastCall
        ifb    <N>
            IFNDEF  @CatStr(Func,<@0>)
                extrn       @CatStr(Func,<@0>):NEAR
            ENDIF
        else
            ifb     <FastCall>
                ifb     <Thunk>
                    EXTRNP2     Func,%(N*4)
                else
                    EXTRNTHUNK  Func,%(N*4)
                endif
            else
                @CatStr(<cFCall@>,Func) equ   (N*4)
                ifb     <Thunk>
                    EXTRNP2     @CatStr(<@>,Func),%(N*4)
                else
                    EXTRNTHUNK  @CatStr(<@>,Func),%(N*4)
                endif
            endif
        endif
endm
'''

    old_extrnp2 = '''EXTRNP2 macro   Func,N
        IFNDEF  Func&@&N
            extrn   Func&@&N:NEAR
        ENDIF
endm
'''

    new_extrnp2 = '''EXTRNP2 macro   Func,N
        IFNDEF  @CatStr(Func,<@>,%N)
            extrn   @CatStr(Func,<@>,%N):NEAR
        ENDIF
endm
'''

    old_extrnthunk = '''EXTRNTHUNK macro   Func,N
        IFNDEF  __imp_&Func&@&N
            extrn       __imp_&Func&@&N:DWORD
        ENDIF
endm
'''

    new_extrnthunk = '''EXTRNTHUNK macro   Func,N
        IFNDEF  @CatStr(<__imp_>,Func,<@>,%N)
            extrn       @CatStr(<__imp_>,Func,<@>,%N):DWORD
        ENDIF
endm
'''

    for old, new, label in (
        (old_extrnp, new_extrnp, "EXTRNP"),
        (old_extrnp2, new_extrnp2, "EXTRNP2"),
        (old_extrnthunk, new_extrnthunk, "EXTRNTHUNK"),
    ):
        text = replace_once(text, old, new, label)

    write(CALLCONV, text)
    print(f"patched {CALLCONV}: modernized EXTRNP symbol construction")


def main() -> int:
    print(f"OpenXP root: {ROOT}")
    patch_callout()
    patch_callconv()
    print("MASM 14.51 compatibility patch completed successfully.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
