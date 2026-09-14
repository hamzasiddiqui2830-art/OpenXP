        title   "Context Swap"
;++
;
; Copyright (c) OpenXP.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    ctxswap.asm
;
; Abstract:
;
;    This module implements the code necessary to field the dispatch
;    interrupt and to perform kernel initiated context switching.
;
;--

.586p
        .xlist
include ks386.inc
include i386\kimacro.inc
include mac386.inc
include callconv.inc
include irqli386.inc
        .list

; Fastcall imports - explicit declarations
        EXTRN   @KefAcquireSpinLockAtDpcLevel@4:NEAR
        EXTRN   @KefReleaseSpinLockFromDpcLevel@4:NEAR
        EXTRN   @HalClearSoftwareInterrupt@4:NEAR
        EXTRN   @HalRequestSoftwareInterrupt@4:NEAR
        EXTRN   @KiCheckForSListAddress@4:NEAR
        EXTRN   @KiQueueReadyThread@8:NEAR
        EXTRN   @KiRetireDpcList@4:NEAR
        EXTRN   @WmiTraceContextSwap@8:NEAR

IFNDEF NT_UP
        EXTRN   @KiIdleSchedule@4:NEAR
ENDIF

        EXTRNP  _KiQuantumEnd, 0
        EXTRNP  _KeBugCheckEx, 5

        EXTRN   _KiTrap13:PROC
        EXTRN   _KeFeatureBits:DWORD

        EXTRN   __imp__KeRaiseIrqlToSynchLevel@0:DWORD

        EXTRN   _KiIdleSummary:DWORD

IF DBG
        EXTRN   _KdDebuggerEnabled:BYTE
        EXTRNP  _DbgBreakPoint, 0
        EXTRNP  _KdPollBreakIn, 0
        EXTRN   _DbgPrint:NEAR
        EXTRN   _MsgDpcTrashedEsp:BYTE
        EXTRN   _MsgDpcTimeout:BYTE
        EXTRN   _KiDPCTimeout:DWORD
ENDIF


_TEXT$00   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
; KiRDTSC - fastcall, 1 arg (ECX)
;--
PUBLIC @KiRDTSC@4
@KiRDTSC@4 proc
        rdtsc                   ; read the timestamp counter
        mov     [ecx], eax      ; return the low 32 bits
        mov     [ecx+4], edx    ; return the high 32 bits
        ret
@KiRDTSC@4 endp

        PAGE ,132
        SUBTTL  "Swap Context"
;++
; KiSwapContext - fastcall, 2 args (ECX=OldThread, EDX=NewThread)
;--

PUBLIC @KiSwapContext@8
@KiSwapContext@8 proc

;
; N.B. The following registers MUST be saved such that ebp is saved last.
;

        sub     esp, 4*4
        mov     [esp+12], ebx           ; save registers
        mov     [esp+8], esi            ;
        mov     [esp+4], edi            ;
        mov     [esp+0], ebp            ;
        mov     ebx, PCR[PcSelfPcr]     ; set address of PCR
        mov     edi, ecx                ; set old thread address
        mov     esi, edx                ; set next thread address
        movzx   ecx, byte ptr [edi+ThWaitIrql] ; set APC interrupt bypass disable

        call    SwapContext             ; swap context
        mov     ebp, [esp+0]            ; restore registers
        mov     edi, [esp+4]            ;
        mov     esi, [esp+8]            ;
        mov     ebx, [esp+12]           ;
        add     esp, 4*4                ;
        ret

@KiSwapContext@8 endp

        PAGE ,132
        SUBTTL  "Dispatch Interrupt"
;++
; _KiDispatchInterrupt
;--

        ALIGN 16
cPublicProc _KiDispatchInterrupt, 0

;
; Check if an SLIST pop operation is being interrupted
;
        test    ecx, ecx                ; check for NULL trap frame
        jz      short @F                ; if z, trap frame NULL
        call    @KiCheckForSListAddress@4 ; check SLIST addresses
@@:

;
; Disable interrupts and check DPC list
;

        mov     ebx, PCR[PcSelfPcr]     ; get address of PCR
kdi00:  cli                             ; disable interrupts
        mov     eax, [ebx+PcPrcbData+PbDpcQueueDepth] ; get DPC queue depth
        or      eax, [ebx+PcPrcbData+PbTimerRequest] ; merge timer request

IFNDEF NT_UP
        or      eax, DWORD PTR [ebx+PcPrcbData+PbDeferredReadyListHead] ; merge deferred list head
