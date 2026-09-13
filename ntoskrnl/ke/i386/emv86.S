/*
 * title  "Vdm Instruction Emulation"
 *
 * Copyright (c) OpenXP
 *
 * Module Name:
 *
 *    emv86.S
 *
 * Abstract:
 *
 *    This module contains the routines for emulating instructions and
 *    faults from v86 mode.
 */

#define KERNELONLY 1

#include <asm.h>
#include <ks386.inc>
#include <i386/kimacro.inc>
#include <mac386.inc>
#include <i386/mi.inc>
#include <callconv.inc>
#include <vdm/i386/vdm.inc>
#include <vdmtib.inc>
#include <irqli386.inc>

/*
 * External routines.
 */
.extern VdmOpcode0f
.extern _DbgPrint
.extern _KeI386VirtualIntExtensions
.extern _Ki386VdmDispatchIo
.extern _Ki386VdmDispatchStringIo
.extern _KiDispatchException
.extern _Ki386VdmReflectException
.extern _VdmEndExecution
.extern _VdmDispatchBop
.extern _VdmPrinterStatus
.extern _VdmPrinterWriteData
.extern _VdmDispatchInterrupts
.extern _KeBugCheck
.extern _VdmSkipNpxInstruction
.extern _VdmFetchBop1
.extern _Ki386AdjustEsp0

#ifdef VDMDBG
.extern _VdmTraceEvent
#endif

.extern _ExVdmOpcodeDispatchCounts
.extern OpcodeIndex
.extern _VdmUserCr0MapIn
.extern _MmUserProbeAddress

/*
 * The original MASM source uses the following helper macros:
 *
 *     stdCall
 *     stdRet
 *     MPLOCK
 *     PCR[]
 *     TsXXX
 *     VtXXX
 *     ViXXX
 *     etc.
 *
 * They are expected to come from the OpenXP/ReactOS assembler headers.
 *
 * For ordinary stdcall calls we expand them explicitly below.  This
 * avoids depending on a MASM-only <name, arg, ...> macro syntax.
 */


/*
 * --------------------------------------------------------------------------
 * OpcodeDispatchV86
 * --------------------------------------------------------------------------
 *
 * Instruction dispatch table.
 *
 * If the OpenXP dtBEGIN/dtS/dtEND macros are GAS-compatible, retain the
 * original form.  Otherwise this can be expressed as a sequence of .long
 * handler addresses.
 */

.section .data
.p2align 2

.globl OpcodeDispatchV86
OpcodeDispatchV86:

    /*
     * The original dtBEGIN/dtS/dtEND construct describes this table.
     * Each entry is the address of the corresponding handler.
     */
    .long Opcode0FV86
    .long OpcodeESPrefixV86
    .long OpcodeCSPrefixV86
    .long OpcodeSSPrefixV86
    .long OpcodeDSPrefixV86
    .long OpcodeFSPrefixV86
    .long OpcodeGSPrefixV86
    .long OpcodeOPER32PrefixV86
    .long OpcodeADDR32PrefixV86
    .long OpcodeINSBV86
    .long OpcodeINSWV86
    .long OpcodeOUTSBV86
    .long OpcodeOUTSWV86
    .long OpcodePUSHFV86
    .long OpcodePOPFV86
    .long OpcodeINTnnV86
    .long OpcodeINTOV86
    .long OpcodeIRETV86
    .long OpcodeNPXV86
    .long OpcodeINBimmV86
    .long OpcodeINWimmV86
    .long OpcodeOUTBimmV86
    .long OpcodeOUTWimmV86
    .long OpcodeINBV86
    .long OpcodeINWV86
    .long OpcodeOUTBV86
    .long OpcodeOUTWV86
    .long OpcodeLOCKPrefixV86
    .long OpcodeREPNEPrefixV86
    .long OpcodeREPPrefixV86
    .long OpcodeCLIV86
    .long OpcodeSTIV86
    .long OpcodeHLTV86

.section .text


/*
 * --------------------------------------------------------------------------
 * Prefix handlers
 * --------------------------------------------------------------------------
 *
 * The MASM source generated these routines with:
 *
 *     opPrefix ES
 *     opPrefix CS
 *     ...
 *
 * GAS does not have MASM's IRP syntax, so generate them explicitly.
 */


/*
 * ES prefix
 */
.globl OpcodeESPrefixV86
.type OpcodeESPrefixV86,@function
OpcodeESPrefixV86:
    orl $(PREFIX_ES), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeESPrefixV86,.-OpcodeESPrefixV86

/*
 * CS prefix
 */
.globl OpcodeCSPrefixV86
.type OpcodeCSPrefixV86,@function
OpcodeCSPrefixV86:
    orl $(PREFIX_CS), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeCSPrefixV86,.-OpcodeCSPrefixV86

/*
 * SS prefix
 */
.globl OpcodeSSPrefixV86
.type OpcodeSSPrefixV86,@function
OpcodeSSPrefixV86:
    orl $(PREFIX_SS), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeSSPrefixV86,.-OpcodeSSPrefixV86

/*
 * DS prefix
 */
.globl OpcodeDSPrefixV86
.type OpcodeDSPrefixV86,@function
OpcodeDSPrefixV86:
    orl $(PREFIX_DS), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeDSPrefixV86,.-OpcodeDSPrefixV86

/*
 * FS prefix
 */
