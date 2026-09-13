        title   "Abios Support Assembly Routines"
;++
;
; Copyright (c) OpenXP
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    abiosa.asm
;
; Abstract:
;
;    This module implements assembly code for ABIOS support.
;
;--
.386p
        .xlist
include ks386.inc
include callconv.inc                    ; calling convention macros
include i386\kimacro.inc
include irqli386.inc
        .list

extrn   _DbgPrint:PROC

extrn _KiStack16GdtEntry:DWORD

;
; This should be either 0 or 1, if it's greater than 1, then we've re-entered the BIOS.
;
extrn _KiInBiosCall:DWORD
extrn _FlagState:DWORD
extrn _KiBiosFrame:DWORD

OPERAND_OVERRIDE        EQU     66h
ADDRESS_OVERRIDE        EQU     67h
KGDT_CDA16              EQU     0E8h

LocalStack                              EQU     16          ; 4 DWORDS of slop for PnPBioses.

IFDEF DBG
extrn  KiBiosReenteredAssert:DWORD
ENDIF

;++
;
;   STACK32_TO_STACK16
;
;   Macro Description:
;
;       This macro remaps current 32bit stack to 16bit stack.
;
;--

STACK32_TO_STACK16      MACRO

        PUSHFD
        MOV     ECX, [ESP]
        MOV     _FlagState, ECX
        POPFD
        MOV     EAX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address
        MOV     EAX, [EAX]+ThStackLimit ; get thread stack base
        MOV     EDX, EAX
        MOV     ECX, _KiStack16GdtEntry
        MOV     WORD PTR [ECX+KgdtBaseLow], AX          ; FIXED: use + offset
        SHR     EAX, 16
        MOV     BYTE PTR [ECX+KgdtBaseMid], AL          ; FIXED: use + offset
        MOV     BYTE PTR [ECX+KgdtBaseHi], AH           ; FIXED: use + offset
        CLI
        SUB     ESP, EDX
        MOV     EAX, KGDT_STACK16
        MOV     SS, AX

ENDM

;++
;
;   STACK16_TO_STACK32
;
;   Macro Description:
;
;       This macro remaps current 32bit stack to 16bit stack.
;
;--

STACK16_TO_STACK32      MACRO   Stack32

        DB      OPERAND_OVERRIDE
        DB      ADDRESS_OVERRIDE
        MOV     EAX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address
        DB      OPERAND_OVERRIDE
        DB      ADDRESS_OVERRIDE
        MOV     EAX, [EAX]+ThStackLimit ; get thread stack limit
        CLI
        DB      OPERAND_OVERRIDE
        ADD     ESP, EAX
        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R0_DATA
        MOV     SS, AX
        DB      OPERAND_OVERRIDE
        DB      ADDRESS_OVERRIDE
        PUSH    DS:_FlagState
        DB      OPERAND_OVERRIDE
        POPFD
ENDM

; *** FIXED MACRO ***
; Use explicit offset arithmetic instead of dot notation.
COPY_CALL_FRAME MACRO FramePtr

        MOV     [FramePtr + TsEax], EAX
        MOV     [FramePtr + TsEbx], EBX
        MOV     [FramePtr + TsEcx], ECX
        MOV     [FramePtr + TsEdx], EDX
        MOV     [FramePtr + TsEsi], ESI
        MOV     [FramePtr + TsEdi], EDI
        MOV     [FramePtr + TsEbp], EBP
        MOV     [FramePtr + TsHardwareEsp], ESP
        MOV     EAX, FS
        MOV     [FramePtr + TsSegFs], EAX
        MOV     EAX, CS
        MOV     [FramePtr + TsSegCs], EAX
ENDM
        PAGE ,132
        SUBTTL  "Abios Support Code"
_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;
; BBT cannot instrument code between this label and BBT_Exclude_Selector_Code_End
;
        PUBLIC  _BBT_Exclude_Selector_Code_Begin