ENDIF

        jz      short kdi40             ; if z, no DPC's or timers to process
        push    ebp                     ; save register

;
; Terminate the exception list.
;

        push    [ebx+PcExceptionList]
        mov     [ebx+PcExceptionList], EXCEPTION_CHAIN_END

;
; Switch to the DPC stack
;

        mov     edx, esp
        mov     esp, [ebx+PcPrcbData+PbDpcStack]
        push    edx

        mov     ecx, [ebx+PcPrcb]       ; get current PRCB address
        call    @KiRetireDpcList@4      ; process the current DPC list

;
; Switch back to the current thread stack
;

        pop     esp
        pop     [ebx+PcExceptionList]
        pop     ebp

;
; Check quantum end
;

kdi40:  sti                             ; enable interrupts
        cmp     byte ptr [ebx+PcPrcbData+PbQuantumEnd], 0 ; quantum end requested
        jne     kdi90                   ; if neq, quantum end request

;
; Check if a new thread has been selected
;

        cmp     dword ptr [ebx+PcPrcbData+PbNextThread], 0 ; check if next thread
        je      kdi70                   ; if eq, then no new thread

        sub     esp, 3*4
        mov     [esp+8], esi            ; save registers
        mov     [esp+4], edi            ;
        mov     [esp+0], ebp            ;
        mov     edi, [ebx+PcPrcbData+PbCurrentThread] ; get current thread

;
; Raise IRQL to SYNCH level and acquire PRCB lock
;

IFNDEF NT_UP

        call    dword ptr [__imp__KeRaiseIrqlToSynchLevel@0] ; raise IRQL
        mov     byte ptr [edi+ThSwapBusy], 1 ; set context swap busy
        lea     ecx, [ebx+PcPrcbData+PbPrcbLock] ; get PRCB lock address
   lock bts     dword ptr [ecx], 0      ; try to acquire PRCB lock
        jnc     short kdi50             ; if nc, PRCB lock acquired
        call    @KefAcquireSpinLockAtDpcLevel@4 ; acquire current PRCB lock

ENDIF

;
; Swap to next thread
;

kdi50:  mov     esi, [ebx+PcPrcbData+PbNextThread] ; get next thread
        and     dword ptr [ebx+PcPrcbData+PbNextThread], 0 ; clear next thread
        mov     [ebx+PcPrcbData+PbCurrentThread], esi ; set current thread
        mov     byte ptr [esi+ThState], Running ; set thread state
        mov     byte ptr [edi+ThWaitReason], WrDispatchInt ; set wait reason
        mov     ecx, edi                ; set address of current thread
        lea     edx, [ebx+PcPrcbData]   ; set address of PRCB
        call    @KiQueueReadyThread@8   ; ready thread for execution
        mov     cl, APC_LEVEL           ; set APC interrupt bypass disable
        call    SwapContext             ; swap context
        mov     ebp, [esp+0]            ; restore registers
        mov     edi, [esp+4]            ;
        mov     esi, [esp+8]            ;
        add     esp, 3*4
kdi70:  stdRET  _KiDispatchInterrupt    ; return

;
; Process quantum end
;

kdi90:  mov     byte ptr [ebx+PcPrcbData+PbQuantumEnd], 0 ; clear quantum end
        stdCall _KiQuantumEnd           ; process quantum end
        stdRET  _KiDispatchInterrupt    ; return

stdENDP _KiDispatchInterrupt

        PAGE ,132
        SUBTTL  "Swap Context to Next Thread"
;++
; SwapContext - internal procedure (not PROC/ENDP to allow public labels inside)
;--

IFNDEF NT_UP
        PUBLIC  _ScPatchFxb
        PUBLIC  _ScPatchFxe
ENDIF

        PUBLIC  SwapContext

        ALIGN 16

