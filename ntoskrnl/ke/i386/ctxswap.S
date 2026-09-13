/*
 * Context Swap
 *
 * Copyright (c) OpenXP.
 *
 * Module Name:
 *
 *    ctxswap.S
 *
 * Abstract:
 *
 *    This module implements the code necessary to field the dispatch
 *    interrupt and to perform kernel initiated context switching.
 *
 *    GAS/ReactOS-style translation of the original ctxswap.asm.
 */

#include <asm.h>
#include <ks386.inc>
#include <i386/kimacro.inc>
#include <mac386.inc>
#include <callconv.inc>
#include <irqli386.inc>

        .intel_syntax noprefix

/*
 * MASM:
 *
 * FPOFRAME macro a, b
 * .FPO ( a, b, 0, 0, 0, 0 )
 * endm
 *
 * Keep as a no-op unless the OpenXP assembler framework supplies
 * an equivalent FPO directive.
 */
        .macro FPOFRAME a, b
        .endm


        .extern KefAcquireSpinLockAtDpcLevel
        .extern KefReleaseSpinLockFromDpcLevel

        .extern HalClearSoftwareInterrupt
        .extern HalRequestSoftwareInterrupt

#ifndef NT_UP
        .extern KiIdleSchedule
#endif

        .extern KiCheckForSListAddress
        .extern KiQueueReadyThread
        .extern KiRetireDpcList
        .extern _KiQuantumEnd
        .extern _KeBugCheckEx

        .extern _KiTrap13
        .extern _KeFeatureBits

        .extern __imp__KeRaiseIrqlToSynchLevel@0

        .extern _KiIdleSummary

        .extern WmiTraceContextSwap

#if DBG
        .extern _KdDebuggerEnabled
        .extern _DbgBreakPoint
        .extern _KdPollBreakIn
        .extern _DbgPrint
        .extern _MsgDpcTrashedEsp
        .extern _MsgDpcTimeout
        .extern _KiDPCTimeout
#endif


/*
 * -------------------------------------------------------------------------
 * Pageable/non-pageable kernel text.
 *
 * If your OpenXP linker uses different section names for the PE kernel
 * sections, change only these .section directives.
 * -------------------------------------------------------------------------
 */

        .section .text$00,"xr"
        .p2align 4


/*
 * =========================================================================
 *
 * KiRDTSC
 *
 * =========================================================================
 */

        .globl KiRDTSC
FUNC(KiRDTSC, 0)

        rdtsc
        mov     [ecx], eax
        mov     [ecx+4], edx

ENDPROC(KiRDTSC)


/*
 * =========================================================================
 *
 * Swap Context
 *
 * BOOLEAN
 * KiSwapContext(
 *     IN PKTHREAD OldThread,
 *     IN PKTHREAD NewThread
 *     )
 *
 * =========================================================================
 */

        .globl KiSwapContext
FUNC(KiSwapContext, 0)

        /*
         * The following registers MUST be saved such that ebp is
         * saved last.  This is required by the debugger.
         */

        sub     esp, 4*4

        mov     [esp+12], ebx
        mov     [esp+8],  esi
        mov     [esp+4],  edi
        mov     [esp+0],  ebp

        mov     ebx, PCR[PcSelfPcr]
        mov     edi, ecx
        mov     esi, edx

        movzx   ecx, byte ptr [edi+ThWaitirql]

        call    SwapContext

        mov     ebp, [esp+0]
        mov     edi, [esp+4]
        mov     esi, [esp+8]
        mov     ebx, [esp+12]

        add     esp, 4*4

        /*
         * FASTCALL function has no stack arguments to remove here.
         */
        ret

ENDPROC(KiSwapContext)


/*
 * =========================================================================
 *
 * KiDispatchInterrupt
 *
 * =========================================================================
 */

        .p2align 4

        .globl _KiDispatchInterrupt
FUNC(_KiDispatchInterrupt, 0)


        /*
         * Check if an SLIST pop operation is being interrupted
         * and reset EIP as necessary.
         *
         * ECX is already loaded.
         */

        test    ecx, ecx
        jz      .Lkdi_no_trap

        fstCall KiCheckForSListAddress

