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

; Macro change note:
;
;   This macro pair used to do an unconditional sti coming back from the 16-bit
;   side, this potentially caused problems in APM. Now we save and restore the
;   flag state
;

;++
;
;   STACK32_TO_STACK16
;
;   Macro Description:
;
;       This macro remaps current 32bit stack to 16bit stack.
;
;   Arguments:
;
;       None.
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
        MOV     WORD PTR [ECX].KgdtBaseLow, AX
        SHR     EAX, 16
        MOV     BYTE PTR [ECX].KgdtBaseMid, AL
        MOV     BYTE PTR [ECX].KgdtBaseHi, AH
        CLI
        SUB     ESP, EDX
        MOV     EAX, KGDT_STACK16
        MOV     SS, AX

;
; NOTE that we MUST leave interrupts remain off.
; We'll turn it back on after we switch to 16 bit code.
;

ENDM

;++
;
;   STACK16_TO_STACK32
;
;   Macro Description:
;
;       This macro remaps current 32bit stack to 16bit stack.
;
;   Arguments:
;
;       None.
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
;
; Routine Description:
;
;     This routine returns the starting address of GDT of current processor.
;
; Arguments:
;
;     None.
;
; Return Value:
;
;     return Pcr->GDT
;
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
;
; Routine Description:
;
;     This function invokes ABIOS service function for device driver.  This
;     routine is executing at DIAPTCH_LEVEL to prevent context swapping.
;
;     N.B. We arrive here from the Ke386AbiosCall with a 32bit CS. That is,
;     we're executing the code with cs:eip where cs contains a selector for a
;     32bit flat segment. We want to get to a 16bit cs. That is, cs:ip.
;     The reason is that ABIOS is running at 16 bit segment.
;     Before we can call ABIOS service we must load ss and cs segment
;     registers with selectors for 16bit segments.  We start by pushing a far
;     pointer to a label in the macro and then doing a retf. This allows us
;     to fall through to the next instruction, but we're now executing
;     through cs:ip with a 16bit CS. Then, we remap our 32-bit stack to 16-bit
;     stack.
;
; Arguments:
;
;     AbiosFunction - a 16:16 pointer to the abios service function.
;
;     DeviceBlockPointer - a 16:16 pointer to Device Block.
;
;     FunctionTransferTable - a 16:16 pointer to Function Transfer Table.
;
;     RequestBlock - a 16:16 pointer to device driver's request block.
;
; Return Value:
;
;     None.
;--

KacAbiosFunction        EQU     [EBP + 8]
KacDeviceBlock          EQU     [EBP + 12]
KacFunctionTable        EQU     [EBP + 16]
KacRequestBlock         EQU     [EBP + 20]

cPublicProc _KiI386CallAbios,4

;
; We're using a 32bit CS:EIP - go to a 16bit CS:IP
; Note the base of KiAbiosCallSelector is the flat address of _KiI386AbiosCall
; routine.
;

        PUSH    EBP
        MOV     EBP, ESP
        PUSH    EBX

        COPY_CALL_FRAME _KiBiosFrame
        SUB     ESP, LocalStack          ; After C style frame
        CurrentIrql                             ; Local Variable
        PUSH    EAX                             ; Local Variable

        CMP     AL, DISPATCH_LEVEL              ; Is irql > Dispatch_level?
        JAE     SHORT Kac00

; Raise to Dispatch Level
        RaiseIrql DISPATCH_LEVEL
        MOV     [ESP], AL

Kac00:

;
; Set up parameters on stack before remapping stack.
;

        PUSH    WORD PTR KGDT_CDA16             ; CDA anchor selector
        PUSH    KacRequestBlock                 ; Request Block
        PUSH    KacFunctionTable                ; Func transfer table
        PUSH    KacDeviceBlock                  ; Device Block
        MOV     EBX, KacAbiosFunction           ; (ebx)-> Abios Entry

;
; Remap current stack to 16:16 stack.  The base of the 16bit stack selector is
; the base of current kernel stack.
;

        INC     _KiInBiosCall                         ; Set the 'In Bios' flag
