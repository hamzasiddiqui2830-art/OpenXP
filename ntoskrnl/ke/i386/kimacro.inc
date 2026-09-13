;++
;
; Copyright (c) OpenXP
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;       kimacro.inc
;
; Abstract:
;
;       This module contains the macros used by kernel assembler code.
;       It includes macros to manipulate interrupts, support system
;       entry and exit for syscalls, faults, and interrupts, and
;       manipulate floating point state.
;
;--

;++
;
;   These constants are used by the fpo directives in this file.
;
FPO_LOCALS      EQU     0
FPO_PARAMS      EQU     0
FPO_PROLOG      EQU     0
FPO_REGS        EQU     0
FPO_USE_EBP     EQU     0
FPO_TRAPFRAME   EQU     1
;
;--


;++
;   POLL_DEBUGGER
;--

POLL_DEBUGGER   MACRO
    LOCAL   a, b, c_

    IFDEF DEVL
        EXTRNP   _DbgBreakPointWithStatus, 1
        stdCall _KdPollBreakIn
        OR      AL, AL
        JZ      SHORT c_
        stdCall _DbgBreakPointWithStatus, <DBG_STATUS_CONTROL_C>
c_:
    ENDIF
ENDM

;++
;   ASSERT_FS
;--

ASSERT_FS   MACRO
    LOCAL   a, b

    IF DBG
        EXTRNP   _KeBugCheck, 1

        MOV     BX, FS
        CMP     BX, KGDT_R0_PCR
        JNZ     SHORT a

        CMP     DWORD PTR FS:[0], 0
        JNE     SHORT b

a:
        stdCall   _KeBugCheck, <-1>
        ALIGN 4
b:
    ENDIF
ENDM


;++
;   SET_DEBUG_DATA
;   USE ebx and edi.
;--

SET_DEBUG_DATA  MACRO

    IFE FPO
        MOV     EBX, [EBP+TsEbp]
        MOV     EDI, [EBP+TsEip]
        MOV     [EBP+TsDbgArgPointer], EDX
        MOV     [EBP+TsDbgArgMark], 0BADB0D00h
        MOV     [EBP+TsDbgEbp], EBX
        MOV     [EBP+TsDbgEip], EDI
    ENDIF

ENDM


;++
;   ENTER_DR_ASSIST
;--

ENTER_DR_ASSIST MACRO   EnterLabel, ExitLabel, NoAbiosAssist, NoV86Assist, V86R
    LOCAL   a, b

    PUBLIC  Dr_&EnterLabel
    ALIGN 4
Dr_&EnterLabel:

    TEST    DWORD PTR [EBP+TsEFlags], EFLAGS_V86_MASK
    JNZ     SHORT a

    .ERRNZ (MODE_MASK AND 0FFFFFF00h)
    TEST    BYTE PTR [EBP+TsSegCs], MODE_MASK
    JZ      Dr_&ExitLabel

a:  MOV     EBX, DR0
    MOV     ECX, DR1
    MOV     EDI, DR2
    MOV     [EBP+TsDr0], EBX
    MOV     [EBP+TsDr1], ECX
    MOV     [EBP+TsDr2], EDI
    MOV     EBX, DR3
    MOV     ECX, DR6
    MOV     EDI, DR7
    MOV     [EBP+TsDr3], EBX
    MOV     [EBP+TsDr6], ECX
    XOR     EBX, EBX
    MOV     [EBP+TsDr7], EDI

    MOV     DR7, EBX

    MOV     EDI, DWORD PTR FS:[PcPrcb]
    MOV     EBX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr0]
    MOV     ECX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr1]
    MOV     DR0, EBX
    MOV     DR1, ECX
    MOV     EBX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr2]
    MOV     ECX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr3]
    MOV     DR2, EBX
    MOV     DR3, ECX
    MOV     EBX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr6]
    MOV     ECX, [EDI+PbProcessorState+PsSpecialRegisters+SrKernelDr7]
    MOV     DR6, EBX
    MOV     DR7, ECX

    IFNB <V86R>
        TEST    DWORD PTR [EBP+TsEFlags], EFLAGS_V86_MASK
        JZ      SHORT b
        JMP     Dr_&V86R
    ENDIF
b:
    JMP     Dr_&ExitLabel


    IFB <NoAbiosAssist>

        PUBLIC  Abios_&EnterLabel
        ALIGN 4