.Lkdi_no_trap:

        /*
         * Disable interrupts and check for DPC work.
         */

        mov     ebx, PCR[PcSelfPcr]

.Lkdi00:
        cli

        mov     eax, [ebx+PcPrcbData+PbDpcQueueDepth]
        or      eax, [ebx+PcPrcbData+PbTimerRequest]

#ifndef NT_UP
        or      eax, [ebx+PcPrcbData+PbDeferredReadyListHead]
#endif

        jz      .Lkdi40

        push    ebp

        /*
         * Exceptions occurring in DPCs are unrelated to the
         * interrupted thread.
         */

        push    [ebx+PcExceptionList]
        mov     [ebx+PcExceptionList], EXCEPTION_CHAIN_END

        /*
         * Switch to the DPC stack.
         */

        mov     edx, esp
        mov     esp, [ebx+PcPrcbData+PbDpcStack]
        push    edx

        mov     ecx, [ebx+PcPrcb]

        fstCall KiRetireDpcList

        /*
         * Restore thread stack, exception list and EBP.
         */

        pop     esp
        pop     [ebx+PcExceptionList]
        pop     ebp

.Lkdi40:
        sti

        cmp     byte ptr [ebx+PcPrcbData+PbQuantumEnd], 0
        jne     .Lkdi90

        /*
         * Check whether another thread has been selected.
         */

        cmp     dword ptr [ebx+PcPrcbData+PbNextThread], 0
        je      .Lkdi70

        /*
         * Save nonvolatile registers.
         */

        sub     esp, 3*4

        mov     [esp+8], esi
        mov     [esp+4], edi
        mov     [esp+0], ebp

        mov     edi, [ebx+PcPrcbData+PbCurrentThread]

#ifndef NT_UP

        /*
         * Raise IRQL to SYNCH and acquire PRCB lock.
         */

        call    dword ptr [__imp__KeRaiseIrqlToSynchLevel@0]

        mov     byte ptr [edi+ThSwapBusy], 1

        lea     ecx, [ebx+PcPrcbData+PbPrcbLock]

        lock bts dword ptr [ecx], 0
        jnc     .Lkdi50

        fstCall KefAcquireSpinLockAtDpcLevel

#endif

.Lkdi50:

        mov     esi, [ebx+PcPrcbData+PbNextThread]

        and     dword ptr [ebx+PcPrcbData+PbNextThread], 0

        mov     [ebx+PcPrcbData+PbCurrentThread], esi

        mov     byte ptr [esi+ThState], Running
        mov     byte ptr [edi+ThWaitReason], WrDispatchInt

        mov     ecx, edi
        lea     edx, [ebx+PcPrcbData]

        fstCall KiQueueReadyThread

        mov     cl, APC_LEVEL

        call    SwapContext

        mov     ebp, [esp+0]
        mov     edi, [esp+4]
        mov     esi, [esp+8]

        add     esp, 3*4

.Lkdi70:
        /*
         * stdRET _KiDispatchInterrupt
         *
         * The routine has no stack arguments.
         */
        ret


.Lkdi90:

        mov     byte ptr [ebx+PcPrcbData+PbQuantumEnd], 0

        /*
         * stdCall _KiQuantumEnd
         */
        call    _KiQuantumEnd

        ret


ENDPROC(_KiDispatchInterrupt)


/*
 * =========================================================================
 *
 * SwapContext
 *
 * Internal context-switch routine.
 *
 * Arguments:
 *
 *   CL  - APC interrupt bypass disable
 *   EDI - OldThread
 *   ESI - NewThread
 *   EBX - PCR
 *
 * Returns:
 *
 *   AL  - Kernel APC pending
 *   EBX - PCR
 *   ESI - current thread
 *
 * =========================================================================
 */

#ifndef NT_UP
        .globl _ScPatchFxb
        .globl _ScPatchFxe
#endif

        .globl SwapContext
        .p2align 4

SwapContext:

        /*
         * Save the APC disable flag.
         */

        push    ecx

#ifndef NT_UP

.Lsc00:
        cmp     byte ptr [esi+ThSwapBusy], 0
        je      .Lsc01

        YIELD
        jmp     .Lsc00

