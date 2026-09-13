;***
; em387.inc - Emulator Internal Format and Macros
;
; Copyright (c) OpenXP
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Purpose:
;       Emulator Internal Format and Macros
;
;*******************************************************************************

;++
;   GetEmData dest, use
;
;   Macro Description:
;       Retrieves emulator data segment selector into dest.
;       Note: _CRUISER and _DOS32EXT paths are legacy/dead code.
;--
GetEmData macro dest, use
    IFDEF _CRUISER
        mov dest, [edataSEG]
    ELSEIFDEF _DOS32EXT
        IFDIFI <use>, <ax>
            push eax
            call _SelKrnGetEmulData
            mov dest, ax
            pop eax
        ELSE
            call _SelKrnGetEmulData
            mov dest, ax
        ENDIF
    ENDIF
endm

;++
;   SKIP dist, target
;
;   Macro Description:
;       Optimizes very short jumps by treating the code as data to a "cmp"
;       instruction. This reduces jump time from 8+ clocks down to 2 clocks.
;       WARNING: This destroys the flags!
;
;       The raw byte emissions below are intentional. They encode the jump
;       displacement as the immediate operand of a cmp instruction so that
;       the CPU "executes" the jump bytes as harmless data.
;--
SKIP macro dist, target
    IF dist EQ 4
        db 3Dh                          ; cmp eax, <immed32>
    ELSEIF dist EQ 3
        db 3Dh, 0                       ; cmp eax, <immed32>
    ELSEIF dist EQ 2
        db 66h, 3Dh                     ; cmp ax, <immed16>
    ELSEIF dist EQ 1
        db 3Ch                          ; cmp al, <immed8>
    ELSE
        .err <SKIP: invalid distance>
    ENDIF

    IFNB <target>
        .erre $+dist EQ target
    ENDIF
endm

;*******************************************************************************
;
;   80x87 environment structures.
;
;*******************************************************************************

Env80x87_32 STRUCT
    E32_ControlWord     dw ?
    reserved1           dw ?
    E32_StatusWord      dw ?
    reserved2           dw ?
    E32_TagWord         dw ?
    reserved3           dw ?
    E32_CodeOff         dd ?
    E32_CodeSeg         dw ?
    reserved4           dw ?
    E32_DataOff         dd ?
    E32_DataSeg         dw ?
    reserved5           dw ?
Env80x87_32 ENDS