.globl OpcodeFSPrefixV86
.type OpcodeFSPrefixV86,@function
OpcodeFSPrefixV86:
    orl $(PREFIX_FS), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeFSPrefixV86,.-OpcodeFSPrefixV86

/*
 * GS prefix
 */
.globl OpcodeGSPrefixV86
.type OpcodeGSPrefixV86,@function
OpcodeGSPrefixV86:
    orl $(PREFIX_GS), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeGSPrefixV86,.-OpcodeGSPrefixV86

/*
 * OPER32 prefix
 */
.globl OpcodeOPER32PrefixV86
.type OpcodeOPER32PrefixV86,@function
OpcodeOPER32PrefixV86:
    orl $(PREFIX_OPER32), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeOPER32PrefixV86,.-OpcodeOPER32PrefixV86

/*
 * ADDR32 prefix
 */
.globl OpcodeADDR32PrefixV86
.type OpcodeADDR32PrefixV86,@function
OpcodeADDR32PrefixV86:
    orl $(PREFIX_ADDR32), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeADDR32PrefixV86,.-OpcodeADDR32PrefixV86

/*
 * LOCK prefix
 */
.globl OpcodeLOCKPrefixV86
.type OpcodeLOCKPrefixV86,@function
OpcodeLOCKPrefixV86:
    orl $(PREFIX_LOCK), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeLOCKPrefixV86,.-OpcodeLOCKPrefixV86

/*
 * REPNE prefix
 */
.globl OpcodeREPNEPrefixV86
.type OpcodeREPNEPrefixV86,@function
OpcodeREPNEPrefixV86:
    orl $(PREFIX_REPNE), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeREPNEPrefixV86,.-OpcodeREPNEPrefixV86

/*
 * REP prefix
 */
.globl OpcodeREPPrefixV86
.type OpcodeREPPrefixV86,@function
OpcodeREPPrefixV86:
    orl $(PREFIX_REP), %ebx
    jmp OpcodeGenericPrefixV86
.size OpcodeREPPrefixV86,.-OpcodeREPPrefixV86


/*
 * --------------------------------------------------------------------------
 * Ki386DispatchOpcodeV86
 * --------------------------------------------------------------------------
 *
 * Arguments:
 *
 *     4(%esp) = trap frame
 *
 * Returns:
 *
 *     EAX = 0 failure
 *     EAX = 1 success
 */

.globl _Ki386DispatchOpcodeV86
.type _Ki386DispatchOpcodeV86,@function
_Ki386DispatchOpcodeV86:

    pushl %ebp

    movl 8(%esp), %ebp

    movzwl TsSegCs(%ebp), %esi
    shll $4, %esi

    andl $0xffff, TsEip(%ebp)
    andl $0xffff, TsHardwareEsp(%ebp)

    addl TsEip(%ebp), %esi

    /*
     * Probe and fetch the first byte from the instruction stream.
     *
     * stdCall _VdmFetchBop1, <esi>
     */
    pushl %esi
    call _VdmFetchBop1@4

    movzbl OpcodeIndex(%eax), %edx

    movl $1, %edi
    xorl %ebx, %ebx

#ifdef DEVL
    incl _ExVdmOpcodeDispatchCounts(,%edx,4)
#endif

#ifdef VDMDBG
    pushal

    pushl %ebp
    pushl $0
    pushl %ecx
    pushl $VDMTR_KERNEL_OP_V86
    call _VdmTraceEvent@16

    popal
#endif

    call *OpcodeDispatchV86(,%edx,4)

    popl %ebp
    ret $4

.size _Ki386DispatchOpcodeV86,.-_Ki386DispatchOpcodeV86


/*
 * --------------------------------------------------------------------------
 * Invalid opcode
 * --------------------------------------------------------------------------
 */

.globl OpcodeInvalidV86
.type OpcodeInvalidV86,@function
OpcodeInvalidV86:

    xorl %eax, %eax
    ret

.size OpcodeInvalidV86,.-OpcodeInvalidV86


/*
 * --------------------------------------------------------------------------
 * Generic prefix dispatcher
 * --------------------------------------------------------------------------
 */

.globl OpcodeGenericPrefixV86
.type OpcodeGenericPrefixV86,@function
OpcodeGenericPrefixV86:

    incl %esi
    incl %edi

    movzbl (%esi), %ecx
    movzbl OpcodeIndex(%ecx), %edx

#ifdef DEVL
    incl _ExVdmOpcodeDispatchCounts(,%edx,4)
#endif

    jmp *OpcodeDispatchV86(,%edx,4)

.size OpcodeGenericPrefixV86,.-OpcodeGenericPrefixV86


/*
 * --------------------------------------------------------------------------
 * INSB
 * --------------------------------------------------------------------------
 */

.globl OpcodeINSBV86
.type OpcodeINSBV86,@function
OpcodeINSBV86:

    pushl %ebp
    pushl %edi

    movzwl TsV86Es(%ebp), %eax
    shll $16, %eax

    movzwl TsEdi(%ebp), %ecx
    orl %ecx, %eax

    pushl %eax

    movl $1, %eax
    xorl %ecx, %ecx

    testl $(PREFIX_REP), %ebx
    jz .Loisb20

    movl $1, %ecx
    movzwl TsEcx(%ebp), %eax

.Loisb20:
    pushl %eax
    pushl $TRUE
    pushl %ecx
    pushl $1

    movzwl TsEdx(%ebp), %eax
    pushl %eax

    call _Ki386VdmDispatchStringIo@32
    ret

