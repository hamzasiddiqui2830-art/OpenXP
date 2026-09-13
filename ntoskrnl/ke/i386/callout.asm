        title   "Call Out to User Mode"
;++
;
; Copyright (c) OpenXP
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    callout.asm
;
; Abstract:
;
;    This module implements the code necessary to call out from kernel
;    mode to user mode.
;
;--

.386p
        .xlist
include ks386.inc
include i386\kimacro.inc
include callconv.inc
include irqli386.inc
        .list

        EXTRN   _KiServiceExit:PROC
        EXTRN   _KeUserCallbackDispatcher:DWORD

        EXTRNP  _KeBugCheck2, 6
        EXTRNP  _MmGrowKernelStack, 1

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

        PAGE ,132
        SUBTTL  "Call User Mode Function"
;++
;
; NTSTATUS
; KiCallUserMode (
;    IN PVOID *Outputbuffer,
;    IN PULONG OutputLength
;    )
;
; Routine Description:
;
;    This function calls a user mode function from kernel mode.
;
;    N.B. This function calls out to user mode and the NtCallbackReturn
;        function returns back to the caller of this function. Therefore,
;        the stack layout must be consistent between the two routines.
;
; Arguments:
;
;    OutputBuffer - Supplies a pointer to the variable that receives
;        the address of the output buffer.
;
;    OutputLength - Supplies a pointer to a variable that receives
;        the length of the output buffer.
;
; Return Value:
;
;    The final status of the call out function is returned as the status
;    of the function.
;
;    N.B. This function does not return to its caller. A return to the
;        caller is executed when a NtCallbackReturn system service is
;        executed.
;
;    N.B. This function does return to its caller if a kernel stack
;         expansion is required and the attempted expansion fails.
;
;--

cPublicProc _KiCallUserMode, 2

; .FPO (3, 2, 4, 4, 0, 0)
; NOTE: .FPO is obsolete in MASM 14.x. Frame data is auto-generated.

;
; Save nonvolatile registers.
;

        PUSH    EBP                     ; save nonvolatile registers
        PUSH    EBX                     ;
        PUSH    ESI                     ;
        PUSH    EDI                     ;

        MOV     EBX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address

;
; Check if the current IRQL is above passive level.
;

IF DBG

        CurrentIrql                     ; get current IRQL
        OR      AL, AL                  ; check if IRQL is passive level
        JZ      SHORT Kcb00             ; if z, IRQL at passive level
        stdCall _KeBugCheck2, <IRQL_GT_ZERO_AT_SYSTEM_SERVICE, 0, EAX, 0, 0, 0>

;
; Check if kernel APCs are disabled or a process is attached.
;

Kcb00:  MOVZX   EAX, BYTE PTR [EBX+ThApcStateIndex] ; get APC state index
        MOV     EDX, [EBX+ThCombinedApcDisable]     ; get kernel APC disable
        OR      EAX, EAX                ; check if process attached
        JNE     SHORT Kcb05             ; if ne, process is attached
        OR      EDX, EDX                ; check if kernel APCs disabled
        JZ      SHORT Kcb07             ; if z, kernel APCs enabled
Kcb05:  stdCall _KeBugCheck2, <APC_INDEX_MISMATCH, 0, EAX, EDX, 0, 0>

ENDIF

;
; Check if sufficient room is available on the kernel stack for another
; system call.
;

Kcb07:  LEA     EAX, [ESP]-KERNEL_LARGE_STACK_COMMIT ; compute bottom address
        CMP     EAX, [EBX+ThStackLimit] ; check if limit exceeded
        JAE     SHORT Kcb10             ; if ae, limit not exceeded
        stdCall _MmGrowKernelStack, <ESP> ; attempt to grow kernel stack
        OR      EAX, EAX                ; check for successful completion
        JNE     Kcb20                   ; if ne, attempt to grow failed

;
; Get the address of the current thread and save the previous trap frame
; and callback stack addresses in the current frame. Also save the new
; callback stack address in the thread object.
;