; Note: SwapContext is NOT declared as PROC/ENDP to allow _ScPatchFxb/_ScPatchFxe
; labels to be PUBLIC (MASM 14.x doesn't allow PUBLIC on labels inside PROC)

SwapContext:

;
; Save the APC disable flag.
;
        push    ecx                     ; save APC bypass disable

;
; Wait for context to be swapped for the target thread.
;

IFNDEF NT_UP

sc00:   cmp     byte ptr [esi+ThSwapBusy], 0 ; check if context swap busy
        je      short sc01              ; if e, context swap idle
        YIELD                           ; yield execution for SMT system
        jmp     short sc00              ;

ENDIF

;
; Increment context switch count
;

sc01:   inc     es:dword ptr [ebx+PcContextSwitches] ; processor count

;
; Save the thread exception list head.
;

        push    [ebx+PcExceptionList]   ; save thread exception list head

;
; Check for context swap logging.
;

        cmp     [ebx+PcPerfGlobalGroupMask], 0 ; check if logging enable
        jne     sc92                    ; If not, then check if we are enabled
sc03:

IFNDEF NT_UP
IF DBG

        mov     cl, [esi+ThNextProcessor] ; get current processor number
        cmp     cl, [ebx+PcPrcbData+PbNumber] ; same as running processor?
        jne     sc_error2               ; if ne, processor number mismatch

ENDIF
ENDIF

;
; NPX state handling
;

        mov     ebp, cr0                ; get current CR0
        mov     edx, ebp                ;

IFNDEF NT_UP

        cmp     byte ptr [edi+ThNpxState], NPX_STATE_LOADED ; check if NPX state
        je      sc_save_npx_state       ; if e, NPX state not loaded

ENDIF

;
; Save the old stack pointer and compute the new stack limits.
;

sc05:   mov     [edi+ThKernelStack], esp ; save old kernel stack pointer
        mov     eax, [esi+ThInitialStack] ; get new initial stack pointer

.errnz (NPX_STATE_NOT_LOADED - CR0_TS - CR0_MP)
.errnz (NPX_STATE_LOADED - 0)

IFDEF NT_UP
        cli
ENDIF
        movzx   ecx, byte ptr [esi+ThNpxState] ; new NPX state
        and     edx, NOT (CR0_MP+CR0_EM+CR0_TS) ; clear thread settable NPX bits
        or      ecx, edx                ; or in new thread's cr0
        or      ecx, [eax+FpCr0NpxState-NPX_FRAME_LENGTH] ; merge new thread state
        cmp     ebp, ecx                ; check if old and new CR0 match
        jne     sc_reload_cr0           ; if ne, change in CR0
sc06:

IFDEF NT_UP
        sti
ENDIF

IF DBG
        mov     eax, [esi+ThKernelStack] ; set new stack pointer
        cmp     esi, dword ptr [eax-4]
        je      @F
        int     3
@@:
        xchg    esp, eax
        mov     [eax-4], edi
ELSE
        mov     esp, [esi+ThKernelStack] ; set new stack pointer
ENDIF

;
; Check if the old process is the same as the new process.
;

        mov     ebp, [esi+ThApcState+AsProcess] ; get new process address
        mov     eax, [edi+ThApcState+AsProcess] ; get old process address
        cmp     ebp, eax                        ; check if process match
        jz      short sc23                      ; if z, process match

;
; Set the processor bit in the new process and clear the old.
;

IFNDEF NT_UP

        mov     ecx, [ebx+PcSetMemberCopy] ; get processor set member
   lock xor     [ebp+PrActiveProcessors], ecx ; set bit in new processor set
   lock xor     [eax+PrActiveProcessors], ecx ; clear bit in old processor set

IF DBG

        test    [ebp+PrActiveProcessors], ecx ; test if bit set in new set
        jz      sc_error5               ; if z, bit not set in new set
        test    [eax+PrActiveProcessors], ecx ; test if bit clear in old set
        jnz     sc_error4               ; if nz, bit not clear in old set
ENDIF

ENDIF

;
; LDT switch
;

        mov     ecx, [ebp+PrLdtDescriptor]
        or      ecx, [eax+PrLdtDescriptor]
        jnz     sc_load_ldt             ; if nz, LDT limit
sc_load_ldt_ret:

;
; Load the new CR3
;

        mov     eax, [ebp+PrDirectoryTableBase] ; get new directory base
        mov     cr3, eax                ; and flush TB

;
; Set context swap idle for the old thread.
;

sc23:

IFNDEF NT_UP

        and     byte ptr [edi+ThSwapBusy], 0 ; clear old thread swap busy

ENDIF

        xor     eax, eax
        mov     gs, eax

;
; Set the TEB descriptor
;

        mov     eax, [esi+ThTeb]        ; get user TEB address
        mov     [ebx+PcTeb], eax        ; set user TEB address
        mov     ecx, [ebx+PcGdt]        ; get GDT address
        mov     [ecx+KGDT_R3_TEB+KgdtBaseLow], ax ;
        shr     eax, 16                 ;
        mov     [ecx+KGDT_R3_TEB+KgdtBaseMid], al ;
        mov     [ecx+KGDT_R3_TEB+KgdtBaseHi], ah ;

;
; Adjust the initial stack address for TSS
;

        mov     eax, [esi+ThInitialStack] ; get initial stack address
        sub     eax, NPX_FRAME_LENGTH
.errnz (EFLAGS_V86_MASK AND 0FF00FFFFh)
        test    byte ptr [eax] - KTRAP_FRAME_LENGTH + TsEFlags + 2, EFLAGS_V86_MASK / 10000h
        jnz     short sc24              ; if nz, V86 frame
        sub     eax, TsV86Gs - TsHardwareSegSs ; bias for missing fields
sc24:   mov     ecx, [ebx+PcTssCopy]    ; get TSS address
        mov     [ecx+TssEsp0], eax      ; set initial kernel stack address

;
; Set the IOPM map offset value.
;

        mov     ax, [ebp+PrIopmOffset]  ; set IOPM offset
        mov     [ecx+TssIoMapBase], ax  ;

;
; Update context switch counters.
;

        inc     dword ptr [esi+ThContextSwitches] ; thread count

;
; Restore thread exception list head and get APC bypass disable.
;

        pop     [ebx+PcExceptionList]   ; restore thread exception list head
        pop     ecx                     ; get APC bypass disable

;
; Check if DPC routine active
;

        cmp     byte ptr [ebx+PcPrcbData+PbDpcRoutineActive], 0 ; check if DPC active
        jne     sc91                    ; bugcheck if DPC active.

;
; Check for kernel APC pending
;

        cmp     byte ptr [esi+ThApcState+AsKernelApcPending], 0 ; APC pending?
        jne     short sc80              ; if ne, kernel APC pending
        xor     eax, eax                ; set return value
        ret                             ; return

;
; Handle APC pending
;

sc80:   cmp     word ptr [esi+ThSpecialApcDisable], 0 ; check special APC disable
        jne     short sc90              ; if ne, special APC disable
        test    cl, cl                  ; test for APC bypass disable
        jz      short sc90              ; if z, APC bypass enabled
        mov     cl, APC_LEVEL           ; request software interrupt level
        call    @HalRequestSoftwareInterrupt@4 ;
        or      eax, esp                ; clear ZF flag
sc90:   setz    al                      ; set return value
        ret                             ; return

;
; Set for new LDT value
;

sc_load_ldt:
        mov     eax, [ebp+PrLdtDescriptor] ;
        test    eax, eax
        je      @F
        mov     ecx, [ebx+PcGdt]        ; get GDT address
        mov     [ecx+KGDT_LDT], eax     ;
        mov     eax, [ebp+PrLdtDescriptor+4] ;
        mov     [ecx+KGDT_LDT+4], eax   ;

        mov     ecx, [ebx+PcIdt]        ;
        mov     eax, [ebp+PrInt21Descriptor] ;
        mov     [ecx+21h*8], eax        ;
        mov     eax, [ebp+PrInt21Descriptor+4] ;
        mov     [ecx+21h*8+4], eax      ;
        mov     eax, KGDT_LDT
@@:     lldt    ax
        jmp     sc_load_ldt_ret

;
; Cr0 has changed, load the new value.
;

sc_reload_cr0:

IF DBG

        test    byte ptr [esi+ThNpxState], NOT (CR0_TS+CR0_MP)
        jnz     sc_error                ;
        test    dword ptr [eax+FpCr0NpxState-NPX_FRAME_LENGTH], NOT (CR0_PE+CR0_MP+CR0_EM+CR0_TS)
        jnz     sc_error3               ;

ENDIF

        mov     cr0, ecx                ; set new CR0 NPX state
        jmp     sc06

;
; Save coprocessor's current context.
;

IFNDEF NT_UP

sc_save_npx_state:
        and     edx, NOT (CR0_MP+CR0_EM+CR0_TS) ; we need access to the NPX state

        mov     ecx, [edi+ThInitialStack] ; get NPX save area address
        sub     ecx, NPX_FRAME_LENGTH

        cmp     ebp, edx                ; Does CR0 need reloading?
        je      short sc_npx10

        mov     cr0, edx                ; set new cr0
        mov     ebp, edx                ; (ebp) = (edx) = current cr0 state

sc_npx10:

_ScPatchFxb:
;       fxsave  [ecx]                   ; save NPX state
        db      0FH, 0AEH, 01
_ScPatchFxe:

        mov     byte ptr [edi+ThNpxState], NPX_STATE_NOT_LOADED ; set no NPX state
        mov     dword ptr [ebx+PcPrcbData+PbNpxThread], 0  ; clear npx owner
        jmp     sc05
ENDIF

;
; Context swap logging (out of line)
;

sc92:   mov     eax, [ebx+PcPerfGlobalGroupMask] ; Load the ptr into eax
        cmp     eax, 0                  ; catch race condition
        jz      sc03                    ; instead of above in mainline code
        mov     edx, esi                ; pass the new ETHREAD object
        mov     ecx, edi                ; pass the old ETHREAD object
        test    dword ptr [eax+PERF_CONTEXTSWAP_OFFSET], PERF_CONTEXTSWAP_FLAG
        jz      sc03                    ; return if our flag is not set

        call    @WmiTraceContextSwap@8  ; call the Wmi context swap trace
        jmp     sc03                    ;

;
; Bugcheck - context switch while in DPC
;

sc91:
        mov     eax, [edi+ThInitialStack] ; get the old stack
        stdCall _KeBugCheckEx, <ATTEMPTED_SWITCH_FROM_DPC, edi, esi, eax, 0>
        ret                             ; return

IF DBG
sc_error5:  int 3
sc_error4:  int 3
sc_error3:  int 3
sc_error2:  int 3
sc_error:   int 3
ENDIF

; Note: No "endp" for SwapContext since it's not declared as PROC

        PAGE ,132
        SUBTTL  "Flush Entire Translation Buffer"
;++
; _KeFlushCurrentTb
; Note: Not declared as PROC/ENDP to allow ktb00/ktb_gb/ktb_eb to be PUBLIC
;--

        PUBLIC  ktb00
        PUBLIC  ktb_gb
        PUBLIC  ktb_eb

; Note: _KeFlushCurrentTb is NOT declared as PROC/ENDP to allow ktb00/ktb_gb/ktb_eb
; labels to be PUBLIC (MASM 14.x doesn't allow PUBLIC on labels inside PROC)

_KeFlushCurrentTb:

ktb00:  mov     eax, cr3                ; (eax) = directory table base
        mov     cr3, eax                ; flush TLB
        stdRET    _KeFlushCurrentTb

ktb_gb: mov     eax, cr4                ; *** see Ki386EnableGlobalPage ***
        and     eax, not CR4_PGE        ; This version gets copied into
        mov     cr4, eax                ; ktb00 at initialization time if needed.
        or      eax, CR4_PGE
        mov     cr4, eax
ktb_eb: stdRET    _KeFlushCurrentTb

; Note: No "endp" since not declared as PROC

        PAGE ,132
        SUBTTL  "Flush Data Cache"
;++
; _KiFlushDcache / _KiFlushIcache - no-op on i386/i486
;--

cPublicProc _KiFlushDcache, 0
cPublicProc _KiFlushIcache, 0

        stdRET    _KiFlushIcache

stdENDP _KiFlushIcache
stdENDP _KiFlushDcache


_TEXT$00   ends

INIT    SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
; _Ki386EnableGlobalPage
;--

cPublicProc _Ki386EnableGlobalPage, 1
        push    esi
        push    edi
        push    ebx

        mov     edx, [esp+16]           ; pointer to Number
        pushfd
        cli

;
; Wait for all processors
;
        lock dec dword ptr [edx]        ; count down
egp10:  YIELD
        cmp     dword ptr [edx], 0      ; wait for all processors
        jnz     short egp10

        cmp     byte ptr PCR[PcNumber], 0 ; processor 0?
        jne     short egp20

;
; Install proper KeFlushCurrentTb function.
;

        mov     edi, ktb00
        mov     esi, ktb_gb
        mov     ecx, ktb_eb - ktb_gb + 1
        rep movsb

        mov     byte ptr [ktb_eb], 0

;
; Wait for P0 to signal
;
egp20:  cmp     byte ptr [ktb_eb], 0
        jnz     short egp20

;
; Flush TB and enable global page support
;
        mov     eax, cr4
        and     eax, not CR4_PGE
        mov     ecx, cr3
        mov     cr4, eax

        mov     cr3, ecx                ; Flush TB

        or      eax, CR4_PGE            ; enable global TBs
        mov     cr4, eax
        popfd
        pop     ebx
        pop     edi
        pop     esi

        stdRET  _Ki386EnableGlobalPage
stdENDP _Ki386EnableGlobalPage

;++
; _Ki386EnableDE
;--

cPublicProc _Ki386EnableDE, 1

        mov     eax, cr4
        or      eax, CR4_DE
        mov     cr4, eax

        stdRET  _Ki386EnableDE
stdENDP _Ki386EnableDE

;++
; _Ki386EnableFxsr
;--

cPublicProc _Ki386EnableFxsr, 1

        mov     eax, cr4
        or      eax, CR4_FXSR
        mov     cr4, eax

        stdRET  _Ki386EnableFxsr
stdENDP _Ki386EnableFxsr

;++
; _Ki386EnableXMMIExceptions
;--

cPublicProc _Ki386EnableXMMIExceptions, 1

        ;Set up IDT for INT19
        mov     ecx, PCR[PcIdt]         ;Get IDT address
        lea     eax, [ecx] + 098h       ;XMMI exception is int 19
        mov     byte ptr [eax + 5], 08eh ;P=1,DPL=0,Type=e
        mov     word ptr [eax + 2], KGDT_R0_CODE ;Kernel code selector
        mov     edx, offset FLAT:_KiTrap13 ;Address of int 19 handler
        mov     ecx, edx
        mov     word ptr [eax], cx      ;addr moves into low byte
        shr     ecx, 16
        mov     word ptr [eax + 6], cx  ;addr moves into high byte
        ;Enable XMMI exception handling
        mov     eax, cr4
        or      eax, CR4_XMMEXCPT
        mov     cr4, eax

        stdRET  _Ki386EnableXMMIExceptions
stdENDP _Ki386EnableXMMIExceptions

;++
; _Ki386EnableCurrentLargePage
; Note: Not declared as PROC/ENDP to allow _Ki386EnableCurrentLargePageEnd to be PUBLIC
;--

        PUBLIC  _Ki386EnableCurrentLargePageEnd

; Note: _Ki386EnableCurrentLargePage is NOT declared as PROC/ENDP to allow
; _Ki386EnableCurrentLargePageEnd label to be PUBLIC

_Ki386EnableCurrentLargePage:
        mov     ecx, [esp+4]            ; (ecx)-> IdentityAddr
        mov     edx, [esp+8]            ; (edx)-> IdentityCr3
        pushfd                          ; save current IF state
        cli                             ; disable interrupts

        mov     eax, cr3                ; (eax)-> original Cr3
        mov     cr3, edx                ; load Cr3 with Identity mapping

        sub     ecx, offset _Ki386EnableCurrentLargePage
        add     ecx, offset _Ki386LargePageIdentityLabel
        jmp     ecx                     ; jump to (linear == physical)

_Ki386LargePageIdentityLabel:
        mov     ecx, cr0
        and     ecx, NOT CR0_PG         ; clear PG bit to disable paging
        mov     cr0, ecx                ; disable paging
        jmp     $+2
        mov     edx, cr4
        or      edx, CR4_PSE            ; enable Page Size Extensions
        mov     cr4, edx
        mov     edx, offset OriginalMapping
        or      ecx, CR0_PG             ; set PG bit to enable paging
        mov     cr0, ecx                ; enable paging
        jmp     edx                     ; Return to original mapping.

OriginalMapping:
        mov     cr3, eax                ; restore original Cr3
        popfd                           ; restore interrupts

        stdRET  _Ki386EnableCurrentLargePage

_Ki386EnableCurrentLargePageEnd:

; Note: No "endp" since not declared as PROC

INIT    ends

_TEXT$00   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        PAGE ,132
        SUBTTL  "Swap Process"
;++
; _KiSwapProcess
;--

cPublicProc _KiSwapProcess, 2

        mov     edx, [esp+4]            ; (edx)-> New process
        mov     eax, [esp+8]            ; (eax)-> Old Process

;
; Set the processor number in the new process and clear it in the old.
;

IFNDEF NT_UP

        mov     ecx, PCR[PcSetMember]
   lock xor     [edx+PrActiveProcessors], ecx ; set bit in new processor set
   lock xor     [eax+PrActiveProcessors], ecx ; clear bit in old processor set

IF DBG

        test    [edx+PrActiveProcessors], ecx ; test if bit set in new set
        jz      kisp_error1             ; if z, bit not set in new set
        test    [eax+PrActiveProcessors], ecx ; test if bit clear in old set
        jnz     kisp_error              ; if nz, bit not clear in old set

ENDIF

ENDIF

;
; Change LDT
;

        mov     ecx, [edx+PrLdtDescriptor]
        or      ecx, [eax+PrLdtDescriptor]
        jnz     kisp_load_ldt           ; if nz, LDT limit
kisp_load_ldt_ret:

;
; Load the new CR3
;

        mov     eax, [edx+PrDirectoryTableBase]
        mov     cr3, eax

        mov     ecx, PCR[PcTssCopy]     ; (ecx)-> TSS

;
; Clear gs
;

        xor     eax, eax
        mov     gs, ax

;
; Change IOPM
;

        mov     ax, [edx+PrIopmOffset]
        mov     [ecx+TssIoMapBase], ax

        stdRET    _KiSwapProcess

kisp_load_ldt:

        mov     eax, [edx+PrLdtDescriptor]
        test    eax, eax
        je      @F
        mov     ecx, PCR[PcGdt]
        mov     [ecx+KGDT_LDT], eax
        mov     eax, [edx+PrLdtDescriptor+4]
        mov     [ecx+KGDT_LDT+4], eax

        mov     ecx, PCR[PcIdt]
        mov     eax, [edx+PrInt21Descriptor]
        mov     [ecx+21h*8], eax
        mov     eax, [edx+PrInt21Descriptor+4]
        mov     [ecx+21h*8+4], eax

        mov     eax, KGDT_LDT
@@:     lldt    ax
        jmp     kisp_load_ldt_ret

IF DBG
kisp_error1: int 3
kisp_error:  int 3
ENDIF

stdENDP _KiSwapProcess

        PAGE ,132
        SUBTTL  "Idle Loop"
;++
; KiIdleLoop - fastcall, 0 args
;--

PUBLIC @KiIdleLoop@0
@KiIdleLoop@0 proc

IF DBG

        xor     edi, edi                ; reset poll breakin counter

ENDIF

        jmp     short kid20             ; Skip HalIdleProcessor on first iteration

;
; Call HAL for power management
;

kid10:  lea     ecx, [ebx+PcPrcbData+PbPowerState]
        call    dword ptr [ecx+PpIdleFunction] ; (ecx) = Arg0

;
; Poll debugger
;

kid20:

IF DBG
IFNDEF NT_UP

        mov     eax, _KiIdleSummary     ; get idle summary
        mov     ecx, [ebx+PcSetMember]  ; get set member
        dec     ecx                     ; compute right bit mask
        and     eax, ecx                ; check if any lower bits set
        jnz     short CheckDpcList      ; if nz, not lowest numbered

ENDIF

        dec     edi                     ; decrement poll counter
        jg      short CheckDpcList      ; if g, not time to poll

        POLL_DEBUGGER                   ; check if break in requested
ENDIF

kid30:

IF DBG

IFNDEF NT_UP

        mov     edi, 20 * 1000          ; set breakin poll interval

ELSE

        mov     edi, 100                ; UP idle loop has a HLT in it

ENDIF

ENDIF

CheckDpcList0:
        YIELD

;
; Check DPC list
;

CheckDpcList:

        sti                             ; enable interrupts
        nop                             ;
        nop                             ;
        cli                             ; disable interrupts

;
; Process DPC list
;

        mov     eax, [ebx+PcPrcbData+PbDpcQueueDepth] ; get DPC queue depth
        or      eax, [ebx+PcPrcbData+PbTimerRequest] ; merge timer request

IFNDEF NT_UP

        or      eax, DWORD PTR [ebx+PcPrcbData+PbDeferredReadyListHead] ; merge deferred list

ENDIF

        jz      short CheckNextThread   ; if z, no DPC's or timers
        mov     cl, DISPATCH_LEVEL      ; set interrupt level
        call    @HalClearSoftwareInterrupt@4 ; clear software interrupt
        lea     ecx, [ebx+PcPrcbData]   ; set current PRCB address
        call    @KiRetireDpcList@4      ; process the current DPC list

IF DBG

        xor     edi, edi                ; clear breakin poll interval

ENDIF

;
; Check if a thread has been selected
;

CheckNextThread:
        cmp     dword ptr [ebx+PcPrcbData+PbNextThread], 0 ; thread selected?

IFDEF NT_UP

        je      short kid10             ; if eq, no thread selected

ELSE

        je      kid40                   ; if eq, no thread selected.

ENDIF

;
; Raise IRQL to synchronization level
;

IFNDEF NT_UP

        RaiseIrql SYNCH_LEVEL, noold    ; raise IRQL (lowercase 'noold' for IFIDNI)

ENDIF

        sti                             ; enable interrupts
        mov     edi, [ebx+PcPrcbData+PbCurrentThread] ; get idle thread

;
; Acquire PRCB lock
;

IFNDEF NT_UP

        mov     byte ptr [edi+ThSwapBusy], 1 ; set context swap busy
   lock bts     dword ptr [ebx+PcPrcbData+PbPrcbLock], 0 ; try to acquire PRCB Lock
        jnc     short kid33             ; if nc, PRCB lock acquired
        lea     ecx, [ebx+PcPrcbData+PbPrcbLock] ; get PRCB lock address
        call    @KefAcquireSpinLockAtDpcLevel@4 ; acquire current PRCB lock

ENDIF

;
; Get next thread
;

kid33:  mov     esi, [ebx+PcPrcbData+PbNextThread] ; get next thread

IFNDEF NT_UP

        cmp     esi, edi                ; check if idle thread
        je      short kisame            ; if e, processor idle again

ENDIF

        and     dword ptr [ebx+PcPrcbData+PbNextThread], 0 ; clear next thread
        mov     [ebx+PcPrcbData+PbCurrentThread], esi ; set new thread
        mov     byte ptr [esi+ThState], Running ; set thread state running

;
; Clear idle schedule and release PRCB lock
;

IFNDEF NT_UP

        and     byte ptr [ebx+PcPrcbData+PbIdleSchedule], 0 ; clear idle schedule
        and     DWORD PTR [ebx+PcPrcbData+PbPrcbLock], 0 ; release PRCB lock

ENDIF

kid35:

        mov     ecx, APC_LEVEL          ; set APC bypass disable
        call    SwapContext             ; swap context

IFNDEF NT_UP

        LowerIrql DISPATCH_LEVEL        ; lower IRQL to dispatch level

ENDIF

        jmp     kid30                   ;

;
; Idle thread same as old thread
;

IFNDEF NT_UP

kisame: and     dword ptr [ebx+PcPrcbData+PbNextThread], 0 ; clear next thread
        and     DWORD PTR [ebx+PcPrcbData+PbPrcbLock], 0 ; release PRCB lock
        and     byte ptr [edi+ThSwapBusy], 0 ; set idle thread context swap idle
        jmp     kid30                   ;

;
; Call idle schedule if requested.
;

kid40:  cmp     byte ptr [ebx+PcPrcbData+PbIdleSchedule], 0 ; check if idle schedule
        je      kid10                   ; if e, idle schedule not requested
        sti                             ; enable interrupts
        lea     ecx, [ebx+PcPrcbData]   ; get current PRCB address
        call    @KiIdleSchedule@4       ; attempt to schedule thread
        test    eax, eax                ; test if new thread schedule
        mov     esi, eax                ; set new thread address
        mov     edi, [ebx+PcPrcbData+PbIdleThread] ; get idle thread
        jnz     short kid35             ; if nz, new thread scheduled
        jmp     kid30                   ;

ENDIF

@KiIdleLoop@0 endp

IFDEF DBGMP
cPublicProc _KiPollDebugger, 0
        push    eax
        push    ecx
        push    edx
        POLL_DEBUGGER
        pop     edx
        pop     ecx
        pop     eax
        stdRET    _KiPollDebugger
stdENDP _KiPollDebugger

ENDIF

        PAGE ,132
        SUBTTL  "Adjust TSS ESP0 value"
;++
; _Ki386AdjustEsp0
;--

cPublicProc _Ki386AdjustEsp0, 1

IF DBG

        CurrentIrql
        cmp     al, APC_LEVEL
        jge     @F
        int     3
@@:

ENDIF

        mov     eax, PCR[PcPrcbData+PbCurrentThread] ; get current thread
        mov     edx, [esp+4]            ; edx -> trap frame
        mov     eax, [eax+ThInitialStack] ; eax = base of stack
        test    dword ptr [edx+TsEFlags], EFLAGS_V86_MASK ; is this a V86 frame?
        jnz     short ae10              ; if nz, V86 frame
        sub     eax, TsV86Gs - TsHardwareSegSS ; compensate for missing regs
ae10:   sub     eax, NPX_FRAME_LENGTH   ;
        pushfd                          ; Make sure we don't move
        cli                             ; processors while we do this
        mov     edx, PCR[PcTssCopy]     ;
        mov     [edx+TssEsp0], eax      ; set Esp0 value
        popfd                           ;

        stdRET    _Ki386AdjustEsp0

stdENDP _Ki386AdjustEsp0


_TEXT$00   ends

        end