_BBT_Exclude_Selector_Code_Begin  EQU     $
        INT 3


;++
; ULONG
; KiAbiosGetGdt (
;     VOID
;     )
;--

cPublicProc _KiAbiosGetGdt,0

        MOV     EAX, PCR[PcGdt]
        stdRET    _KiAbiosGetGdt

stdENDP _KiAbiosGetGdt

;++
; VOID
; KiI386CallAbios(
;     IN KABIOS_POINTER AbiosFunction,
;     IN KABIOS_POINTER DeviceBlockPointer,
;     IN KABIOS_POINTER FunctionTransferTable,
;     IN KABIOS_POINTER RequestBlock
;     )
;--

KacAbiosFunction        EQU     [EBP + 8]
KacDeviceBlock          EQU     [EBP + 12]
KacFunctionTable        EQU     [EBP + 16]
KacRequestBlock         EQU     [EBP + 20]

cPublicProc _KiI386CallAbios,4

        PUSH    EBP
        MOV     EBP, ESP
        PUSH    EBX

        COPY_CALL_FRAME _KiBiosFrame
        SUB     ESP, LocalStack
        CurrentIrql
        PUSH    EAX

        CMP     AL, DISPATCH_LEVEL
        JAE     SHORT Kac00

        RaiseIrql DISPATCH_LEVEL
        MOV     [ESP], AL

Kac00:

        PUSH    WORD PTR KGDT_CDA16
        PUSH    KacRequestBlock
        PUSH    KacFunctionTable
        PUSH    KacDeviceBlock
        MOV     EBX, KacAbiosFunction

        INC     _KiInBiosCall
IFDEF DBG
        CMP     _KiInBiosCall, 2
        JB      @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

        STACK32_TO_STACK16
        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kac40 - OFFSET FLAT:_KiI386CallAbios@16)
        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kac30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kac30:

        STI
        PUSH    BX
        RETF
Kac40:
        ADD     ESP, 14

        STACK16_TO_STACK32

        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kac50
        DB      OPERAND_OVERRIDE
        RETF
Kac50:
        POP     ECX
        POP     EBX
        CMP     CL, DISPATCH_LEVEL
        JAE     SHORT Kac60

        LowerIrql cl                  ; FIXED: lowercase 'cl' to match IFIDN in macro

Kac60:

        DEC     _KiInBiosCall

        ADD     ESP, LocalStack
        POP     EBP
        stdRET    _KiI386CallAbios

stdENDP _KiI386CallAbios


;++
; VOID
; KeI386Call16BitFunction (
;     IN OUT PCONTEXT Regs
;     )
;--

cPublicProc _KeI386Call16BitFunction,1

        PUSH    EBP
        PUSH    EBX
        PUSH    ESI
        PUSH    EDI
        MOV     EBX, DWORD PTR [ESP + 20]

        COPY_CALL_FRAME _KiBiosFrame

        SUB     ESP, LocalStack

        INC     _KiInBiosCall
IFDEF DBG
        CMP     _KiInBiosCall, 2
        JB      @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

        STACK32_TO_STACK16

        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kbf40 - OFFSET FLAT:_KiI386CallAbios@16)

        PUSH    WORD PTR [EBX + CsEFlags]
        PUSH    WORD PTR [EBX + CsSegCs]
        PUSH    WORD PTR [EBX + CsEip]

        MOV     EAX, [EBX + CsEax]
        MOV     ECX, [EBX + CsEcx]
        MOV     EDX, [EBX + CsEdx]
        MOV     EDI, [EBX + CsEdi]
        MOV     ESI, [EBX + CsEsi]
        MOV     EBP, [EBX + CsEbp]
        PUSH    [EBX + CsSegGs]
        PUSH    [EBX + CsSegFs]
        PUSH    [EBX + CsSegEs]
        PUSH    [EBX + CsSegDs]
        MOV     EBX, [EBX + CsEbx]
        POP     DS
        POP     ES
        POP     FS
        POP     GS

        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kbf30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kbf30:
        IRETD