.size OpcodeINSBV86,.-OpcodeINSBV86


/*
 * --------------------------------------------------------------------------
 * INSW
 * --------------------------------------------------------------------------
 */

.globl OpcodeINSWV86
.type OpcodeINSWV86,@function
OpcodeINSWV86:

    pushl %ebp
    pushl %edi

    movzwl TsV86Es(%ebp), %eax
    shll $16, %eax

    movzwl TsEdi(%ebp), %ecx
    orl %ecx, %eax

    pushl %eax

    movl $1, %eax
    xorl %ecx, %ecx

    testl $(PREFIX_REP), %ebx
    jz .Loisw20

    movl $1, %ecx
    movzwl TsEcx(%ebp), %eax

.Loisw20:
    pushl %eax
    pushl $TRUE
    pushl %ecx
    pushl $2

    movzwl TsEdx(%ebp), %eax
    pushl %eax

    call _Ki386VdmDispatchStringIo@32
    ret

.size OpcodeINSWV86,.-OpcodeINSWV86


/*
 * --------------------------------------------------------------------------
 * OUTSB
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTSBV86
.type OpcodeOUTSBV86,@function
OpcodeOUTSBV86:

    pushl %ebp
    pushl %edi

    movzwl TsV86Ds(%ebp), %eax
    shll $16, %eax

    movzwl TsEsi(%ebp), %ecx
    orl %ecx, %eax

    pushl %eax

    movl $1, %eax
    xorl %ecx, %ecx

    testl $(PREFIX_REP), %ebx
    jz .Loosb20

    movl $1, %ecx
    movzwl TsEcx(%ebp), %eax

.Loosb20:
    pushl %eax
    pushl $FALSE
    pushl %ecx
    pushl $1

    movzwl TsEdx(%ebp), %eax
    pushl %eax

    call _Ki386VdmDispatchStringIo@32
    ret

.size OpcodeOUTSBV86,.-OpcodeOUTSBV86


/*
 * --------------------------------------------------------------------------
 * OUTSW
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTSWV86
.type OpcodeOUTSWV86,@function
OpcodeOUTSWV86:

    pushl %ebp
    pushl %edi

    movzwl TsV86Ds(%ebp), %eax
    shll $16, %eax

    movzwl TsEsi(%ebp), %ecx
    orl %ecx, %eax

    pushl %eax

    movl $1, %eax
    xorl %ecx, %ecx

    testl $(PREFIX_REP), %ebx
    jz .Loosw20

    movl $1, %ecx
    movzwl TsEcx(%ebp), %eax

.Loosw20:
    pushl %eax
    pushl $FALSE
    pushl %ecx
    pushl $2

    movzwl TsEdx(%ebp), %eax
    pushl %eax

    call _Ki386VdmDispatchStringIo@32
    ret

.size OpcodeOUTSWV86,.-OpcodeOUTSWV86


/*
 * --------------------------------------------------------------------------
 * PUSHF
 * --------------------------------------------------------------------------
 */

.globl OpcodePUSHFV86
.type OpcodePUSHFV86,@function
OpcodePUSHFV86:

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Lpuf00

    movl TsEFlags(%ebp), %eax

    leal FIXED_NTVDMSTATE_LINEAR, %ecx

    orl $(VDM_VIRTUAL_INTERRUPTS), (%ecx)

    testl $(EFLAGS_VIF), %eax
    jnz .Lpuf03

    andl $(~VDM_VIRTUAL_INTERRUPTS), (%ecx)
    andl $(~EFLAGS_INTERRUPT_MASK), %eax

    jmp .Lpuf03

.Lpuf00:
    leal FIXED_NTVDMSTATE_LINEAR, %eax

    movl TsEFlags(%ebp), %edx
    movl (%eax), %eax

    andl $(~EFLAGS_INTERRUPT_MASK), %edx

    andl $(VDM_VIRTUAL_INTERRUPTS | VDM_VIRTUAL_AC | VDM_VIRTUAL_NT), %eax

    orl %edx, %eax
    orl $(EFLAGS_IOPL_MASK), %eax

.Lpuf03:

    movzwl TsHardwareSegSS(%ebp), %ecx
    movzwl TsHardwareEsp(%ebp), %edx

    shll $4, %ecx
    subw $2, %dx

    testl $(PREFIX_OPER32), %ebx
    jnz .Lpuf10

    movw %ax, (%ecx,%edx,1)

.Lpuf05:
    movw %dx, TsHardwareEsp(%ebp)

    addl %edi, TsEip(%ebp)

    movl FIXED_NTVDMSTATE_LINEAR, %eax

    testl $(VDM_VIRTUAL_INTERRUPTS), %eax
    jz .Lpuf06

    testl $(VDM_INTERRUPT_PENDING), %eax
    jz .Lpuf06

    call VdmDispatchIntAck

.Lpuf06:
    movl $1, %eax
    ret

.Lpuf10:
    subw $2, %dx
    movl %eax, (%ecx,%edx,1)

    jmp .Lpuf05

.size OpcodePUSHFV86,.-OpcodePUSHFV86


/*
 * --------------------------------------------------------------------------
 * POPF
 * --------------------------------------------------------------------------
 */

.globl OpcodePOPFV86
.type OpcodePOPFV86,@function
OpcodePOPFV86:

#ifdef DBG
    CurrentIrql

    cmpb $APC_LEVEL, %al
    jge .LpofDbgOk

    int $3

.LpofDbgOk:
#endif

    leal FIXED_NTVDMSTATE_LINEAR, %eax

    movl TsHardwareSegSS(%ebp), %ecx
    movzwl TsHardwareEsp(%ebp), %edx

    shll $4, %ecx

    movl (%ecx,%edx,1), %ecx
    addl $4, %edx

    testl $(PREFIX_OPER32), %ebx
    jnz .Lpof10

    andl $0xffff, %ecx
    subl $2, %edx

.Lpof10:
    movl %edx, TsHardwareEsp(%ebp)

    andl $(~EFLAGS_IOPL_MASK), %ecx

    movl %ecx, %ebx
    andl $(~EFLAGS_NT_MASK), %ebx

    andl $(EFLAGS_INTERRUPT_MASK | EFLAGS_ALIGN_CHECK | EFLAGS_NT_MASK), %ecx

    /*
     * [ecx] = IF + AC + NT
     * [ebx] = User EFlags - IOPL - NT
     */

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Lpof15

    andl $(~(EFLAGS_VIP | EFLAGS_VIF)), %ebx

    testl $(EFLAGS_INTERRUPT_MASK), %ebx
    jz .LpofVifOff

    orl $(EFLAGS_VIF), %ebx

.LpofVifOff:
    orl $(EFLAGS_INTERRUPT_MASK | EFLAGS_V86_MASK), %ebx

    pushl %eax

    movl TsEFlags(%ebp), %eax
    pushl %eax

    andl $(EFLAGS_VIP), %eax
    orl %ebx, %eax

    movl %eax, TsEFlags(%ebp)

    jmp .Lpof20

.Lpof15:
    pushl %eax

    orl $(EFLAGS_INTERRUPT_MASK | EFLAGS_V86_MASK), %ebx

    pushl TsEFlags(%ebp)

    movl %ebx, TsEFlags(%ebp)

.Lpof20:

#ifdef DBG
    testl $(EFLAGS_V86_MASK), %ebx
    jnz .LpofDbg2

    int $3

.LpofDbg2:
#endif

    /*
     * Original:
     *
     * test byte ptr [esp].TsEFlags+2,
     *      EFLAGS_V86_MASK/010000h
     *
     * This is /0x10000, NOT /0x1000000.
     */
    .if ((EFLAGS_V86_MASK & 0x00ff00ff) != 0)
        .error "EFLAGS_V86_MASK is not byte-test compatible"
    .endif

    testb $(EFLAGS_V86_MASK / 0x10000), TsEFlags+2(%esp)

    leal 4(%esp), %esp

    jnz .LpofAdjustDone

    pushl %ebp
    call _Ki386AdjustEsp0@4

.LpofAdjustDone:
    popl %eax

    /*
     * MPLOCK and [eax],NOT (...)
     * MPLOCK or [eax],ecx
     */
    MPLOCK
    andl $(~(EFLAGS_INTERRUPT_MASK | EFLAGS_ALIGN_CHECK | EFLAGS_NT_MASK)), (%eax)

    MPLOCK
    orl %ecx, (%eax)

    addl %edi, TsEip(%ebp)

    movl (%eax), %eax

    testl $(VDM_INTERRUPT_PENDING), %eax
    jz .Lpof25

    testl $(VDM_VIRTUAL_INTERRUPTS), %eax
    jz .Lpof25

    call VdmDispatchIntAck

.Lpof25:
    movl $1, %eax
    ret

.size OpcodePOPFV86,.-OpcodePOPFV86


/*
 * --------------------------------------------------------------------------
 * INT nn
 * --------------------------------------------------------------------------
 */

.globl OpcodeINTnnV86
.type OpcodeINTnnV86,@function
OpcodeINTnnV86:

    movl TsEFlags(%ebp), %edx

    leal FIXED_NTVDMSTATE_LINEAR, %eax

    movl (%eax), %ecx

    MPLOCK
    andl $(~VDM_VIRTUAL_INTERRUPTS), (%eax)

    movl %edx, %eax

    andl $(~EFLAGS_INTERRUPT_MASK), %eax

    andl $(VDM_VIRTUAL_INTERRUPTS | VDM_VIRTUAL_AC), %ecx

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Loinnv15

    andl $(VDM_VIRTUAL_AC), %ecx

    orl %ecx, %eax

    movl %edx, %ecx
    andl $(EFLAGS_VIF), %ecx

    rorl $10, %ecx

.Loinnv15:
    orl %ecx, %eax

.Loinnv20:
    andl $(~(EFLAGS_NT_MASK | EFLAGS_TF_MASK | EFLAGS_VIF)), %edx

    movl %edx, TsEFlags(%ebp)

    orl $(EFLAGS_IOPL_MASK), %eax

    movzwl TsHardwareSegSS(%ebp), %ecx
    shll $4, %ecx

    movzwl TsHardwareEsp(%ebp), %edx

    subw $2, %dx
    movw %ax, (%ecx,%edx,1)

    movw TsSegCS(%ebp), %ax

    subw $2, %dx
    movw %ax, (%ecx,%edx,1)

    movzwl TsEip(%ebp), %eax
    addl %edi, %eax
    incl %eax

    subw $2, %dx
    movw %ax, (%ecx,%edx,1)

    movw %dx, TsHardwareEsp(%ebp)

    incl %esi
    movzbl (%esi), %ecx

    /*
     * Check whether this interrupt is hooked by a protected-mode
     * reflector.
     */
    pushl %ecx
    call oinnvuserrefs
    orl %eax, %eax
    popl %ecx

    je .Loinnv30

    /*
     * Encode interrupt number in CS.
     */
    movl %ebx, %eax
    shrl $16, %eax

    subl %ecx, %eax

    shll $4, %ecx
    addl %ecx, %ebx

    jmp .Loinnv40