#endif

.Lsc01:

        /*
         * Increment processor context switch count.
         */

        inc     dword ptr es:[ebx+PcContextSwitches]

        /*
         * Save exception list head.
         */

        push    [ebx+PcExceptionList]

        /*
         * Context swap logging.
         */

        cmp     dword ptr [ebx+PcPerfGlobalGroupMask], 0
        jne     .Lsc92

.Lsc03:

#ifndef NT_UP

#if DBG

        mov     cl, [esi+ThNextProcessor]
        cmp     cl, [ebx+PcPrcbData+PbNumber]
        jne     .Lsc_error2

#endif

#endif

        /*
         * Read current CR0.
         */

        mov     ebp, cr0
        mov     edx, ebp

#ifndef NT_UP

        cmp     byte ptr [edi+ThNpxState], NPX_STATE_LOADED
        je      .Lsc_save_npx_state

#endif

.Lsc05:

        /*
         * Save old stack pointer.
         */

        mov     [edi+ThKernelStack], esp

        mov     eax, [esi+ThInitialStack]

        /*
         * Validate NPX constants.
         */

#if (NPX_STATE_NOT_LOADED != (CR0_TS + CR0_MP))
#error "NPX_STATE_NOT_LOADED constant mismatch"
#endif

#if (NPX_STATE_LOADED != 0)
#error "NPX_STATE_LOADED constant mismatch"
#endif

#ifdef NT_UP

        cli

#endif

        movzx   ecx, byte ptr [esi+ThNpxState]

        and     edx, ~(CR0_MP+CR0_EM+CR0_TS)

        or      ecx, edx

        or      ecx, [eax+FpCr0NpxState-NPX_FRAME_LENGTH]

        cmp     ebp, ecx
        jne     .Lsc_reload_cr0

.Lsc06:

#ifdef NT_UP
        sti
#endif

#if DBG

        mov     eax, [esi+ThKernelStack]

        cmp     esi, dword ptr [eax-4]
        je      .Lsc_stack_ok

        int     3

.Lsc_stack_ok:

        xchg    esp, eax

        mov     [eax-4], edi

#else

        mov     esp, [esi+ThKernelStack]

#endif

        /*
         * Check whether process changed.
         */

        mov     ebp, [esi+ThApcState+AsProcess]
        mov     eax, [edi+ThApcState+AsProcess]

        cmp     ebp, eax
        je      .Lsc23

#ifndef NT_UP

        /*
         * Set processor bit in new process and clear in old process.
         */

        mov     ecx, [ebx+PcSetMemberCopy]

        lock xor [ebp+PrActiveProcessors], ecx
        lock xor [eax+PrActiveProcessors], ecx

#if DBG

        test    [ebp+PrActiveProcessors], ecx
        jz      .Lsc_error5

        test    [eax+PrActiveProcessors], ecx
        jnz     .Lsc_error4

#endif

#endif

        /*
         * LDT switch.
         */

        mov     ecx, [ebp+PrLdtDescriptor]
        or      ecx, [eax+PrLdtDescriptor]

        jnz     .Lsc_load_ldt

.Lsc_load_ldt_ret:

        /*
         * Load new CR3.
         */

        mov     eax, [ebp+PrDirectoryTableBase]
        mov     cr3, eax


.Lsc23:

#ifndef NT_UP

        /*
         * Clear context swap busy.
         */

        and     byte ptr [edi+ThSwapBusy], 0

#endif

        xor     eax, eax
        mov     gs, ax

        /*
         * Update TEB descriptor and PCR TEB.
         */

        mov     eax, [esi+ThTeb]
        mov     [ebx+PcTeb], eax

        mov     ecx, [ebx+PcGdt]

        mov     [ecx+KGDT_R3_TEB+KgdtBaseLow], ax

        shr     eax, 16

        mov     [ecx+KGDT_R3_TEB+KgdtBaseMid], al
        mov     [ecx+KGDT_R3_TEB+KgdtBaseHi], ah

        /*
         * Adjust initial stack and establish TSS ESP0.
         */

        mov     eax, [esi+ThInitialStack]

        sub     eax, NPX_FRAME_LENGTH