IFDEF DBG
        CMP   _KiInBiosCall, 2
        JB  @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

        STACK32_TO_STACK16                      ; Switch to 16bit stack
        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kac40 - OFFSET FLAT:_KiI386CallAbios@16)
        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kac30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kac30:

;
; Stack switching (from 32 to 16) turns interrupt off.  We must turn it
; back on.
;

        STI
        PUSH    BX                              ; Yes, BX not EBX!
        RETF
Kac40:
        ADD     ESP, 14                         ; pop out all the parameters

        STACK16_TO_STACK32                      ; switch back to 32 bit stack

;
; Pull callers flat return address off stack and push the
; flat code selector followed by the return offset, then
; execute a far return and we'll be back in the 32-bit code space.
;

        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kac50
        DB      OPERAND_OVERRIDE
        RETF
Kac50:
        POP     ECX                             ; [ecx] = OldIrql
        POP     EBX                             ; restore ebx
        CMP     CL, DISPATCH_LEVEL
        JAE     SHORT Kac60

        LowerIrql CL

Kac60:

        DEC     _KiInBiosCall                          ;Clear 'In Bios' Flag

        ADD     ESP, LocalStack                           ; subtract off the scratch space
        POP     EBP
        stdRET    _KiI386CallAbios

stdENDP _KiI386CallAbios


;; ********************************************************
;;
;; BEGIN - power_management
;;
;;

;++
; VOID
; KeI386Call16BitFunction (
;     IN OUT PCONTEXT Regs
;     )
;
; Routine Description:
;
;     This function calls the 16 bit function specified in the Regs.
;
; Parameters:
;
;     Regs - supplies a pointer to register context to call 16 function.
;
;   NOTE: Caller must be at DPC_LEVEL
;
;--

cPublicProc _KeI386Call16BitFunction,1

    ;  verify CurrentIrql
    ;  verify context flags

        PUSH    EBP                             ; save nonvolatile registers
        PUSH    EBX
        PUSH    ESI
        PUSH    EDI
        MOV     EBX, DWORD PTR [ESP + 20]       ; (ebx)-> Context

        COPY_CALL_FRAME _KiBiosFrame

        SUB     ESP, LocalStack          ; After prolog

        INC    _KiInBiosCall                         ; Set the 'In Bios' flag
IFDEF DBG
        CMP   _KiInBiosCall, 2
        JB  @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

;
; We're using a 32bit CS:EIP - go to a 16bit CS:IP
; Note the base of KiAbiosCallSelector is the flat address of _KiI386AbiosCall
; routine.
;

;
; Remap current stack to 16:16 stack.  The base of the 16bit stack selector is
; the base of current kernel stack.
;

        STACK32_TO_STACK16                      ; Switch to 16bit stack
    ;
    ; Push return address from 16 bit function call to kernel
    ;

        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kbf40 - OFFSET FLAT:_KiI386CallAbios@16)

        ;
        ; Load context to call with
        ;

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

    ;
    ; Switch to 16bit CS
    ;
        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kbf30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kbf30:
    ;
    ; "call" to 16 bit function
    ;
        IRETD

Kbf40:
    ;
    ; Push some of the returned context which will be needed to
    ; switch back to the 32 bit SS & CS.
    ;
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

    ;
    ; Switch back to 32 bit stack
    ;

        STACK16_TO_STACK32

;
; Push the flat code selector followed by the return offset, then
; execute a far return and we'll be back in the 32-bit code space.
;


        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kbf50
        DB      OPERAND_OVERRIDE
        RETF

Kbf50:
    ;
    ; Return resulting context
    ;

        MOV     EAX, DWORD PTR [ESP+44+LocalStack]     ; (eax) = Context Record
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

;
; Restore regs & return
;
        DEC     _KiInBiosCall                         ; Clear  the 'In Bios' flag

        ADD     ESP, LocalStack                                          ;remove scratch space
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
;
; Routine Description:
;
;     This function calls the 16 bit function which supports C style calling convention.
;
; Parameters:
;
;     EntryOffset and EntrySelector - specifies the entry point of the 16 bit function.
;
;     Parameters - supplies a pointer to a parameter block which will be
;         passed to 16 bit function as parameters.
;
;     Size - supplies the size of the parameter block.
;
;   NOTE: Caller must be at DPC_LEVEL
;
; Returned Value:
;
;     AX returned by 16 bit function.
;
;--