.Loinnv30:

    /*
     * Not hooked. Pick up the real-mode IVT vector.
     */
    movl (%ecx,4), %ebx

    movl %ebx, %eax
    shrl $16, %eax

.Loinnv40:

    movw %bx, TsEip(%ebp)

    testl $(EFLAGS_V86_MASK), TsEFlags(%ebp)
    jnz .LoinnvCSDone

    orw $(RPL_MASK), %ax

    cmpw $8, %ax
    jae .LoinnvCSDone

    movw $(KGDT_R3_CODE | RPL_MASK), %ax

.LoinnvCSDone:
    movw %ax, TsSegCS(%ebp)

    movl $1, %eax
    ret

.size OpcodeINTnnV86,.-OpcodeINTnnV86


/*
 * --------------------------------------------------------------------------
 * oinnvuserrefs
 * --------------------------------------------------------------------------
 */

.type oinnvuserrefs,@function
oinnvuserrefs:

    pushl %ebp

    pushl $.Loinnvuserrefs_fault
    pushl PCR[PcExceptionList]

    movl %esp, PCR[PcExceptionList]

    movl PCR[PcTeb], %eax
    movl TeVdm(%eax), %eax

    cmpl _MmUserProbeAddress, %eax
    jae .Loinnvuserrefs_fault_resume

    movl VtInterruptTable(%eax), %ebx

    testl %ebx, %ebx
    jz .Loinnvuserrefs_fault_resume

    leal (%ebx,%ecx,8), %ebx

    cmpl _MmUserProbeAddress, %ebx
    jae .Loinnvuserrefs_fault_resume

    testl $(VDM_INT_HOOKED), ViFlags(%ebx)
    jz .Loinnvuserrefs_fault_resume

    leal VtDpmiInfo(%eax), %ebx
    movl VpDosxRmReflector(%ebx), %ebx

    movl $1, %eax

    popl PCR[PcExceptionList]
    addl $4, %esp
    popl %ebp
    ret

.Loinnvuserrefs_fault_resume:

    popl PCR[PcExceptionList]
    addl $4, %esp
    popl %ebp

    xorl %eax, %eax
    ret

.size oinnvuserrefs,.-oinnvuserrefs


/*
 * Exception handler for oinnvuserrefs.
 */
.type oinnvuserrefs_fault,@function
oinnvuserrefs_fault:

    movl 8(%esp), %esp
    jmp .Loinnvuserrefs_fault_resume

.size oinnvuserrefs_fault,.-oinnvuserrefs_fault


/*
 * --------------------------------------------------------------------------
 * INTO
 * --------------------------------------------------------------------------
 */

.globl OpcodeINTOV86
.type OpcodeINTOV86,@function
OpcodeINTOV86:

    xorl %eax, %eax
    ret

.size OpcodeINTOV86,.-OpcodeINTOV86


/*
 * --------------------------------------------------------------------------
 * IRET
 * --------------------------------------------------------------------------
 */

.globl OpcodeIRETV86
.type OpcodeIRETV86,@function
OpcodeIRETV86:

#ifdef DBG
    CurrentIrql

    cmpb $APC_LEVEL, %al
    jge .LirtDbgOk

    int $3

.LirtDbgOk:
#endif

    leal FIXED_NTVDMSTATE_LINEAR, %eax

    movzwl TsHardwareSegSS(%ebp), %ecx
    movzwl TsHardwareEsp(%ebp), %edx

    shll $4, %ecx
    addl %edx, %ecx

    testl $(PREFIX_OPER32), %ebx
    jnz .Lirt50

    movzwl (%ecx), %edi
    movl %edi, TsEip(%ebp)

    movzwl 2(%ecx), %esi

    addl $6, %edx

    movzwl 4(%ecx), %ebx

    movl %edx, TsHardwareEsp(%ebp)
    movl %esi, TsSegCS(%ebp)

.Lirt10:

    andl $(~(EFLAGS_IOPL_MASK | EFLAGS_NT_MASK |
             EFLAGS_VIP | EFLAGS_VIF)), %ebx

    movl %ebx, %ecx

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Lirt15

    orl $(EFLAGS_VIF), %ebx

    testl $(EFLAGS_INTERRUPT_MASK), %ebx
    jnz .Lirt15

    andl $(~EFLAGS_VIF), %ebx

.Lirt15:

    pushl %eax

    orl $(EFLAGS_V86_MASK | EFLAGS_INTERRUPT_MASK), %ebx

    movl TsEFlags(%ebp), %eax
    pushl %eax

    andl $(EFLAGS_VIP), %eax
    orl %ebx, %eax

    movl %eax, TsEFlags(%ebp)

    popl %ebx

    testl $(EFLAGS_V86_MASK), %ebx
    jnz .LirtAdjustDone

    pushl %ecx
    pushl %edx
    pushl %ebp
    call _Ki386AdjustEsp0@4
    popl %edx
    popl %ecx