#if ((EFLAGS_V86_MASK & 0FF00FFFFh) != 0)
#error "EFLAGS_V86_MASK mismatch"
#endif

        /*
         * Original MASM:
         *
         * test byte ptr [eax] - KTRAP_FRAME_LENGTH + TsEFlags + 2,
         *      EFLAGS_V86_MASK / 10000h
         *
         * 10000h == 0x10000.
         */

        test    byte ptr [eax-KTRAP_FRAME_LENGTH+TsEFlags+2], \
                (EFLAGS_V86_MASK / 0x10000)

        jnz     .Lsc24

        sub     eax, TsV86Gs-TsHardwareSegSs

.Lsc24:

        mov     ecx, [ebx+PcTssCopy]
        mov     [ecx+TssEsp0], eax

        /*
         * Update IOPM.
         */

        mov     ax, [ebp+PrIopmOffset]
        mov     [ecx+TssIoMapBase], ax

        /*
         * Update thread context switch count.
         */

        inc     dword ptr [esi+ThContextSwitches]

        /*
         * Restore exception list and APC bypass flag.
         */

        pop     [ebx+PcExceptionList]
        pop     ecx

        /*
         * Context switch from DPC is illegal.
         */

        cmp     byte ptr [ebx+PcPrcbData+PbDpcRoutineActive], 0
        jne     .Lsc91

        /*
         * Check kernel APC pending.
         */

        cmp     byte ptr [esi+ThApcState+AsKernelApcPending], 0
        jne     .Lsc80

        xor     eax, eax
        ret


.Lsc80:

        /*
         * Special APC disable count.
         */

        cmp     word ptr [esi+ThSpecialApcDisable], 0
        jne     .Lsc90

        /*
         * APC bypass enabled?
         */

        test    cl, cl
        jz      .Lsc90

        mov     cl, APC_LEVEL

        fstCall HalRequestSoftwareInterrupt

        /*
         * Ensure ZF is clear.
         */

        or      eax, esp

.Lsc90:

        setz    al
        ret


/*
 * -------------------------------------------------------------------------
 * Load new LDT.
 * -------------------------------------------------------------------------
 */

.Lsc_load_ldt:

        mov     eax, [ebp+PrLdtDescriptor]

        test    eax, eax
        je      .Lsc_no_ldt_descriptor

        mov     ecx, [ebx+PcGdt]

        mov     [ecx+KGDT_LDT], eax

        mov     eax, [ebp+PrLdtDescriptor+4]
        mov     [ecx+KGDT_LDT+4], eax

        /*
         * Update INT 21 descriptor.
         */

        mov     ecx, [ebx+PcIdt]

        mov     eax, [ebp+PrInt21Descriptor]
        mov     [ecx+21h*8], eax

        mov     eax, [ebp+PrInt21Descriptor+4]
        mov     [ecx+21h*8+4], eax

        mov     eax, KGDT_LDT

.Lsc_no_ldt_descriptor:

        lldt    ax

        jmp     .Lsc_load_ldt_ret


/*
 * -------------------------------------------------------------------------
 * CR0 changed.
 * -------------------------------------------------------------------------
 */

.Lsc_reload_cr0:

#if DBG

        test    byte ptr [esi+ThNpxState], ~(CR0_TS+CR0_MP)
        jnz     .Lsc_error

        test    dword ptr [eax+FpCr0NpxState-NPX_FRAME_LENGTH], \
                ~(CR0_PE+CR0_MP+CR0_EM+CR0_TS)
        jnz     .Lsc_error3

#endif

        mov     cr0, ecx

        jmp     .Lsc06


#ifndef NT_UP

/*
 * -------------------------------------------------------------------------
 * Save current NPX state.
 * -------------------------------------------------------------------------
 */

.Lsc_save_npx_state:

        and     edx, ~(CR0_MP+CR0_EM+CR0_TS)

        mov     ecx, [edi+ThInitialStack]
        sub     ecx, NPX_FRAME_LENGTH

        cmp     ebp, edx
        je      .Lsc_npx10

        mov     cr0, edx
        mov     ebp, edx