Kcb10:  PUSH    [EBX+ThCallbackStack]   ; save callback stack address
        MOV     EDX, [EBX+ThTrapFrame]  ; get current trap frame address
        PUSH    EDX                     ; save trap frame address
        MOV     ESI, [EBX+ThInitialStack] ; get initial stack address
        PUSH    ESI                     ; save initial stack address
        MOV     [EBX+ThCallbackStack], ESP ; save callback stack address

KcbPrologEnd: ; help for the debugger

;
; Copy the numeric save area from the previous save area to the new save
; area and establish a new initial kernel stack.
;
; Make sure that the destination NPX Save area is 16-byte aligned
; as required by fxsave\fxrstor
;

        AND     ESP, 0FFFFFFF0h         ;
        MOV     EDI, ESP                ; set new initial stack address
        SUB     ESP, NPX_FRAME_LENGTH   ; compute destination NPX save area
        SUB     ESI, NPX_FRAME_LENGTH   ; compute source NPX save area
        CLI                             ; disable interrupts
        MOV     ECX, [ESI+FpControlWord] ; copy NPX state to new frame
        MOV     [ESP+FpControlWord], ECX ;
        MOV     ECX, [ESI+FpStatusWord]  ;
        MOV     [ESP+FpStatusWord], ECX  ;
        MOV     ECX, [ESI+FpTagWord]     ;
        MOV     [ESP+FpTagWord], ECX     ;
        MOV     ECX, [ESI+FxMXCsr]       ;
        MOV     [ESP+FxMXCsr], ECX       ;
        MOV     ECX, [ESI+FpCr0NpxState] ;
        MOV     [ESP+FpCr0NpxState], ECX ;
        MOV     ESI, PCR[PcTss]         ; get address of task switch segment
        MOV     [EBX+ThInitialStack], EDI ; reset initial stack address
        MOV     ECX, ESP                ;
        SUB     ESP, TsV86Gs-TsHardwareSegSs ; bias for missing V86 fields

        .ERRNZ (EFLAGS_V86_MASK AND 0FF00FFFFh)

        TEST    BYTE PTR [EDX+TsEFlags+2], EFLAGS_V86_MASK/010000h  ; is this a V86 frame?
        JNE     @F
        MOV     ECX, ESP                ; Use adjusted esp (normal case)
@@:     MOV     [ESI+TssEsp0], ECX      ; set kernel entry stack address

;
; Construct a trap frame to facilitate the transfer into user mode via
; the standard system call exit.
;

        SUB     ESP, TsHardwareSegSs+4  ; allocate trap frame
        MOV     EBP, ESP                ; set address of trap frame
        MOV     ECX, (TsHardwareSegSs-TsSegFs+4)/4 ; set repeat count
        LEA     EDI, [ESP+TsSegFs]      ; set destination address
        LEA     ESI, [EDX+TsSegFs]      ; set source address
        REP     MOVSD                   ; copy trap information

        MOV     EDI, [EDX+TsDr7]        ; Fetch control register
        TEST    EDI, (NOT DR7_RESERVED_MASK) ; Do we need to restore Debug reg?
        MOV     [ESP+TsDr7], EDI        ; Save away Dr7
        JNZ     SHORT Kcb18             ; Yes, go save them.

Kcb15:  MOV     EAX, _KeUserCallbackDispatcher ; st address of callback dispatcher
        MOV     [ESP+TsEip], EAX        ;
        MOV     EAX, PCR[PcExceptionList] ; get current exception list
        MOV     [ESP+TsExceptionList], EAX ; set previous exception list
        MOV     EAX, [EDX+TsPreviousPreviousMode] ; get previous mode
        MOV     [ESP+TsPreviousPreviousMode], EAX ; set previous mode
        STI                             ; enable interrupts

        SET_DEBUG_DATA                  ; set system call debug data for exit

        JMP     _KiServiceExit          ; exit through service dispatch

Kcb18:
        MOV     ECX, (TsDr6-TsDr0+4)/4  ; set repeat count
        LEA     EDI, [ESP+TsDr0]        ; set destination address
        LEA     ESI, [EDX+TsDr0]        ; set source address
        REP     MOVSD                   ; copy trap information
        JMP     SHORT Kcb15