.LirtAdjustDone:

    popl %eax

    andl $(EFLAGS_INTERRUPT_MASK), %ecx

    MPLOCK
    andl $(~VDM_VIRTUAL_INTERRUPTS), (%eax)

    MPLOCK
    orl %ecx, (%eax)

    movl (%eax), %ebx

    /*
     * Check whether the return address is a BOP.
     */
    shll $4, %esi
    addl %edi, %esi

    cmpl _MmUserProbeAddress, %esi
    jbe .LirtProbeDone

    movl _MmUserProbeAddress, %esi

.LirtProbeDone:

    movw (%esi), %ax

    cmpw $0xc4c4, %ax
    je .LirtBop

    testl $(VDM_INTERRUPT_PENDING), %ebx
    jz .Lirt25

    testl $(VDM_VIRTUAL_INTERRUPTS), %ebx
    jz .Lirt25

    call VdmDispatchIntAck

.Lirt25:
    movl $1, %eax
    ret


.LirtBop:

    pushl %ebp
    call _VdmDispatchBop@4

    jmp .Lirt25


.Lirt50:

    movl (%ecx), %edi
    movl %edi, TsEip(%ebp)

    movzwl 4(%ecx), %esi

    addl $12, %edx

    movl 8(%ecx), %ebx

    movl %esi, TsSegCS(%ebp)
    movl %edx, TsHardwareEsp(%ebp)

    jmp .Lirt10

.size OpcodeIRETV86,.-OpcodeIRETV86


/*
 * --------------------------------------------------------------------------
 * INB immediate
 * --------------------------------------------------------------------------
 */

.globl OpcodeINBimmV86
.type OpcodeINBimmV86,@function
OpcodeINBimmV86:

    incl %esi
    incl %edi

    movzbl (%esi), %ecx

    pushl %ebp
    pushl %edi
    pushl $TRUE
    pushl $1
    pushl %ecx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeINBimmV86,.-OpcodeINBimmV86


/*
 * --------------------------------------------------------------------------
 * INW immediate
 * --------------------------------------------------------------------------
 */

.globl OpcodeINWimmV86
.type OpcodeINWimmV86,@function
OpcodeINWimmV86:

    incl %esi
    incl %edi

    movzbl (%esi), %ecx

    pushl %ebp
    pushl %edi
    pushl $TRUE
    pushl $2
    pushl %ecx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeINWimmV86,.-OpcodeINWimmV86


/*
 * --------------------------------------------------------------------------
 * OUTB immediate
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTBimmV86
.type OpcodeOUTBimmV86,@function
OpcodeOUTBimmV86:

    incl %edi
    incl %esi

    movzbl (%esi), %ecx

    pushl %ebp
    pushl %edi
    pushl $FALSE
    pushl $1
    pushl %ecx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeOUTBimmV86,.-OpcodeOUTBimmV86


/*
 * --------------------------------------------------------------------------
 * OUTW immediate
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTWimmV86
.type OpcodeOUTWimmV86,@function
OpcodeOUTWimmV86:

    incl %esi
    incl %edi

    movzbl (%esi), %ecx

    pushl %ebp
    pushl %edi
    pushl $FALSE
    pushl $2
    pushl %ecx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeOUTWimmV86,.-OpcodeOUTWimmV86


/*
 * --------------------------------------------------------------------------
 * INB
 * --------------------------------------------------------------------------
 */

.globl OpcodeINBV86
.type OpcodeINBV86,@function
OpcodeINBV86:

    movzwl TsEdx(%ebp), %ebx

    cmpl $0x3bd, %ebx
    je .LoibPrt1

    cmpl $0x379, %ebx
    je .LoibPrt1

    cmpl $0x279, %ebx
    je .LoibPrt1

.LoibReflect:

    pushl %ebp
    pushl %edi
    pushl $TRUE
    pushl $1
    pushl %ebx

    call _Ki386VdmDispatchIo@20
    ret

.LoibPrt1:

    pushl %ebp
    pushl %edi
    pushl %ebx

    call _VdmPrinterStatus@12

    testb %al, %al
    jz .LoibReflect

    ret

.size OpcodeINBV86,.-OpcodeINBV86


/*
 * --------------------------------------------------------------------------
 * INW
 * --------------------------------------------------------------------------
 */

.globl OpcodeINWV86
.type OpcodeINWV86,@function
OpcodeINWV86:

    movzwl TsEdx(%ebp), %ebx

    pushl %ebp
    pushl %edi
    pushl $TRUE
    pushl $2
    pushl %ebx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeINWV86,.-OpcodeINWV86


/*
 * --------------------------------------------------------------------------
 * OUTB
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTBV86
.type OpcodeOUTBV86,@function
OpcodeOUTBV86:

    movzwl TsEdx(%ebp), %ebx

    cmpl $0x3bc, %ebx
    je .LoobPrt1

    cmpl $0x378, %ebx
    je .LoobPrt1

    cmpl $0x278, %ebx
    je .LoobPrt1

.LoobReflect:

    pushl %ebp
    pushl %edi
    pushl $FALSE
    pushl $1
    pushl %ebx

    call _Ki386VdmDispatchIo@20
    ret

.LoobPrt1:

    pushl %ebp
    pushl %edi
    pushl %ebx

    call _VdmPrinterWriteData@12

    testb %al, %al
    jz .LoobReflect

    ret

.size OpcodeOUTBV86,.-OpcodeOUTBV86


/*
 * --------------------------------------------------------------------------
 * OUTW
 * --------------------------------------------------------------------------
 */