.Lsc_npx10:

        /*
         * FXSR instruction is patched at boot time if necessary.
         *
         * Original bytes:
         *
         *     0F AE 01
         */

_ScPatchFxb:
        .byte   0x0f, 0xae, 0x01

_ScPatchFxe:

        mov     byte ptr [edi+ThNpxState], NPX_STATE_NOT_LOADED

        mov     dword ptr [ebx+PcPrcbData+PbNpxThread], 0

        jmp     .Lsc05

#endif /* !NT_UP */


/*
 * -------------------------------------------------------------------------
 * Context switch tracing.
 * -------------------------------------------------------------------------
 */

.Lsc92:

        mov     eax, [ebx+PcPerfGlobalGroupMask]

        cmp     eax, 0
        je      .Lsc03

        mov     edx, esi
        mov     ecx, edi

        test    dword ptr [eax+PERF_CONTEXTSWAP_OFFSET], \
                PERF_CONTEXTSWAP_FLAG

        jz      .Lsc03

        fstCall WmiTraceContextSwap

        jmp     .Lsc03


/*
 * -------------------------------------------------------------------------
 * Context switch attempted while in DPC.
 * -------------------------------------------------------------------------
 */

.Lsc91:

        mov     eax, [edi+ThInitialStack]

        /*
         * stdCall _KeBugCheckEx <
         *     ATTEMPTED_SWITCH_FROM_DPC,
         *     edi,
         *     esi,
         *     eax,
         *     0
         * >
         */

        push    0
        push    eax
        push    esi
        push    edi
        push    ATTEMPTED_SWITCH_FROM_DPC

        call    _KeBugCheckEx@20

        ret


#if DBG

.Lsc_error5:
        int     3

.Lsc_error4:
        int     3

.Lsc_error3:
        int     3

.Lsc_error2:
        int     3

.Lsc_error:
        int     3

#endif


/*
 * =========================================================================
 *
 * KeFlushCurrentTb
 *
 * =========================================================================
 */

        .globl _KeFlushCurrentTb
FUNC(_KeFlushCurrentTb, 0)

ktb00:
        mov     eax, cr3
        mov     cr3, eax

        ret


ktb_gb:
        mov     eax, cr4
        and     eax, ~CR4_PGE
        mov     cr4, eax

        or      eax, CR4_PGE
        mov     cr4, eax

ktb_eb:
        ret


ENDPROC(_KeFlushCurrentTb)


/*
 * =========================================================================
 *
 * KiFlushDcache / KiFlushIcache
 *
 * =========================================================================
 */

        .globl _KiFlushDcache
        .globl _KiFlushIcache

FUNC(_KiFlushDcache, 0)
        ret
ENDPROC(_KiFlushDcache)

FUNC(_KiFlushIcache, 0)
        ret
ENDPROC(_KiFlushIcache)


/*
 * =========================================================================
 *
 * INIT CODE
 *
 * =========================================================================
 */

        .section .text$INIT,"xr"
        .p2align 4


/*
 * =========================================================================
 *
 * Ki386EnableGlobalPage
 *
 * =========================================================================
 */

        .globl _Ki386EnableGlobalPage
FUNC(_Ki386EnableGlobalPage, 0)

        push    esi
        push    edi
        push    ebx

        mov     edx, [esp+16]

        pushfd
        cli

        /*
         * Wait for all processors.
         */

        lock dec dword ptr [edx]

.Legp10:
        YIELD

        cmp     dword ptr [edx], 0
        jne     .Legp10

        cmp     byte ptr PCR[PcNumber], 0
        jne     .Legp20

        /*
         * Install proper KeFlushCurrentTb implementation.
         */

        mov     edi, OFFSET FLAT:ktb00
        mov     esi, OFFSET FLAT:ktb_gb

        mov     ecx, ktb_eb-ktb_gb+1

        rep movsb

        mov     byte ptr [ktb_eb], 0


/*
 * Wait until processor 0 has completed patching.
 */