Abios_&EnterLabel:

        MOV     EAX, [ESP+TsErrCode]
        MOV     ECX, KGDT_R0_DATA
        MOV     EDX, FS:[PcPrcb]
        MOV     EDX, [EDX+PbCurrentThread]
        MOV     EDX, [EDX+ThStackLimit]
        SHL     EAX, 16
        ADD     EDX, ESP
        MOV     [ESP+TsErrCode], EAX
        MOV     SS, CX
        MOV     ESP, EDX
        MOV     EBP, EDX
        JMP     Abios_&ExitLabel

    ENDIF

    IFB <NoV86Assist>

        PUBLIC  V86_&EnterLabel
        ALIGN 4
V86_&EnterLabel:

        MOV     EAX, DWORD PTR [EBP+TsV86Fs]
        MOV     EBX, DWORD PTR [EBP+TsV86Gs]
        MOV     ECX, DWORD PTR [EBP+TsV86Es]
        MOV     EDX, DWORD PTR [EBP+TsV86Ds]
        MOV     [EBP+TsSegFs], AX
        MOV     [EBP+TsSegGs], BX
        MOV     [EBP+TsSegEs], CX
        MOV     [EBP+TsSegDs], DX
        JMP     V86_&ExitLabel

    ENDIF

ENDM

;++
;   ENTER_SYSCALL
;--

ENTER_SYSCALL MACRO     AssistLabel, TargetLabel, NoFSLoad, RejectVdmLabel, SaveEcx

    IFDEF KERNELONLY

        PUSH    0
        PUSH    EBP
        PUSH    EBX
        PUSH    ESI
        PUSH    EDI
        IFB <NoFSLoad>
            PUSH    FS
            MOV     EBX, KGDT_R0_PCR
            MOV     FS, BX
        ELSE
            PUSH    KGDT_R3_TEB OR RPL_MASK
        ENDIF

        MOV     ESI, PCR[PcPrcbData+PbCurrentThread]

        PUSH    PCR[PcExceptionList]
        MOV     PCR[PcExceptionList], EXCEPTION_CHAIN_END

        PUSH    [ESI+ThPreviousMode]
        SUB     ESP, TsPreviousPreviousMode
        MOV     EBX, [ESP+TsSegCS]
        AND     EBX, MODE_MASK
        MOV     [ESI+ThPreviousMode], BL

        MOV     EBP, ESP
        MOV     EBX, [ESI+ThTrapFrame]
        MOV     [EBP+TsEdx], EBX
        IFNB <SaveEcx>
            MOV     [EBP+TsEcx], ECX
        ENDIF
        AND     DWORD PTR [EBP+TsDr7], 0
        TEST    BYTE PTR [ESI+ThDebugActive], 0FFh
        MOV     [ESI+ThTrapFrame], EBP
        CLD
        
        JNZ     Dr_&AssistLabel

Dr_&TargetLabel:

        SET_DEBUG_DATA
        STI

    ELSE
        %OUT    ENTER_SYSCALL outside of kernel
        .ERR
    ENDIF
ENDM

;++
;   ENTER_INTERRUPT
;--

ENTER_INTERRUPT MACRO   AssistLabel, TargetLabel, PassParm
    LOCAL b

    IFB <PassParm>
        PUSH    ESP
    ENDIF
    PUSH    EBP
    PUSH    EBX
    PUSH    ESI
    PUSH    EDI

    SUB     ESP, TsEdi
    MOV     EBP, ESP

    MOV     [ESP+TsEax], EAX
    MOV     [ESP+TsEcx], ECX
    MOV     [ESP+TsEdx], EDX
    IF DBG
        MOV     DWORD PTR [ESP+TsPreviousPreviousMode], -1
    ENDIF

    TEST    DWORD PTR [ESP+TsEFlags], EFLAGS_V86_MASK

    JNZ     V86_&AssistLabel

    CMP     WORD PTR [ESP+TsSegCs], KGDT_R0_CODE
    JZ      SHORT @F

    MOV     [ESP+TsSegFs], FS
    MOV     [ESP+TsSegDs], DS
    MOV     [ESP+TsSegEs], ES
    MOV     [ESP+TsSegGs], GS

V86_&TargetLabel:
    MOV     EBX, KGDT_R0_PCR
    MOV     EAX, KGDT_R3_DATA OR RPL_MASK
    MOV     FS, BX
    MOV     DS, AX
    MOV     ES, AX