.globl OpcodeOUTWV86
.type OpcodeOUTWV86,@function
OpcodeOUTWV86:

    movzwl TsEdx(%ebp), %ebx

    pushl %ebp
    pushl %edi
    pushl $FALSE
    pushl $2
    pushl %ebx

    call _Ki386VdmDispatchIo@20
    ret

.size OpcodeOUTWV86,.-OpcodeOUTWV86


/*
 * --------------------------------------------------------------------------
 * CLI
 * --------------------------------------------------------------------------
 */

.globl OpcodeCLIV86
.type OpcodeCLIV86,@function
OpcodeCLIV86:

    leal FIXED_NTVDMSTATE_LINEAR, %eax

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Loc50

    movl TsEFlags(%ebp), %edx
    movl (%eax), %eax

    andl $(EFLAGS_VIF | EFLAGS_VIP), %edx

    cmpl $(EFLAGS_VIF | EFLAGS_VIP), %edx
    jne .Loc40

    testl $(VDM_INTERRUPT_PENDING), %eax
    jz .Loc40

    call VdmDispatchIntAck

    movl $1, %eax
    ret

.Loc40:
    leal FIXED_NTVDMSTATE_LINEAR, %eax

.Loc50:
    MPLOCK
    andl $(~VDM_VIRTUAL_INTERRUPTS), (%eax)

    addl %edi, TsEip(%ebp)

    movl $1, %eax
    ret

.size OpcodeCLIV86,.-OpcodeCLIV86


/*
 * --------------------------------------------------------------------------
 * STI
 * --------------------------------------------------------------------------
 */

.globl OpcodeSTIV86
.type OpcodeSTIV86,@function
OpcodeSTIV86:

    leal FIXED_NTVDMSTATE_LINEAR, %eax

    testl $(V86_VIRTUAL_INT_EXTENSIONS), _KeI386VirtualIntExtensions
    jz .Los10

    orl $(EFLAGS_VIF), TsEFlags(%ebp)

.Los10:
    MPLOCK
    orl $(EFLAGS_INTERRUPT_MASK), (%eax)

.Los20:
    addl %edi, TsEip(%ebp)

    movl (%eax), %eax

    testl $(VDM_INTERRUPT_PENDING), %eax
    jz .Los30

    call VdmDispatchIntAck

.Los30:
    movl $1, %eax
    ret

.size OpcodeSTIV86,.-OpcodeSTIV86


/*
 * --------------------------------------------------------------------------
 * Opcode0F
 * --------------------------------------------------------------------------
 */

.globl Opcode0FV86
.type Opcode0FV86,@function
Opcode0FV86:

    /*
     * RI is the local REGINFO structure.
     *
     * Original:
     *
     *     RI equ [ebp - REGINFOSIZE]
     *
     * In GAS, use the explicit base-relative form.
     */

    pushl %ebp
    movl %esp, %ebp

    subl $REGINFOSIZE, %esp

    pushl %esi
    pushl %edi

.Ldo10:

    movl (%ebp), %esi

    /*
     * Initialize trap-frame fields which were not initialized for V86.
     */
    movl TsV86Es(%esi), %eax
    movl %eax, TsSegEs(%esi)

    movl TsV86Ds(%esi), %eax
    movl %eax, TsSegDs(%esi)

    movl TsV86Fs(%esi), %eax
    movl %eax, TsSegFs(%esi)

    movl TsV86Gs(%esi), %eax
    movl %eax, TsSegGs(%esi)

    movl %esi, RiTrapFrame(%ebp)

    movl TsHardwareSegSS(%esi), %eax
    movl %eax, RiSegSs(%ebp)

    movl TsHardwareEsp(%esi), %eax
    movl %eax, RiEsp(%ebp)

    movl TsEFlags(%esi), %eax
    movl %eax, RiEFlags(%ebp)

    movl TsSegCs(%esi), %eax
    movl %eax, RiSegCs(%ebp)

    movl TsEip(%esi), %eax

    decl %edi
    addl %edi, %eax

    movl %eax, RiEip(%ebp)

    movl %ebx, RiPrefixFlags(%ebp)

    leal 0(%ebp), %esi

    /*
     * CsToLinearV86 operates on the RegInfo structure.
     */
    CsToLinearV86

    /*
     * VdmOpcode0f enables interrupts.
     */
    call VdmOpcode0f

    testl $0xffff, %eax
    jz .Ldo20

    movl RiTrapFrame(%ebp), %edi
    movl RiEip(%ebp), %eax

    movl %eax, TsEip(%edi)

.Ldo19:
    movl $1, %eax

.Ldo20:

    popl %edi
    popl %esi

    movl %ebp, %esp
    popl %ebp
    ret

.size Opcode0FV86,.-Opcode0FV86


/*
 * --------------------------------------------------------------------------
 * VdmDispatchIntAck
 * --------------------------------------------------------------------------
 */