.Legp20:

        cmp     byte ptr [ktb_eb], 0
        jne     .Legp20

        /*
         * Flush TB and enable global pages.
         */

        mov     eax, cr4
        and     eax, ~CR4_PGE

        mov     ecx, cr3

        mov     cr4, eax
        mov     cr3, ecx

        or      eax, CR4_PGE
        mov     cr4, eax

        popfd

        pop     ebx
        pop     edi
        pop     esi

        ret

ENDPROC(_Ki386EnableGlobalPage)


/*
 * =========================================================================
 *
 * Ki386EnableDE
 *
 * =========================================================================
 */

        .globl _Ki386EnableDE
FUNC(_Ki386EnableDE, 0)

        mov     eax, cr4
        or      eax, CR4_DE
        mov     cr4, eax

        ret

ENDPROC(_Ki386EnableDE)


/*
 * =========================================================================
 *
 * Ki386EnableFxsr
 *
 * =========================================================================
 */

        .globl _Ki386EnableFxsr
FUNC(_Ki386EnableFxsr, 0)

        mov     eax, cr4
        or      eax, CR4_FXSR
        mov     cr4, eax

        ret

ENDPROC(_Ki386EnableFxsr)


/*
 * =========================================================================
 *
 * Ki386EnableXMMIExceptions
 *
 * =========================================================================
 */

        .globl _Ki386EnableXMMIExceptions
FUNC(_Ki386EnableXMMIExceptions, 0)

        /*
         * Set up IDT entry for INT 19.
         */

        mov     ecx, PCR[PcIdt]

        lea     eax, [ecx+098h]

        /*
         * P=1, DPL=0, interrupt gate type=e.
         */

        mov     byte ptr [eax+5], 08eh

        mov     word ptr [eax+2], KGDT_R0_CODE

        mov     edx, OFFSET FLAT:_KiTrap13
        mov     ecx, edx

        mov     word ptr [eax], cx

        shr     ecx, 16

        mov     word ptr [eax+6], cx

        /*
         * Enable XMMI exception handling.
         */

        mov     eax, cr4
        or      eax, CR4_XMMEXCPT
        mov     cr4, eax

        ret

ENDPROC(_Ki386EnableXMMIExceptions)


/*
 * =========================================================================
 *
 * Ki386EnableCurrentLargePage
 *
 * =========================================================================
 */

        .globl _Ki386EnableCurrentLargePage
        .globl _Ki386EnableCurrentLargePageEnd

FUNC(_Ki386EnableCurrentLargePage, 0)

        mov     ecx, [esp+4]
        mov     edx, [esp+8]

        pushfd
        cli

        /*
         * Save original CR3.
         */

        mov     eax, cr3

        /*
         * Load identity mapping.
         */

        mov     cr3, edx

        /*
         * Convert supplied identity address into the physical address
         * corresponding to _Ki386LargePageIdentityLabel.
         */

        sub     ecx, OFFSET FLAT:_Ki386EnableCurrentLargePage
        add     ecx, OFFSET FLAT:_Ki386LargePageIdentityLabel

        jmp     ecx


_Ki386LargePageIdentityLabel:

        mov     ecx, cr0
        and     ecx, ~CR0_PG
        mov     cr0, ecx

        /*
         * Serialize the paging transition.
         */

        jmp     .Llarge_page_serialized

.Llarge_page_serialized:

        mov     edx, cr4
        or      edx, CR4_PSE
        mov     cr4, edx

        mov     edx, OFFSET FLAT:OriginalMapping

        or      ecx, CR0_PG
        mov     cr0, ecx

        jmp     edx


OriginalMapping:

        mov     cr3, eax

        popfd

        ret


_Ki386EnableCurrentLargePageEnd:


ENDPROC(_Ki386EnableCurrentLargePage)


/*
 * =========================================================================
 *
 * Return to normal kernel text.
 *
 * =========================================================================
 */

        .section .text$00,"xr"
        .p2align 4


/*
 * =========================================================================
 *
 * KiSwapProcess
 *
 * =========================================================================
 */

        .globl _KiSwapProcess
FUNC(_KiSwapProcess, 0)

        mov     edx, [esp+4]
        mov     eax, [esp+8]

#ifndef NT_UP

        mov     ecx, PCR[PcSetMember]

        lock xor [edx+PrActiveProcessors], ecx
        lock xor [eax+PrActiveProcessors], ecx