Kbf40:
        DB      OPERAND_OVERRIDE
        PUSH    DS

        DB      OPERAND_OVERRIDE
        PUSH    ES

        DB      OPERAND_OVERRIDE
        PUSH    FS

        DB      OPERAND_OVERRIDE
        PUSH    GS

        DB      OPERAND_OVERRIDE
        PUSH    EAX

        DB      OPERAND_OVERRIDE
        PUSHFD

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R0_PCR
        MOV     FS, AX

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R3_DATA OR RPL_MASK
        MOV     DS, AX
        MOV     ES, AX

        XOR     EAX, EAX

        STACK16_TO_STACK32

        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kbf50
        DB      OPERAND_OVERRIDE
        RETF

Kbf50:
        MOV     EAX, DWORD PTR [ESP+44+LocalStack]
        POP     [EAX + CsEflags]
        POP     [EAX + CsEax]
        POP     [EAX + CsSegGs]
        POP     [EAX + CsSegFs]
        POP     [EAX + CsSegEs]
        POP     [EAX + CsSegDs]

        MOV     [EAX + CsEbx], EBX
        MOV     [EAX + CsEcx], ECX
        MOV     [EAX + CsEdx], EDX
        MOV     [EAX + CsEdi], EDI
        MOV     [EAX + CsEsi], ESI
        MOV     [EAX + CsEbp], EBP

        DEC     _KiInBiosCall

        ADD     ESP, LocalStack
        POP     EDI
        POP     ESI
        POP     EBX
        POP     EBP
        stdRET    _KeI386Call16BitFunction

stdENDP _KeI386Call16BitFunction

;++
; USHORT
; KeI386Call16BitCStyleFunction (
;     IN ULONG EntryOffset,
;     IN ULONG EntrySelector,
;     IN PUCHAR Parameters,
;     IN ULONG Size
;     )
;--

cPublicProc _KeI386Call16BitCStyleFunction,4

        PUSH    EBP
        PUSH    EBX
        PUSH    ESI
        PUSH    EDI

        COPY_CALL_FRAME _KiBiosFrame

        INC     _KiInBiosCall
IFDEF DBG
        CMP     _KiInBiosCall, 2
        JB      @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

        MOV     EDI, ESP
        SUB     ESP, LocalStack
        MOV     ESI, DWORD PTR [ESP + LocalStack + 28]
        OR      ESI, ESI
        JZ      SHORT @F

        MOV     ECX, [ESP + LocalStack + 32]
        SUB     ESP, ECX

        MOV     EDI, ESP
        REP     MOVSB
        ADD     EDI, LocalStack

@@:

        STACK32_TO_STACK16

        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kbfex40 - OFFSET FLAT:_KiI386CallAbios@16)

        PUSH    WORD PTR 0200h
        PUSH    WORD PTR [EDI + 24]
        PUSH    WORD PTR [EDI + 20]

        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kbfex30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kbfex30:
        IRETD

Kbfex40:
        DB      OPERAND_OVERRIDE
        PUSH    EAX

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R0_PCR
        MOV     FS, AX

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R3_DATA OR RPL_MASK
        MOV     DS, AX
        MOV     ES, AX

        XOR     EAX, EAX

        STACK16_TO_STACK32

        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kbfex50
        DB      OPERAND_OVERRIDE
        RETF

Kbfex50:
        POP     EAX

        DEC     _KiInBiosCall

        MOV     ESP, EDI
        POP     EDI
        POP     ESI
        POP     EBX
        POP     EBP
        stdRET    _KeI386Call16BitCStyleFunction

stdENDP _KeI386Call16BitCStyleFunction

        PUBLIC  _BBT_Exclude_Selector_Code_End
_BBT_Exclude_Selector_Code_End  EQU     $
        INT 3

        PUBLIC  _KiEndOfCode16
_KiEndOfCode16  EQU     $

_TEXT   ENDS
        END