;---------------------------------------------------------------------------
;
; Emulator Internal Format:
;
;            +0  +1  +2  +3  +4  +5  +6  +7  +8  +9  +10 +11
;           .___.___.___.___.___.___.___.___.___.___.___.___.
;   ptr --> |___|___|___|___|___|___|___|___|___|___|___|___|
;            lsb                         msb tag sgn exl exh
;           |<---      mantissa         --->|       |exponent
;
;   The mantissa contains the leading 1 before the decimal point in the hi
;   bit of the msb. The exponent is not biased (signed two's complement).
;   The flag and tag bytes are as below.
;
;   bit:      7   6   5   4   3   2   1   0
;           .___.___.___.___.___.___.___.___.
;   Sign:   |___|_X_|_X_|_X_|_X_|_X_|_X_|_X_|  X = unused
;             ^
;     SIGN
;
;   bit:      7   6   5   4   3   2   1   0
;           .___.___.___.___.___.___.___.___.
;   Tag:    |___|___|_X_|_X_|___|___|___|___|  X = unused
;             ^   ^           ^   ^   ^   ^
;             |   |           |   |   |   |
;    387 tag -+---+           |   |   |   |
;                             |   |   |   |
;    Special enumeration -----+---+   |   |
;                                     |   |
;    Internal tag --------------------+---+
;
; There are four internal tags: Single, Double, Zero, Special. Within
; Special, there is NAN, Infinity, Denormal, and Empty.
;
; Tag            Mantissa        Exponent        Sign
; ---------------------------------------------------
; Zero           0               0               valid
; Empty          ?               ?               ?
; NAN            valid           TexpMax         valid
; Infinity       8000...000      TexpMax         valid
;
;*******************************************************************************

;*******************************************************************************
;   Stack entry defined with a struct.
;*******************************************************************************

EmStackEntry STRUCT
    bMan0   db ?
    bMan1   db ?
    bMan2   db ?
    bMan3   db ?
    bMan4   db ?
    bMan5   db ?
    bMan6   db ?
    bMan7   db ?
    bTag    db ?
    bSgn    db ?
    bExpLo  db ?
    bExpHi  db ?
EmStackEntry ENDS

wMantisa STRUCT
    wMan0   dw ?
    wMan1   dw ?
    wMan2   dw ?
    wMan3   dw ?
    TagSgn  dw ?
    wExp    dw ?
wMantisa ENDS

lMantisa STRUCT
    lManLo  dd ?
    lManHi  dd ?
    ExpSgn  dd ?
lMantisa ENDS

.erre SIZE lMantisa EQ SIZE wMantisa

Reg87Len equ SIZE lMantisa

;*******************************************************************************
;   bFlags and bTag constants.
;*******************************************************************************

bSign equ 80h

; INTERNAL flags
TAG_MASK    equ 3
TAG_SHIFT   equ 2

TAG_SNGL    equ 0               ; SINGLE: low 32 bits are zero
TAG_VALID   equ 1
TAG_ZERO    equ 2
TAG_SPCL    equ 3               ; NAN, Infinity, Denormal, Empty
ZEROorSPCL  equ 2               ; Test for Zero or Special

; Enumeration of "special":
TAG_SPCLBITS equ 0Ch
TAG_EMPTY   equ TAG_SPCL + (0 SHL TAG_SHIFT)
TAG_INF     equ TAG_SPCL + (1 SHL TAG_SHIFT)
TAG_NAN     equ TAG_SPCL + (2 SHL TAG_SHIFT)
TAG_DEN     equ TAG_SPCL + (3 SHL TAG_SHIFT)

; Tags used by the 387
T87_VALID   equ 0
T87_ZERO    equ 1
T87_SPCL    equ 2               ; NAN, Infinity, Denormal
T87_EMPTY   equ 3

; Combined tag word for each stack entry.
; Internal tags are in the low bits, 387 tags are in the high two bits.
bTAG_VALID  equ (T87_VALID SHL 6) OR TAG_VALID
bTAG_SNGL   equ (T87_VALID SHL 6) OR TAG_SNGL
bTAG_ZERO   equ (T87_ZERO  SHL 6) OR TAG_ZERO
bTAG_NAN    equ (T87_SPCL  SHL 6) OR TAG_NAN
bTAG_INF    equ (T87_SPCL  SHL 6) OR TAG_INF
bTAG_EMPTY  equ (T87_EMPTY SHL 6) OR TAG_EMPTY
bTAG_DEN    equ (T87_SPCL  SHL 6) OR TAG_DEN
bTAG_NOPOP  equ -1

bTAG_MASK   equ 3

MantissaByteCnt equ 8

; Exponent biases and limits
IexpBias    equ 3FFFh           ; 16,383
IexpMax     equ 7FFFh           ; Biased Exponent for Infinity
IexpMin     equ 0               ; Biased Exponent for zero

DexpBias    equ 3FFh            ; 1023
DexpMax     equ 7FFh            ; Biased Exponent for Infinity
DexpMin     equ 0               ; Biased Exponent for zero

SexpBias    equ 7Fh             ; 127
SexpMax     equ 0FFh            ; Biased Exponent for Infinity
SexpMin     equ 0               ; Biased Exponent for zero

TexpBias    equ 0               ; Bias for internal format of temp real
UnderBias   equ 24576           ; 3 * 2^13. Extra bias for unmasked underflow
TexpMax     equ IexpMax - IexpBias + TexpBias   ; NAN/Infinity exponent
TexpMin     equ IexpMin - IexpBias + 1          ; Smallest non-denormal exponent

; Control Word Format (CWcntl)
RoundControl equ 0Ch
    RCchop  equ 0Ch
    RCup    equ 08h
    RCdown  equ 04h
    RCnear  equ 0

PrecisionControl equ 03h
    PC24    equ 0
    PC53    equ 02h
    PC64    equ 03h

; Status Word Format (SWcc)
    C0      equ 01h
    C1      equ 02h
    C2      equ 04h
    C3      equ 40h
ConditionCode equ C3 OR C2 OR C1 OR C0
    CCgreater       equ 0
    CCless          equ C0
    CCequal         equ C3
    CCincomprable   equ C3 OR C2 OR C0

RoundUp         equ C1
StackOverflow   equ C1

; Status Flags Format (CURerr)
Invalid     equ 01h             ; chip status flags
Denormal    equ 02h
ZeroDivide  equ 04h
Overflow    equ 08h
Underflow   equ 10h
Precision   equ 20h
StackFlag   equ 40h
Summary     equ 80h

SavedErrs       equ Invalid OR Denormal OR ZeroDivide OR Overflow OR Underflow OR Precision OR StackFlag
LongSavedFlags  equ (CCincomprable SHL 16) OR (SavedErrs SHL 8)

;*******************************************************************************
;   Define emulator interrupt stack frame.
;*******************************************************************************

StackFrame STRUCT
    regEAX          dd ?
    regECX          dd ?
    regEDX          dd ?
    regEBX          dd ?
    regESP          dd ?
    regEBP          dd ?
    regESI          dd ?
    regEDI          dd ?
    OldCodeOff      dd ?
    OldLongStatus   dd ?
    regDS           dd ?
    regEIP          dd ?
    regCS           dd ?
    regFlg          dd ?
StackFrame ENDS

regAX       equ word ptr regEAX
OldStatus   equ word ptr OldLongStatus + 1

;*******************************************************************************
;   Define emulator entry point macro.
;*******************************************************************************

EM_ENTRY macro entryname
    IFDEF NT386
        PUBLIC ___&entryname
___&entryname:
    ENDIF
endm

Em87Busy equ 1
Em87Idle equ 0


IFDEF NT386
;*********************************************************************
;                     Emulator TEB Layout
;*********************************************************************

.errnz (TbSystemReserved1 AND 3)        ; Make sure TB is dword aligned

Numlev          equ 8                   ; Number of stack registers

InitControlWord equ 37Fh                ; Default - Round near, 64 bits, all masked
DefaultControlWord equ 27Fh             ; Default - Round near, 53 bits, all masked

EmulatorTebData STRUCT
    TbSystemResrvd  db TbSystemReserved1 DUP (?)    ; Skip to Emulator area

    RoundMode       dd ?                ; Address of rounding routine
    SavedRoundMode  dd ?                ; For restoring RoundMode
    ZeroVector      dd ?                ; Address of sum-to-zero routine
    TransRound      dd ?                ; Round mode w/o precision
    Result          dd ?                ; Result pointer
    PrevCodeOff     dd ?
    PrevDataOff     dd ?

    ; Emulator stack area (see comment below)
    CURstk          dd ?                ; init to start of stack
    BEGstk          db (Numlev - 1) * Reg87Len DUP (?)  ; Allocate register 1-7
    INITstk         db Reg87Len DUP (?)

    FloatTemp       db Reg87Len DUP (?)
    ArgTemp         db Reg87Len DUP (?)

    Einstall        db 0                ; Emulator installed flag
    SWerr           db ?                ; Initially no exceptions (sticky flags)
    SWcc            db ?                ; Condition codes from various operations
    CURerr          db ?                ; Internal flag reset after each operation
    CWmask          db ?                ; Exception masks
    CWcntl          db ?                ; Arithmetic control flags
    ErrMask         db ?
    dummy           db ?
EmulatorTebData ENDS

ENDstk          equ byte ptr INITstk + Reg87Len
LongStatusWord  equ dword ptr Einstall      ; Combine Einstall, CURerr, StatusWord
StatusWord      equ word ptr SWerr          ; Combine SWerr, SWcc
CurErrCond      equ word ptr SWcc           ; Combine SWcc, CURErr
LongControlWord equ dword ptr CWmask        ; Combine CWMask, CWcntl, ErrMask, dummy
ControlWord     equ word ptr CWmask         ; Combine CWMask, CWcntl

YFloatTemp      equ FloatTemp
YArgTemp        equ ArgTemp

; Compile-time layout assertions
.errnz (SWerr   - Einstall - 1)
.errnz (SWcc    - Einstall - 2)
.errnz (CURerr  - Einstall - 3)
.errnz (CWcntl  - CWmask   - 1)
.errnz (ErrMask - CWmask   - 2)
.errnz (dummy   - CWmask   - 3)

;*******************************************************************************
;
; Emulator stack area
;
; The top of stack pointer CURstk is initialized to the last register
; in the list; on a real 8087, this corresponds to hardware register 0.
; The stack grows toward lower addresses, so the first push (which is
; hardware register 7) is stored into the second-to-last slot.
;
; BEGstk --> |    reg 1    |  (lowest memory address)
;            |    reg 2    |
;            |    reg 3    |
;            |    reg 4    |
;            |    reg 5    |
;            |    reg 6    |
;            |    reg 7    |
;            |    reg 0    |  <-- Initial top of stack (empty)
; ENDstk -->
;
;*******************************************************************************

ENDIF ; IFDEF NT386