cPublicProc _KeI386Call16BitCStyleFunction,4

;
;  verify CurrentIrql
;  verify context flags
;

        PUSH    EBP                             ; save nonvolatile registers
        PUSH    EBX
        PUSH    ESI
        PUSH    EDI

        COPY_CALL_FRAME _KiBiosFrame

        INC     _KiInBiosCall                         ; Set the 'In Bios' flag
IFDEF DBG
        CMP   _KiInBiosCall, 2
        JB  @F
        PUSH    OFFSET FLAT:KiBiosReenteredAssert
        CALL    _DbgPrint
        ADD     ESP, 4
@@:
ENDIF

        MOV     EDI, ESP
        SUB     ESP, LocalStack          ;  now, add in some scratch space
        MOV     ESI, DWORD PTR [ESP + LocalStack +28]       ; (esi)->BiosParameters
        OR         ESI, ESI
        JZ         SHORT @F

        MOV    ECX, [ESP + LocalStack +32]                 ; (ecx) = parameter size
        SUB    ESP, ECX                        ; allocate space on TOS to copy parameters

        MOV   EDI, ESP
        REP     MOVSB                           ; (edi)-> Top of nonvolatile reg save area
        ADD    EDI, LocalStack           ; edi now points to original stack

@@:

;
; We're using a 32bit CS:EIP - go to a 16bit CS:IP
; Note the base of KiAbiosCallSelector is the flat address of _KiI386AbiosCall
; routine.
;

;
; Remap current stack to 16:16 stack.  The base of the 16bit stack selector is
; the base of current kernel stack.
;

        STACK32_TO_STACK16                      ; Switch to 16bit stack

;
; Push return address from 16 bit function call to kernel
;

        PUSH    WORD PTR KGDT_CODE16
        PUSH    WORD PTR (OFFSET FLAT:Kbfex40 - OFFSET FLAT:_KiI386CallAbios@16)

        PUSH    WORD PTR 0200h                  ; flags
        PUSH    WORD PTR [EDI + 24 ]             ; entry selector
        PUSH    WORD PTR [EDI + 20 ]             ; entry offset

;
; Switch to 16bit CS
;
        PUSH    KGDT_CODE16
        PUSH    OFFSET FLAT:Kbfex30 - OFFSET FLAT:_KiI386CallAbios@16
        RETF

Kbfex30:
;
; "call" to 16 bit function
;
        IRETD

Kbfex40:
;
; Save return value.
;

        DB      OPERAND_OVERRIDE
        PUSH    EAX

;
; Restore Flat mode segment registers.
;

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R0_PCR
        MOV     FS, AX

        DB      OPERAND_OVERRIDE
        MOV     EAX, KGDT_R3_DATA OR RPL_MASK
        MOV     DS, AX
        MOV     ES, AX

        XOR     EAX, EAX

;
; Switch back to 32 bit stack
;

        STACK16_TO_STACK32

;
; Push the flat code selector followed by the return offset, then
; execute a far return and we'll be back in the 32-bit code space.
;


        DB      OPERAND_OVERRIDE
        PUSH    KGDT_R0_CODE
        DB      OPERAND_OVERRIDE
        PUSH    OFFSET FLAT:Kbfex50
        DB      OPERAND_OVERRIDE
        RETF

Kbfex50:
        POP     EAX

;
; Restore regs & return
;
        DEC    _KiInBiosCall                         ; Clear  the 'In Bios' flag

        MOV     ESP, EDI                                 ; Also removes the scratch space!
        POP     EDI
        POP     ESI
        POP     EBX
        POP     EBP
        stdRET    _KeI386Call16BitCStyleFunction

stdENDP _KeI386Call16BitCStyleFunction

;
; BBT cannot instrument code between BBT_Exclude_Selector_Code_Begin and this label
;

        PUBLIC  _BBT_Exclude_Selector_Code_End
_BBT_Exclude_Selector_Code_End  EQU     $
        INT 3

;;
;; END - power_management
;;
;; ********************************************************


        PUBLIC  _KiEndOfCode16
_KiEndOfCode16  EQU     $



_TEXT   ENDS
        END