;
; An attempt to grow the kernel stack failed.
;

Kcb20:  POP     EDI                     ; restore nonvolatile register
        POP     ESI                     ;
        POP     EBX                     ;
        POP     EBP                     ;
        stdRET  _KiCallUserMode

stdENDP _KiCallUserMode

        PAGE ,132
        SUBTTL  "Switch Kernel Stack"
;++
;
; PVOID
; KeSwitchKernelStack (
;    IN PVOID StackBase,
;    IN PVOID StackLimit
;    )
;
; Routine Description:
;
;    This function switches to the specified large kernel stack.
;
;    N.B. This function can ONLY be called when there are no variables
;        in the stack that refer to other variables in the stack, i.e.,
;        there are no pointers into the stack.
;
; Arguments:
;
;    StackBase (esp + 4) - Supplies a pointer to the base of the new kernel
;        stack.
;
;    StackLimit (esp + 8) - Supplies a pointer to the limit of the new kernel
;        stack.
;
; Return Value:
;
;    The old kernel stack is returned as the function value.
;
;--

SsStkBs EQU     4                       ; new kernel stack base address
SsStkLm EQU     8                       ; new kernel stack limit address

cPublicProc _KeSwitchKernelStack, 2

;
; Save the address of the new stack and copy the old stack to the new
; stack.
;

        PUSH    ESI                     ; save string move registers
        PUSH    EDI                     ;
        MOV     EDX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address
        MOV     EDI, [ESP+SsStkBs+8]    ; get new kernel stack base address
        MOV     ECX, [EDX+ThStackBase]  ; get current stack base address
        SUB     EBP, ECX                ; relocate the callers frame pointer
        ADD     EBP, EDI                ;
        MOV     EAX, [EDX+ThTrapFrame]  ; relocate the current trap frame address
        SUB     EAX, ECX                ;
        ADD     EAX, EDI                ;
        MOV     [EDX+ThTrapFrame], EAX  ;
        SUB     ECX, ESP                ; compute length of copy
        SUB     EDI, ECX                ; set destination address of copy
        MOV     ESI, ESP                ; set source address of copy
        PUSH    EDI                     ; save new stack pointer address
        REP     MOVSB                   ; copy old stack to new stack
        POP     EDI                     ; restore new stack pointer address

;
; Switch to the new kernel stack and return the address of the old kernel
; stack.
;

        MOV     EAX, [EDX+ThStackBase]  ; get old kernel stack base address
        MOV     ECX, [ESP+SsStkBs+8]    ; get new kernel stack base address
        MOV     ESI, [ESP+SsStkLm+8]    ; get new kernel stack limit address
        CLI                             ; disable interrupts
        MOV     [EDX+ThStackBase], ECX  ; set new kernel stack base address
        MOV     [EDX+ThStackLimit], ESI ; set new kernel stack limit address
        MOV     BYTE PTR [EDX+ThLargeStack], 1 ; set large stack TRUE
        MOV     [EDX+ThInitialStack], ECX ; set new initial stack address
        MOV     ESI, [EDX+ThTrapFrame]  ; Get current trap frame address
        MOV     EDX, PCR[PcTss]         ; get address of task switch segment
        .ERRNZ (EFLAGS_V86_MASK AND 0FF00FFFFh)
        TEST    BYTE PTR [ESI+TsEFlags+2], EFLAGS_V86_MASK/010000h  ; is this a V86 frame?
        LEA     ECX, DWORD PTR [ECX-NPX_FRAME_LENGTH] ; compute NPX save area address
        JNE     @F
        SUB     ECX, TsV86Gs-TsHardwareSegSs ; bias for missing V86 fields
@@:     MOV     [EDX+TssEsp0], ECX      ; set kernel entry stack address
        MOV     ESP, EDI                ; set new stack pointer address
        STI                             ;
        POP     EDI                     ; restore string move registers
        POP     ESI                     ;
        stdRET  _KeSwitchKernelStack