@@:
    MOV     EBX, FS:[PcExceptionList]
    MOV     FS:[PcExceptionList], EXCEPTION_CHAIN_END
    MOV     [ESP+TsExceptionList], EBX

    IFNB <PassParm>
        LEA     EAX, [ESP+TsErrCode]
        LEA     ECX, [ESP+TsEip]
        MOV     EBX, SS:[EAX]
        MOV     SS:[EAX], ECX
    ENDIF

    CMP     ESP, 10000h
    JB      Abios_&AssistLabel

    MOV     DWORD PTR [ESP+TsErrCode], 0
Abios_&TargetLabel:

    IFDEF PcPrcbData
        MOV     ECX, PCR[PcPrcbData+PbCurrentThread]
    ELSE
        MOV     ECX, PCR[PcPrcb]
        MOV     ECX, [ECX+PbCurrentThread]
    ENDIF

    CLD

    AND     DWORD PTR [EBP+TsDr7], 0
    TEST    BYTE PTR [ECX+ThDebugActive], 0FFh
    IFNB <PassParm>
        PUSH    EBX
    ENDIF
    JNZ     Dr_&AssistLabel

Dr_&TargetLabel:

    SET_DEBUG_DATA

ENDM

;++
;   ENTER_INTERRUPT_FORCE_STATE
;--

ENTER_INTERRUPT_FORCE_STATE MACRO   AssistLabel, TargetLabel, PassParm
    LOCAL b

    IFB <PassParm>
        PUSH    ESP
    ENDIF
    PUSH    EBP
    PUSH    EBX
    PUSH    ESI
    PUSH    EDI

    SUB     ESP, TsEdi
    MOV     EBP, ESP

    MOV     [ESP+TsEax], EAX
    MOV     [ESP+TsEcx], ECX
    MOV     [ESP+TsEdx], EDX
    IF DBG
        MOV     DWORD PTR [ESP+TsPreviousPreviousMode], -1
    ENDIF

    TEST    DWORD PTR [ESP+TsEflags], EFLAGS_V86_MASK

    JNZ     V86_&AssistLabel

V86_&TargetLabel:
    MOV     EBX, KGDT_R0_PCR
    MOV     EAX, KGDT_R3_DATA OR RPL_MASK
    MOV     FS, BX
    MOV     DS, AX
    MOV     ES, AX
@@:
    MOV     EBX, FS:[PcExceptionList]
    MOV     FS:[PcExceptionList], EXCEPTION_CHAIN_END
    MOV     [ESP+TsExceptionList], EBX

    IFNB <PassParm>
        LEA     EAX, [ESP+TsErrCode]
        LEA     ECX, [ESP+TsEip]
        MOV     EBX, SS:[EAX]
        MOV     SS:[EAX], ECX
    ENDIF

    CMP     ESP, 10000h
    JB      Abios_&AssistLabel

    MOV     DWORD PTR [ESP+TsErrCode], 0
Abios_&TargetLabel:

    IFDEF PcPrcbData
        MOV     ECX, PCR[PcPrcbData+PbCurrentThread]
    ELSE
        MOV     ECX, PCR[PcPrcb]
        MOV     ECX, [ECX+PbCurrentThread]
    ENDIF
    CLD

    AND     DWORD PTR [EBP+TsDr7], 0
    TEST    BYTE PTR [ECX+ThDebugActive], 0FFh
    IFNB <PassParm>
        PUSH    EBX
    ENDIF
    JNZ     Dr_&AssistLabel

Dr_&TargetLabel:

    SET_DEBUG_DATA

ENDM

;++
;   ENTER_TRAP
;--

ENTER_TRAP MACRO    AssistLabel, TargetLabel
    LOCAL b

    IF DBG
        IFNDEF  _Ki16BitStackException
            EXTRNP   _Ki16BitStackException
        ENDIF
    ENDIF

    MOV     WORD PTR [ESP+2], 0

    PUSH    EBP
    PUSH    EBX
    PUSH    ESI
    PUSH    EDI

    PUSH    FS
    MOV     EBX, KGDT_R0_PCR
    MOV     FS, BX
    MOV     EBX, FS:[PcExceptionList]
    PUSH    EBX
    IF DBG
        PUSH    -1
    ELSE
        SUB     ESP, 4
    ENDIF
    PUSH    EAX
    PUSH    ECX
    PUSH    EDX

    PUSH    DS
    PUSH    ES
    PUSH    GS

    MOV     AX, KGDT_R3_DATA OR RPL_MASK
    SUB     ESP, TsSegGs
    MOV     DS, AX
    MOV     ES, AX

    IF DBG
        CMP     ESP, 10000h
        JB      _Ki16BitStackException
    ENDIF

    MOV     EBP, ESP

    TEST    DWORD PTR [ESP+TsEflags], EFLAGS_V86_MASK

    JNZ     V86_&AssistLabel