#if DBG

        test    [edx+PrActiveProcessors], ecx
        jz      .Lkisp_error1

        test    [eax+PrActiveProcessors], ecx
        jnz     .Lkisp_error

#endif

#endif

        /*
         * LDT switch.
         */

        mov     ecx, [edx+PrLdtDescriptor]
        or      ecx, [eax+PrLdtDescriptor]

        jnz     .Lkisp_load_ldt

.Lkisp_load_ldt_ret:

        /*
         * Load new CR3.
         */

        mov     eax, [edx+PrDirectoryTableBase]
        mov     cr3, eax

        mov     ecx, PCR[PcTssCopy]

        /*
         * Clear GS.
         */

        xor     eax, eax
        mov     gs, ax

        /*
         * Change IOPM.
         */

        mov     ax, [edx+PrIopmOffset]
        mov     [ecx+TssIoMapBase], ax

        ret


.Lkisp_load_ldt:

        /*
         * Edit LDT descriptor.
         */

        mov     eax, [edx+PrLdtDescriptor]

        test    eax, eax
        je      .Lkisp_no_ldt

        mov     ecx, PCR[PcGdt]

        mov     [ecx+KGDT_LDT], eax

        mov     eax, [edx+PrLdtDescriptor+4]
        mov     [ecx+KGDT_LDT+4], eax

        /*
         * Set INT 21 descriptor.
         */

        mov     ecx, PCR[PcIdt]

        mov     eax, [edx+PrInt21Descriptor]
        mov     [ecx+21h*8], eax

        mov     eax, [edx+PrInt21Descriptor+4]
        mov     [ecx+21h*8+4], eax

        mov     eax, KGDT_LDT

.Lkisp_no_ldt:

        lldt    ax

        jmp     .Lkisp_load_ldt_ret


#if DBG

.Lkisp_error1:
        int     3

.Lkisp_error:
        int     3

#endif


ENDPROC(_KiSwapProcess)


/*
 * =========================================================================
 *
 * Idle Loop
 *
 * =========================================================================
 */

        .globl KiIdleLoop
FUNC(KiIdleLoop, 0)

#if DBG

        xor     edi, edi

#endif

        jmp     .Lkid20


/*
 * -------------------------------------------------------------------------
 * HAL idle processor.
 * -------------------------------------------------------------------------
 */

.Lkid10:

        lea     ecx, [ebx+PcPrcbData+PbPowerState]

        call    dword ptr [ecx+PpIdleFunction]


/*
 * -------------------------------------------------------------------------
 * Debugger polling.
 * -------------------------------------------------------------------------
 */

.Lkid20:

#if DBG

#ifndef NT_UP

        mov     eax, [_KiIdleSummary]

        mov     ecx, [ebx+PcSetMember]

        dec     ecx
        and     eax, ecx

        jnz     .LCheckDpcList

#endif

        dec     edi
        jg      .LCheckDpcList

        POLL_DEBUGGER

#endif


.Lkid30:

        YIELD


.LCheckDpcList:


        /*
         * Enable interrupts briefly and then disable them again.
         */

        sti
        nop
        nop
        cli

        /*
         * Process current processor DPC list.
         */

        mov     eax, [ebx+PcPrcbData+PbDpcQueueDepth]
        or      eax, [ebx+PcPrcbData+PbTimerRequest]

#ifndef NT_UP
        or      eax, [ebx+PcPrcbData+PbDeferredReadyListHead]
#endif

        jz      .LCheckNextThread

        mov     cl, DISPATCH_LEVEL

        fstCall HalClearSoftwareInterrupt

        lea     ecx, [ebx+PcPrcbData]

        fstCall KiRetireDpcList

#if DBG

        xor     edi, edi

#endif


.LCheckNextThread:

        cmp     dword ptr [ebx+PcPrcbData+PbNextThread], 0

#ifdef NT_UP

        je      .Lkid10

#else

        je      .Lkid40

#endif


#ifndef NT_UP

        /*
         * Raise IRQL to synchronization level.
         */

        RaiseIrql SYNCH_LEVEL, NoOld