stdENDP _KeSwitchKernelStack

        PAGE ,132
        SUBTTL  "Get User Mode Stack Address"
;++
;
; PULONG
; KiGetUserModeStackAddress (
;    VOID
;    )
;
; Routine Description:
;
;    This function returns the address of the user stack address in the
;    current trap frame.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    The address of the user stack address.
;
;--

cPublicProc _KiGetUserModeStackAddress, 0

        MOV     EAX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address
        MOV     EAX, [EAX+ThTrapFrame]  ; get current trap frame address
        LEA     EAX, [EAX+TsHardwareEsp] ; get address of stack address
        stdRET  _KiGetUserModeStackAddress

stdENDP _KiGetUserModeStackAddress

        PAGE ,132
        SUBTTL  "Return from User Mode Callback"
;++
;
; NTSTATUS
; NtCallbackReturn (
;    IN PVOID OutputBuffer OPTIONAL,
;    IN ULONG OutputLength,
;    IN NTSTATUS Status
;    )
;
; Routine Description:
;
;    This function returns from a user mode callout to the kernel
;    mode caller of the user mode callback function.
;
;    N.B. This function returns to the function that called out to user
;        mode and the KiCallUserMode function calls out to user mode.
;        Therefore, the stack layout must be consistent between the
;        two routines.
;
; Arguments:
;
;    OutputBuffer - Supplies an optional pointer to an output buffer.
;
;    OutputLength - Supplies the length of the output buffer.
;
;    Status - Supplies the status value returned to the caller of the
;        callback function.
;
; Return Value:
;
;    If the callback return cannot be executed, then an error status is
;    returned. Otherwise, the specified callback status is returned to the
;    caller of the callback function.
;
;    N.B. This function returns to the function that called out to user
;         mode is a callout is currently active.
;
;--

cPublicProc _NtCallbackReturn, 3

        MOV     EAX, PCR[PcPrcbData+PbCurrentThread] ; get current thread address
        MOV     ECX, [EAX+ThCallbackStack] ; get callback stack address
        TEST    ECX, ECX
        JE      CbExit                  ; if zero, no callback stack present

;
; Restore the current exception list from the saved exception list in the
; current trap frame, restore the trap frame and callback stack addresses,
; store the output buffer address and length, and set the service status.
;

        MOV     EBX, [EAX+ThTrapFrame]  ; get current trap frame address
        MOV     EDX, [EBX+TsExceptionList] ; get saved exception list address
        MOV     PCR[PcExceptionList], EDX ; restore exception list address
        MOV     EDI, [ESP+4]            ; get output buffer address
        MOV     ESI, [ESP+8]            ; get output buffer length
        MOV     EBP, [ESP+12]           ; get callout service status
        MOV     EBX, [ECX+CuOutBf]      ; get address to store output buffer
        MOV     [EBX], EDI              ; store output buffer address
        MOV     EBX, [ECX+CuOutLn]      ; get address to store output length
        MOV     [EBX], ESI              ; store output buffer length
        MOV     EBX, [ECX]              ; get previous initial stack address
        CLI                             ; disable interrupt
        MOV     ESI, [EAX+ThInitialStack] ; get source NPX save area address
        MOV     [EAX+ThInitialStack], EBX ; restore initial stack address
        SUB     ESI, NPX_FRAME_LENGTH   ; compute source NPX save area
        SUB     EBX, NPX_FRAME_LENGTH   ; compute destination NPX save area
        MOV     EDX, [ESI+FpControlWord] ; copy NPX state to previous frame
        MOV     [EBX+FpControlWord], EDX ;
        MOV     EDX, [ESI+FpStatusWord]  ;
        MOV     [EBX+FpStatusWord], EDX  ;
        MOV     EDX, [ESI+FpTagWord]     ;
        MOV     [EBX+FpTagWord], EDX     ;
        MOV     EDX, [ESI+FxMXCsr]       ;
        MOV     [EBX+FxMXCsr], EDX       ;
        MOV     EDX, [ESI+FpCr0NpxState] ;
        MOV     [EBX+FpCr0NpxState], EDX ;
        CMP     EBP, STATUS_CALLBACK_POP_STACK
        MOV     EDI, [ECX+CuTrFr]       ; Fetch address of previous trap frame
        JZ      SHORT _NtCbCopyTrapFrame