V86_&TargetLabel:

    IFDEF PcPrcbData
        MOV     ECX, PCR[PcPrcbData+PbCurrentThread]
    ELSE
        MOV     ECX, PCR[PcPrcb]
        MOV     ECX, [ECX+PbCurrentThread]
    ENDIF
    CLD

    AND     DWORD PTR [EBP+TsDr7], 0
    TEST    BYTE PTR [ECX+ThDebugActive], 0FFh

    JNZ     Dr_&AssistLabel

Dr_&TargetLabel:

    SET_DEBUG_DATA

ENDM

;++
;   EXIT_ALL
;--

?adjesp = 0
?RestoreAll = 1

EXIT_ALL MACRO  NoRestoreSegs, NoRestoreVolatiles, NoPreviousMode
    LOCAL   a, b, f, x
    LOCAL   Dr_ExitHelp, Dr_ExitHelp_Target
    LOCAL   Db_NotATrapFrame, Db_A, Db_NotValidEntry, NonFlatPm_Target

    ?adjesp = TsSegGs
    ?RestoreAll = 1

    IFNB <NoRestoreSegs>
        ?RestoreAll = 0
        ?adjesp = ?adjesp + 12
    ENDIF

    IFNB <NoRestoreVolatiles>
        IFE ?RestoreAll
            %OUT "EXIT_ALL NoRestoreVolatiles requires NoRestoreSegs"
            .ERR
        ENDIF
        ?adjesp = ?adjesp + 12
    ENDIF

    IFB <NoPreviousMode>
        IFNDEF KERNELONLY
            %OUT    EXIT_ALL can not restore previousmode outside kernel
            .ERR
        ENDIF
    ENDIF

    IF DBG
        PUSHFD
        POP     EDX

        TEST    EDX, EFLAGS_INTERRUPT_MASK
        JNZ     Db_NotValidEntry

        CMP     ESP, EBP
        JNE     Db_NotValidEntry

Db_A:   SUB     [ESP+TsDbgArgMark], 0BADB0D00h
        JNE     Db_NotATrapFrame

    ENDIF

    ASSERT_FS

    MOV     EDX, [ESP+TsExceptionList]
    IF DBG
        OR      EDX, EDX
        JNZ     SHORT @F
        INT 3
@@:
    ENDIF
    MOV     PCR[PcExceptionList], EDX

    IFB <NoPreviousMode>
        MOV     ECX, [ESP+TsPreviousPreviousMode]
        IF DBG
            CMP     ECX, -1
            JNE     @F
            INT 3
@@:
        ENDIF
        MOV     ESI, PCR[PcPrcbData+PbCurrentThread]
        MOV     [ESI+ThPreviousMode], CL
    ELSE
        IF DBG
            MOV     ECX, [ESP+TsPreviousPreviousMode]
            CMP     ECX, -1
            JE      @F
            INT 3
@@:
        ENDIF
    ENDIF

    TEST    DWORD PTR [ESP+TsDr7], (NOT DR7_RESERVED_MASK)
    JNZ     Dr_ExitHelp

Dr_ExitHelp_Target:

    TEST    DWORD PTR [ESP+TsEflags], EFLAGS_V86_MASK
    JNZ     V86ExitHelp

    TEST    WORD PTR [ESP+TsSegCs], FRAME_EDITED
    JZ      b

    IFE ?RestoreAll
        .ERRNZ MODE_MASK-1
        CMP     WORD PTR [ESP+TsSegCs], KGDT_R3_CODE OR RPL_MASK
        BT      WORD PTR [ESP+TsSegCs], 0
        CMC
        JA      f
    ENDIF
    IFB <NoRestoreVolatiles>
        MOV     EDX, [ESP+TsEdx]
        MOV     ECX, [ESP+TsEcx]
        MOV     EAX, [ESP+TsEax]
    ENDIF

    CMP     WORD PTR [EBP+TsSegCs], KGDT_R0_CODE
    JZ      SHORT @F

    IFB <NoRestoreSegs>
        LEA     ESP, [EBP+TsSegGs]
        POP     GS
        POP     ES
        POP     DS
    ENDIF