.globl VdmDispatchIntAck
.type VdmDispatchIntAck,@function
VdmDispatchIntAck:

    pushl %ebp

    pushl $.Ldiafault
    pushl PCR[PcExceptionList]

    movl %esp, PCR[PcExceptionList]

    testl $(VDM_INT_HARDWARE), FIXED_NTVDMSTATE_LINEAR

    movl PCR[PcTeb], %eax
    movl TeVdm(%eax), %eax

    jz .Ldia20

    cmpl _MmUserProbeAddress, %eax
    jae .Ldia10

    /*
     * Remove exception handler.
     */
    popl PCR[PcExceptionList]
    addl $4, %esp
    popl %ebp

    /*
     * Hardware interrupt.
     */
    pushl %eax
    pushl %ebp
    call _VdmDispatchInterrupts@8

    ret

.Ldia10:

    popl PCR[PcExceptionList]
    addl $4, %esp
    popl %ebp
    ret


/*
 * Timer interrupt: switch to monitor context.
 */
.Ldia20:

    cmpl _MmUserProbeAddress, %eax
    jae .Ldia10

    movl $VdmIntAck, VtEIEvent(%eax)
    movl $0, VtEIInstSize(%eax)
    movl $0, VtEiIntAckInfo(%eax)

    popl PCR[PcExceptionList]

    addl $4, %esp
    popl %ebp

    pushl %eax
    pushl %ebp
    call _VdmEndExecution@8

    ret

.size VdmDispatchIntAck,.-VdmDispatchIntAck


/*
 * --------------------------------------------------------------------------
 * diafault
 * --------------------------------------------------------------------------
 */

.type diafault,@function
.Ldiafault:

    movl 8(%esp), %esp

    popl PCR[PcExceptionList]
    addl $4, %esp

    popl %ebp
    ret

.size diafault,.-diafault


/*
 * --------------------------------------------------------------------------
 * vdmDebugPoint
 * --------------------------------------------------------------------------
 */

.globl vdmDebugPoint
.type vdmDebugPoint,@function
vdmDebugPoint:
    ret
.size vdmDebugPoint,.-vdmDebugPoint


/*
 * --------------------------------------------------------------------------
 * HLT
 * --------------------------------------------------------------------------
 */

.globl OpcodeHLTV86
.type OpcodeHLTV86,@function
OpcodeHLTV86:

    addl %edi, TsEip(%ebp)

    movl $1, %eax
    ret

.size OpcodeHLTV86,.-OpcodeHLTV86


/*
 * --------------------------------------------------------------------------
 * NPX Opcode Handler
 *
 * This routine was deliberately moved to the non-pageable text section
 * in the original source.
 * --------------------------------------------------------------------------
 */

.globl OpcodeNPXV86
.type OpcodeNPXV86,@function
OpcodeNPXV86:

    movl PCR[PcPrcbData+PbCurrentThread], %edx
    movl ThInitialStack(%edx), %edx

    subl $NPX_FRAME_LENGTH, %edx
    movl FpCr0NpxState(%edx), %edx

    testl $(CR0_EM), %edx
    jnz .Lonp40

.Lonp30:

    andl $(PREFIX_ADDR32), %ebx

    pushl %edi
    pushl %esi
    pushl %ebx
    pushl %ebp

    call _VdmSkipNpxInstruction@16

    testb %al, %al
    jnz .Lonp60

.Lonp40:

    pushl $7
    call _Ki386VdmReflectException@4

.Lonp60:

    movl $1, %eax
    ret

.size OpcodeNPXV86,.-OpcodeNPXV86


/*
 * --------------------------------------------------------------------------
 * KiVdmSetUserCR0
 * --------------------------------------------------------------------------
 */

.globl KiVdmSetUserCR0
.type KiVdmSetUserCR0,@function
KiVdmSetUserCR0:

    /*
     * Sanitize parameter.
     */
    andl $(CR0_MP | CR0_EM), %eax
    shrl $1, %eax

    movzbl _VdmUserCr0MapIn(%eax), %eax

    /*
     * Install exception handler.
     */
    pushl %ebp
    pushl $.Lscr_fault
    pushl PCR[PcExceptionList]

    movl %esp, PCR[PcExceptionList]

    movl PCR[PcTeb], %edx
    movl TeVdm(%edx), %edx

    cmpl _MmUserProbeAddress, %edx
    jbe .LscrStore

    movl _MmUserProbeAddress, %edx

.LscrStore:

    movl %eax, VtVdmContext+CsFloatSave+FpCtxtCr0NpxState(%edx)

.Lscr10:

    popl PCR[PcExceptionList]

    movl PCR[PcPrcbData+PbCurrentThread], %ebx

    addl $4, %esp
    popl %ebp

    movl ThInitialStack(%ebx), %edx

    subl $NPX_FRAME_LENGTH, %edx

.Lscr20:

    /*
     * Synchronize with context switch.
     */
    cli

    andl $(~(CR0_MP | CR0_EM | CR0_PE)), FpCr0NpxState(%edx)

    orl %eax, FpCr0NpxState(%edx)

    movl %cr0, %eax

    andl $(~(CR0_MP | CR0_EM | CR0_TS)), %eax

    orb ThNpxState(%ebx), %al

    orl FpCr0NpxState(%edx), %eax

    movl %eax, %cr0

    sti
    ret

.size KiVdmSetUserCR0,.-KiVdmSetUserCR0


/*
 * --------------------------------------------------------------------------
 * scr_fault
 * --------------------------------------------------------------------------
 */

.type scr_fault,@function
.Lscr_fault:

    movl 8(%esp), %esp
    jmp .Lscr10

.size scr_fault,.-scr_fault