_NtCbCopyTrapFrameRet:
        AND     DWORD PTR [EDI+TsDr7], 0 ; Assume original trap frame has no debug registers active
        TEST    BYTE PTR [EAX+ThDebugActive], 0FFh
        JNE     SHORT _NtCbGetDebugRegs
_NtCbGetDebugRet:
        MOV     EDX, PCR[PcTss]         ; Get address of task switch segment
        LEA     ESP, [ECX+CuCbStk]      ; Trim stack back to callback frame
        .ERRNZ (EFLAGS_V86_MASK AND 0FF00FFFFh)
        TEST    BYTE PTR [EDI+TsEFlags+2], EFLAGS_V86_MASK/010000h  ; is this a V86 frame?
        JNE     @F
        SUB     EBX, TsV86Gs-TsHardwareSegSs ; bias for missing V86 fields
@@:
        MOV     [EDX+TssEsp0], EBX      ; restore kernel entry stack address
        MOV     [EAX+ThTrapFrame], EDI  ; restore current trap frame address
        STI                             ; enable interrupts
        POP     [EAX+ThCallbackStack]   ; restore callback stack address
        MOV     EAX, EBP                ; set callback service status

;
; Restore nonvolatile registers, clean call parameters from stack, and
; return to callback caller.
;

        POP     EDI                     ; restore nonvolatile registers
        POP     ESI                     ;
        POP     EBX                     ;
        POP     EBP                     ;
        POP     EDX                     ; save return address
        ADD     ESP, 8                  ; remove parameters from stack
        JMP     EDX                     ; return to callback caller

;
; edi - previous trap frame [in/out]
; ecx - callback stack [in/out]
; eax - current thread [in/out]
; esi/edx: available
;

_NtCbCopyTrapFrame:
        MOV     ECX, (TsV86Es-TsSegFs)/4
        MOV     ESI, [EAX+ThTrapFrame]  ; trap frame for this system call
        .ERRNZ (EFLAGS_V86_MASK AND 0FF00FFFFh)
        TEST    BYTE PTR [ESI+TsEFlags+2], EFLAGS_V86_MASK/010000h
        MOV     EDX, EDI
        LEA     EDI, [EDI+TsSegFs]
        JZ      SHORT @F
        ADD     ECX, (KTRAP_FRAME_LENGTH-TsV86Es)/4
@@:     LEA     ESI, [ESI+TsSegFs]
        REP     MOVSD                   ; copy the relevant parts of the frame
        MOV     ECX, [EAX+ThCallbackStack]
        MOV     EDI, EDX                ; restores ecx, edi
        JMP     _NtCbCopyTrapFrameRet

_NtCbGetDebugRegs:

;
; Copy debug registers from current frame to original frame.
;
        MOV     ESI, [EAX+ThTrapFrame]  ; Get current trap frame
        MOV     EDX, [ESI+TsDr0]
        MOV     [EDI+TsDr0], EDX
        MOV     EDX, [ESI+TsDr1]
        MOV     [EDI+TsDr1], EDX
        MOV     EDX, [ESI+TsDr2]
        MOV     [EDI+TsDr2], EDX
        MOV     EDX, [ESI+TsDr3]
        MOV     [EDI+TsDr3], EDX
        MOV     EDX, [ESI+TsDr6]
        MOV     [EDI+TsDr6], EDX
        MOV     EDX, [ESI+TsDr7]
        MOV     [EDI+TsDr7], EDX
        JMP     _NtCbGetDebugRet

;
; No callback is currently active.
;

CbExit: MOV     EAX, STATUS_NO_CALLBACK_ACTIVE ; set service status
        stdRET  _NtCallbackReturn

stdENDP _NtCallbackReturn

_TEXT   ENDS
        END