NonFlatPm_Target:
    LEA     ESP, [EBP+TsSegFs]
    POP     FS
@@:
    LEA     ESP, [EBP+TsEdi]

    POP     EDI
    POP     ESI
    POP     EBX
    POP     EBP

    CMP     WORD PTR [ESP+8], 80h
    JA      AbiosExitHelp

    ADD     ESP, 4

    IFNB <NoRestoreVolatiles>

        PUBLIC  _KiSystemCallExitBranch
        PUBLIC  _KiSystemCallExit
        PUBLIC  _KiSystemCallExit2
        PUBLIC  _KiSystemCallExit3

        TEST    DWORD PTR [ESP+4], MODE_MASK

_KiSystemCallExitBranch:
        JNZ     SHORT _KiSystemCallExit

        POP     EDX
        POP     ECX
        POPFD
        JMP     EDX


_KiSystemCallExit:

        IRETD

_KiSystemCallExit2:

        TEST    DWORD PTR [ESP+8], EFLAGS_TF
        JNE     SHORT _KiSystemCallExit

        POP     EDX
        ADD     ESP, 4
        AND     DWORD PTR [ESP], NOT EFLAGS_INTERRUPT_MASK
        POPFD
        POP     ECX

        STI

        iSYSEXIT

_KiSystemCallExit3:

        POP     ECX
        ADD     ESP, 8
        POP     ESP

        iSYSRET

    ENDIF

    IRETD

    IF DBG
Db_NotATrapFrame:
        ADD     [ESP+TsDbgArgMark], 0BADB0D00h
Db_NotValidEntry:
        INT 3
        JMP     Db_A
    ENDIF

    ALIGN DWORD
Dr_ExitHelp:

    TEST    DWORD PTR [EBP+TsEFlags], EFLAGS_V86_MASK
    JNZ     SHORT x

    TEST    DWORD PTR [EBP+TsSegCs], MODE_MASK
    JZ      Dr_ExitHelp_Target

x:  XOR     EBX, EBX
    MOV     ESI, [EBP+TsDr0]
    MOV     EDI, [EBP+TsDr1]
    MOV     DR7, EBX
    MOV     DR0, ESI
    MOV     EBX, [EBP+TsDr2]
    MOV     DR1, EDI
    MOV     DR2, EBX
    MOV     ESI, [EBP+TsDr3]
    MOV     EDI, [EBP+TsDr6]
    MOV     EBX, [EBP+TsDr7]
    MOV     DR3, ESI
    MOV     DR6, EDI
    MOV     DR7, EBX

    JMP     Dr_ExitHelp_Target

    IFE ?RestoreAll

f:      MOV     EAX, [ESP+TsEax]
        ADD     ESP, TsSegGs

        POP     GS
        POP     ES
        POP     DS

        POP     EDX
        POP     ECX
        JMP     NonFlatPm_Target

    ENDIF


b:  MOV     EBX, [ESP+TsTempSegCs]
    MOV     [ESP+TsSegCs], EBX

    MOV     EBX, [ESP+TsTempEsp]
    SUB     EBX, 12
    MOV     [ESP+TsErrCode], EBX

    MOV     ESI, [ESP+TsEflags]
    MOV     [EBX+8], ESI
    MOV     ESI, [ESP+TsSegCs]
    MOV     [EBX+4], ESI
    MOV     ESI, [ESP+TsEip]
    MOV     [EBX], ESI

    IFB <NoRestoreVolatiles>
        MOV     EAX, [ESP+TsEax]
    ENDIF

    IFB <NoRestoreVolatiles>
        MOV     EDX, [ESP+TsEdx]
        MOV     ECX, [ESP+TsEcx]
    ENDIF

    ADD     ESP, TsEdi
    POP     EDI
    POP     ESI
    POP     EBX
    POP     EBP

    MOV     ESP, [ESP]
    IRETD

ENDM


;++
;   INTERRUPT_EXIT
;--

INTERRUPT_EXIT     MACRO    DebugCheck
    LOCAL   a

    IFNB <DebugCheck>
        POLL_DEBUGGER
    ENDIF
    IF DBG
