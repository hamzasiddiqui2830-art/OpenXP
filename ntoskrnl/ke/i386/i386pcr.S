/*
 * title  "I386 PCR"
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * Module Name:
 *
 *    i386pcr.S
 *
 * Abstract:
 *
 *    This module implements routines for accessing and initializing the pcr.
 *
 * Environment:
 *
 *    Kernel mode, early init of first processor.
 */

#define NT386 1

#include "ks386.inc"
#include "callconv.inc"

/*
 * NOTE - This definition of PCR gives us 2 instructions to get to some
 *        variables that need to be addressable in one instruction.  Any
 *        such variable (such as current thread) must be accessed via its
 *        own access procedure (see below), NOT by KeGetPcr()->PbCurrentThread.
 *        (This is only an issue on MP machines.)
 */

.section .text$00,"xr"


/*
 *++
 *
 * PKTHREAD
 * KeGetCurrentThread()
 *
 * Return Value:
 *
 *   Pointer to current Thread object.
 *
 *--
 */

.globl ___KeGetCurrentThread
.type  ___KeGetCurrentThread,@function

___KeGetCurrentThread:

        movl    %fs:(PcPrcbData + PbCurrentThread), %eax

        ret

.size ___KeGetCurrentThread, .-___KeGetCurrentThread


/*
 *++
 *
 * KPROCESSOR_MODE
 * KeGetPreviousMode()
 *
 * Return Value:
 *
 *   PreviousMode of current thread.
 *
 *--
 */

.globl _KeGetPreviousMode
.type  _KeGetPreviousMode,@function

_KeGetPreviousMode:

        movl    %fs:(PcPrcbData + PbCurrentThread), %eax
                                                /* (eax) -> Thread */

        movzbl  ThPreviousMode(%eax), %eax       /* (eax) = PreviousMode */

        ret

.size _KeGetPreviousMode, .-_KeGetPreviousMode


/*
 *++
 *
 * BOOLEAN
 * KeIsExecutingDpc(
 *       VOID
 *       );
 *
 * Return Value:
 *
 *   Value of flag which indicates whether we're executing in DPC context
 *
 *--
 */

.globl ___KeIsExecutingDpc
.type  ___KeIsExecutingDpc,@function

___KeIsExecutingDpc:

        movzbl  %fs:(PcPrcbData + PbDpcRoutineActive), %eax

        ret

.size ___KeIsExecutingDpc, .-___KeIsExecutingDpc


/*
 *++
 *
 * VOID
 * GetMachineBootPointers(
 *       )
 *
 * Routine Description:
 *
 *   This routine is called at system startup to extract the address of
 *   the PCR and machine control values.  It is useful only for the P0
 *   case where the boot loader must already init the machine before it
 *   turns on paging and calls us.
 *
 *   Pcr address is extracted from the base of KGDT_R0_PCR.
 *
 *   Gdt and Idt are extracted from the machine GDTR and IDTR.
 *
 *   TSS is derived from the TSR and related descriptor.
 *
 * Arguments:
 *
 *   None.
 *
 * Return Value:
 *
 *
 *   (edi) -> gdt
 *   (esi) -> pcr
 *   (edx) -> tss
 *   (eax) -> idt
 *
 *--
 */

.globl GetMachineBootPointers
.type  GetMachineBootPointers,@function

GetMachineBootPointers:

        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp


        /*
         * sgdt fword ptr [ebp-8]
         *
         * The GDTR is:
         *   offset 0 : 16-bit limit
         *   offset 2 : 32-bit base
         */

        sgdt    -8(%ebp)

        movl    -6(%ebp), %edi             /* (edi) = gdt address */


        /*
         * Extract the selector used by FS and convert it to a
         * GDT index by removing the RPL bits.
         */

        movw    %fs, %cx
        andw    $(~RPL_MASK), %cx
        movzwl  %cx, %ecx

        addl    %edi, %ecx                 /* (ecx) -> pcr descriptor */


        /*
         * Reconstruct the 32-bit PCR descriptor base:
         *
         *     Base = BaseLow | BaseMid << 16 | BaseHi << 24
         */

        movb    KgdtBaseHi(%ecx), %dh
        movb    KgdtBaseMid(%ecx), %dl

        shll    $16, %edx

        movw    KgdtBaseLow(%ecx), %dx     /* (edx) -> pcr */

        movl    %edx, %esi                 /* (esi) -> pcr */


        /*
         * Get the current task register selector.
         */

        str     %cx
        movzwl  %cx, %ecx

        addl    %edi, %ecx                 /* (ecx) -> TSS descriptor */


        /*
         * Reconstruct the 32-bit TSS descriptor base.
         */

        movb    KgdtBaseHi(%ecx), %dh
        movb    KgdtBaseMid(%ecx), %dl

        shll    $16, %edx

        movw    KgdtBaseLow(%ecx), %dx     /* (edx) -> TSS */


        /*
         * Read the IDTR.
         */

        sidt    -8(%ebp)

        movl    -6(%ebp), %eax             /* (eax) -> Idt */


        /*
         * Epilogue.
         */

        movl    %ebp, %esp
        popl    %ebp

        ret

.size GetMachineBootPointers, .-GetMachineBootPointers