#endif

        sti

        mov     edi, [ebx+PcPrcbData+PbCurrentThread]


#ifndef NT_UP

        /*
         * Set swap busy and acquire PRCB lock.
         */

        mov     byte ptr [edi+ThSwapBusy], 1

        lock bts dword ptr [ebx+PcPrcbData+PbPrcbLock], 0

        jnc     .Lkid33

        lea     ecx, [ebx+PcPrcbData+PbPrcbLock]

        fstCall KefAcquireSpinLockAtDpcLevel

#endif


.Lkid33:

        mov     esi, [ebx+PcPrcbData+PbNextThread]

#ifndef NT_UP

        cmp     esi, edi
        je      .Lkisame

#endif

        and     dword ptr [ebx+PcPrcbData+PbNextThread], 0

        mov     [ebx+PcPrcbData+PbCurrentThread], esi

        mov     byte ptr [esi+ThState], Running


#ifndef NT_UP

        /*
         * Clear idle schedule and release PRCB lock.
         */

        and     byte ptr [ebx+PcPrcbData+PbIdleSchedule], 0

        and     dword ptr [ebx+PcPrcbData+PbPrcbLock], 0

#endif


.Lkid35:

        mov     ecx, APC_LEVEL

        call    SwapContext


#ifndef NT_UP

        LowerIrql DISPATCH_LEVEL

#endif

        jmp     .Lkid30


#ifndef NT_UP

/*
 * -------------------------------------------------------------------------
 *
 * The idle thread was selected again.
 *
 * -------------------------------------------------------------------------
 */

.Lkisame:

        and     dword ptr [ebx+PcPrcbData+PbNextThread], 0

        and     dword ptr [ebx+PcPrcbData+PbPrcbLock], 0

        and     byte ptr [edi+ThSwapBusy], 0

        jmp     .Lkid30


/*
 * -------------------------------------------------------------------------
 *
 * Call idle schedule if requested.
 *
 * -------------------------------------------------------------------------
 */

.Lkid40:

        cmp     byte ptr [ebx+PcPrcbData+PbIdleSchedule], 0
        je      .Lkid10

        sti

        lea     ecx, [ebx+PcPrcbData]

        fstCall KiIdleSchedule

        test    eax, eax

        mov     esi, eax

        mov     edi, [ebx+PcPrcbData+PbIdleThread]

        jnz     .Lkid35

        jmp     .Lkid30

#endif /* !NT_UP */


ENDPROC(KiIdleLoop)


/*
 * =========================================================================
 *
 * Optional debugger polling entry point.
 *
 * =========================================================================
 */

#ifdef DBGMP

        .globl _KiPollDebugger
FUNC(_KiPollDebugger, 0)

        push    eax
        push    ecx
        push    edx

        POLL_DEBUGGER

        pop     edx
        pop     ecx
        pop     eax

        ret

ENDPROC(_KiPollDebugger)

#endif /* DBGMP */


/*
 * =========================================================================
 *
 * Ki386AdjustEsp0
 *
 * =========================================================================
 */

        .globl _Ki386AdjustEsp0
FUNC(_Ki386AdjustEsp0, 0)

#if DBG

        /*
         * Make sure the trap frame cannot currently be edited by
         * SetContextThread.
         */

        CurrentIrql

        cmp     al, APC_LEVEL
        jge     .Lae_irql_ok

        int     3

.Lae_irql_ok:

#endif

        mov     eax, PCR[PcPrcbData+PbCurrentThread]

        mov     edx, [esp+4]

        mov     eax, [eax+ThInitialStack]

        test    dword ptr [edx+TsEFlags], EFLAGS_V86_MASK

        jnz     .Lae10

        sub     eax, TsV86Gs-TsHardwareSegSS


.Lae10:

        sub     eax, NPX_FRAME_LENGTH

        /*
         * Do not allow another processor to modify the TSS while
         * changing ESP0.
         */

        pushfd
        cli

        mov     edx, PCR[PcTssCopy]

        mov     [edx+TssEsp0], eax

        popfd

        ret

ENDPROC(_Ki386AdjustEsp0)


        .att_syntax