a:      MOV     ESI, OFFSET a
    ENDIF

    IFDEF __imp_Kei386EoiHelper@0
        CLI

        CALL    _HalEndSystemInterrupt@8
        JMP     DWORD PTR [__imp_Kei386EoiHelper@0]

    ELSE
        CLI
        CALL    DWORD PTR [__imp__HalEndSystemInterrupt@8]
        JMP     Kei386EoiHelper@0
    ENDIF
ENDM


;++
;   SPURIOUS_INTERRUPT_EXIT
;--

SPURIOUS_INTERRUPT_EXIT  MACRO
    LOCAL   a
    IF DBG
a:      MOV     ESI, OFFSET a
    ENDIF


    IFDEF __imp_Kei386EoiHelper@0
        JMP     DWORD PTR [__imp_Kei386EoiHelper@0]
    ELSE
        JMP     Kei386EoiHelper@0
    ENDIF
ENDM

;++
;   ENTER_TRAPV86
;--

ENTER_TRAPV86 MACRO DRENTER, V86ENTER
    SUB     ESP, TsErrCode
    MOV     WORD PTR [ESP+TsErrCode+2], 0
    MOV     [ESP+TsEbx], EBX
    MOV     [ESP+TsEax], EAX
    MOV     [ESP+TsEbp], EBP
    MOV     [ESP+TsEsi], ESI
    MOV     [ESP+TsEdi], EDI
    MOV     EBX, KGDT_R0_PCR
    MOV     EAX, KGDT_R3_DATA OR RPL_MASK
    MOV     [ESP+TsEcx], ECX
    MOV     [ESP+TsEdx], EDX
    IF DBG
        MOV     [ESP+TsPreviousPreviousMode], -1
        MOV     [ESP+TsDbgArgMark], 0BADB0D00h
    ENDIF
    MOV     FS, BX
    MOV     DS, AX
    MOV     ES, AX
    MOV     EBP, ESP
    MOV     EAX, PCR[PcExceptionList]
    MOV     [ESP+TsExceptionList], EAX
    MOV     EAX, DR7
    CLD
    TEST    EAX, (NOT DR7_RESERVED_MASK)
    MOV     [ESP+TsDr7], EAX
    JNZ     Dr_&DRENTER

Dr_&V86ENTER:
ENDM


FIXED_NTVDMSTATE_LINEAR_PC_AT       EQU 0714h
FIXED_NTVDMSTATE_LINEAR_PC_98       EQU 0614h
MACHINE_TYPE_MASK                   EQU 0FF00h
VDM_VIRTUAL_INTERRUPTS              EQU 0200h

;++
;   EXIT_TRAPV86
;--

EXIT_TRAPV86 MACRO
    LOCAL w, x, y, z

z:  MOV     EBX, PCR[PcPrcbData+PbCurrentThread]
    MOV     BYTE PTR [EBX+ThAlerted], 0
    CMP     BYTE PTR [EBX+ThApcState+AsUserApcPending], 0
    JNE     SHORT w

    ADD     ESP, TsEdx
    POP     EDX
    POP     ECX
    POP     EAX
    TEST    DWORD PTR [EBP+TsDr7], (NOT DR7_RESERVED_MASK)
    JNZ     SHORT x
y:
    ADD     ESP, 12
    POP     EDI
    POP     ESI
    POP     EBX
    POP     EBP
    ADD     ESP, 4
    IRETD

x:  XOR     EBX, EBX
    MOV     ESI, [EBP+TsDr0]
    MOV     EDI, [EBP+TsDr1]
    MOV     DR7, EBX
    MOV     EBX, [EBP+TsDr2]
    MOV     DR0, ESI
    MOV     DR1, EDI
    MOV     DR2, EBX
    MOV     ESI, [EBP+TsDr3]
    MOV     EDI, [EBP+TsDr6]
    MOV     EBX, [EBP+TsDr7]
    MOV     DR3, ESI
    MOV     DR6, EDI
    MOV     DR7, EBX
    JMP     SHORT y

w:
    RaiseIrql APC_LEVEL
    PUSH    EAX
    STI

    stdCall _KiDeliverApc, <1, 0, EBP>

    POP     ECX
    LowerIrql cl

    CLI

    TEST    DWORD PTR [EBP+TsEFlags], EFLAGS_V86_MASK
    JNZ     z

ENDM


;++
;   PERF_GET_TIMESTAMP
;--
PERF_GET_TIMESTAMP MACRO
    EXTRN   _WmiGetCpuClock:DWORD

    CALL    [_WmiGetCpuClock]

ENDM
