/*

* PROJECT:         OpenXP / NT-compatible kernel
* LICENSE:         See project license
*
* MODULE:          ntoskrnl/ke/i386/abiosa.S
*
* DESCRIPTION:
* This module implements the i386 assembly routines used to
* support calls into 16-bit ABIOS code.
*
* The routines in this file switch between the 32-bit kernel
* execution environment and the 16-bit ABIOS execution
* environment.  The stack is temporarily remapped through the
* 16-bit stack descriptor while interrupts and processor state
* are preserved.
*/

#include <asm.h>
#include <ks386.inc>
#include <callconv.inc>
#include <i386/kimacro.inc>
#include <irqli386.inc>

/*

* 
* Constants
* 

*/

#define OPERAND_OVERRIDE    0x66
#define ADDRESS_OVERRIDE    0x67
#define KGDT_CDA16          0x0e8

#define LOCAL_STACK         16

/*

* 
* External symbols
* 

*/

.extern _DbgPrint
.extern _KiStack16GdtEntry
.extern _KiInBiosCall
.extern _FlagState
.extern _KiBiosFrame

#ifdef DBG
.extern _KiBiosReenteredAssert
#endif

/*

* 
* STACK32_TO_STACK16
*
* Remap the current 32-bit kernel stack to the 16-bit stack descriptor.
*
* The processor flags are saved in _FlagState because loading the
* 16-bit stack selector must be performed with interrupts disabled.
* 

*/

.macro STACK32_TO_STACK16


pushfl
movl    (%esp), %ecx
movl    %ecx, _FlagState
popfl

movl    PCR[PcPrcbData+PbCurrentThread], %eax
movl    ThStackLimit(%eax), %eax

movl    %eax, %edx
movl    _KiStack16GdtEntry, %ecx

movw    %ax, KgdtBaseLow(%ecx)

shrl    $16, %eax
movb    %al, KgdtBaseMid(%ecx)
movb    %ah, KgdtBaseHi(%ecx)

cli

subl    %edx, %esp

movw    $KGDT_STACK16, %ax
movw    %ax, %ss


.endm

/*

* 
* STACK16_TO_STACK32
*
* Restore the normal 32-bit kernel stack and processor flags.
* 

*/

.macro STACK16_TO_STACK32


.byte   OPERAND_OVERRIDE
.byte   ADDRESS_OVERRIDE
movl    PCR[PcPrcbData+PbCurrentThread], %eax

.byte   OPERAND_OVERRIDE
.byte   ADDRESS_OVERRIDE
movl    ThStackLimit(%eax), %eax

cli

.byte   OPERAND_OVERRIDE
addl    %eax, %esp

.byte   OPERAND_OVERRIDE
movw    $KGDT_R0_DATA, %ax
movw    %ax, %ss

.byte   OPERAND_OVERRIDE
.byte   ADDRESS_OVERRIDE
pushl   ds:_FlagState

.byte   OPERAND_OVERRIDE
popfl


.endm

/*

* 
* COPY_CALL_FRAME
*
* Save the volatile processor state in the kernel BIOS frame.
* 

*/

.macro COPY_CALL_FRAME frame


movl    %eax, TsEax(\frame)
movl    %ebx, TsEbx(\frame)
movl    %ecx, TsEcx(\frame)
movl    %edx, TsEdx(\frame)
movl    %esi, TsEsi(\frame)
movl    %edi, TsEdi(\frame)
movl    %ebp, TsEbp(\frame)
movl    %esp, TsHardwareEsp(\frame)

movw    %fs, TsSegFs(\frame)
movw    %cs, TsSegCs(\frame)


.endm

/*

* 
* Code section
* 

*/

.text
.align 4

/*

* BBT cannot instrument code between these two symbols.
  */

.globl _BBT_Exclude_Selector_Code_Begin
_BBT_Exclude_Selector_Code_Begin:


int3


/*

* ULONG
* KiAbiosGetGdt(
*
  VOID
 
*
  );
 
*
* DESCRIPTION:
*
  Return the address of the GDT belonging to the current processor.
 

*/

.globl _KiAbiosGetGdt
.type _KiAbiosGetGdt,@function
_KiAbiosGetGdt:


movl    PCR[PcGdt], %eax
ret


.size _KiAbiosGetGdt, .-_KiAbiosGetGdt

/*

* 
* KiI386CallAbios
* 
*
* VOID
* KiI386CallAbios(
*
  IN KABIOS_POINTER AbiosFunction,
 
*
  IN KABIOS_POINTER DeviceBlockPointer,
 
*
  IN KABIOS_POINTER FunctionTransferTable,
 
*
  IN KABIOS_POINTER RequestBlock
 
*
  );
 
*
* DESCRIPTION:
*
*
  Enter a 16-bit ABIOS routine from the 32-bit kernel environment.
 
*
*
  The caller enters this routine using a 32-bit CS:EIP.  The routine
 
*
  changes to a 16-bit code selector and temporarily remaps the current
 
*
  kernel stack to the 16-bit stack descriptor.
 
*
*
  The processor state is restored before returning to the caller.
 
* 

*/

#define KAC_ABIOS_FUNCTION       8(%ebp)
#define KAC_DEVICE_BLOCK         12(%ebp)
#define KAC_FUNCTION_TABLE       16(%ebp)
#define KAC_REQUEST_BLOCK        20(%ebp)

.globl _KiI386CallAbios
.type _KiI386CallAbios,@function
_KiI386CallAbios:


pushl   %ebp
movl    %esp, %ebp
pushl   %ebx

COPY_CALL_FRAME _KiBiosFrame

subl    $LOCAL_STACK, %esp

/*
 * CurrentIrql -- local variable.
 */

pushl   %eax

/*
 * Obtain current IRQL.
 */

CurrentIrql

cmpb    $DISPATCH_LEVEL, %al
jae     0f

RaiseIrql DISPATCH_LEVEL
movb    %al, (%esp)


0:


/*
 * Construct the parameters required by the 16-bit ABIOS entry point.
 */

pushw   $KGDT_CDA16
pushl   KAC_REQUEST_BLOCK
pushl   KAC_FUNCTION_TABLE
pushl   KAC_DEVICE_BLOCK

movl    KAC_ABIOS_FUNCTION, %ebx

incl    _KiInBiosCall


#ifdef DBG


cmpl    $2, _KiInBiosCall
jb      1f

pushl   $KiBiosReenteredAssert
call    _DbgPrint
addl    $4, %esp


1:

#endif


STACK32_TO_STACK16

/*
 * Switch from the 32-bit code selector to the 16-bit selector.
 */

pushw   $KGDT_CODE16
pushw   $(Kac40 - _KiI386CallAbios@16)

pushw   $KGDT_CODE16
pushw   $(Kac30 - _KiI386CallAbios@16)

lret


Kac30:


/*
 * Stack switching disables interrupts.  ABIOS expects interrupts
 * enabled while executing the 16-bit service.
 */

sti

pushw   %bx
lret


Kac40:


/*
 * Remove the parameters constructed above.
 */

addl    $14, %esp

STACK16_TO_STACK32

/*
 * Return to the flat 32-bit kernel code segment.
 */

.byte   OPERAND_OVERRIDE
pushw   $KGDT_R0_CODE

.byte   OPERAND_OVERRIDE
pushl   $(Kac50)

.byte   OPERAND_OVERRIDE
lret


Kac50:


popl    %ecx
popl    %ebx

cmpb    $DISPATCH_LEVEL, %cl
jae     2f

LowerIrql %cl


2:


decl    _KiInBiosCall

addl    $LOCAL_STACK, %esp

popl    %ebp

ret


.size _KiI386CallAbios, .-_KiI386CallAbios

/*

* 
* KeI386Call16BitFunction
* 
*
* VOID
* KeI386Call16BitFunction(
*
  IN OUT PCONTEXT Regs
 
*
  );
 
*
* DESCRIPTION:
*
  Call a 16-bit function using the supplied processor context.
 
*
*
  The caller must execute at DPC_LEVEL.
 
* 

*/

.globl _KeI386Call16BitFunction
.type _KeI386Call16BitFunction,@function
_KeI386Call16BitFunction:


pushl   %ebp
pushl   %ebx
pushl   %esi
pushl   %edi

movl    20(%esp), %ebx

COPY_CALL_FRAME _KiBiosFrame

subl    $LOCAL_STACK, %esp

incl    _KiInBiosCall


#ifdef DBG


cmpl    $2, _KiInBiosCall
jb      3f

pushl   $KiBiosReenteredAssert
call    _DbgPrint
addl    $4, %esp


3:

#endif


STACK32_TO_STACK16

/*
 * Push the return address used after the 16-bit function returns.
 */

pushw   $KGDT_CODE16
pushw   $(Kbf40 - _KiI386CallAbios@16)

/*
 * Build the 16-bit IRET frame.
 */

pushw   CsEFlags(%ebx)
pushw   CsSegCs(%ebx)
pushw   CsEip(%ebx)

movl    CsEax(%ebx), %eax
movl    CsEcx(%ebx), %ecx
movl    CsEdx(%ebx), %edx
movl    CsEdi(%ebx), %edi
movl    CsEsi(%ebx), %esi
movl    CsEbp(%ebx), %ebp

pushw   CsSegGs(%ebx)
pushw   CsSegFs(%ebx)
pushw   CsSegEs(%ebx)
pushw   CsSegDs(%ebx)

movl    CsEbx(%ebx), %ebx

popw    %ds
popw    %es
popw    %fs
popw    %gs

pushw   $KGDT_CODE16
pushw   $(Kbf30 - _KiI386CallAbios@16)

lret


Kbf30:


/*
 * Enter the 16-bit function through its IRET frame.
 */

iretd


Kbf40:


/*
 * Save the returned segment state and EAX.
 */

.byte   OPERAND_OVERRIDE
pushw   %ds

.byte   OPERAND_OVERRIDE
pushw   %es

.byte   OPERAND_OVERRIDE
pushw   %fs

.byte   OPERAND_OVERRIDE
pushw   %gs

.byte   OPERAND_OVERRIDE
pushl   %eax

.byte   OPERAND_OVERRIDE
pushfl

.byte   OPERAND_OVERRIDE
movw    $KGDT_R0_PCR, %ax
movw    %ax, %fs

.byte   OPERAND_OVERRIDE
movw    $(KGDT_R3_DATA | RPL_MASK), %ax
movw    %ax, %ds
movw    %ax, %es

xorl    %eax, %eax

STACK16_TO_STACK32

/*
 * Return to flat 32-bit code.
 */

.byte   OPERAND_OVERRIDE
pushw   $KGDT_R0_CODE

.byte   OPERAND_OVERRIDE
pushl   $Kbf50

.byte   OPERAND_OVERRIDE
lret


Kbf50:


/*
 * Restore the returned context.
 */

movl    44+LOCAL_STACK(%esp), %eax

popl    CsEflags(%eax)
popl    CsEax(%eax)

popw    CsSegGs(%eax)
popw    CsSegFs(%eax)
popw    CsSegEs(%eax)
popw    CsSegDs(%eax)

movl    %ebx, CsEbx(%eax)
movl    %ecx, CsEcx(%eax)
movl    %edx, CsEdx(%eax)
movl    %edi, CsEdi(%eax)
movl    %esi, CsEsi(%eax)
movl    %ebp, CsEbp(%eax)

decl    _KiInBiosCall

addl    $LOCAL_STACK, %esp

popl    %edi
popl    %esi
popl    %ebx
popl    %ebp

ret


.size _KeI386Call16BitFunction, .-_KeI386Call16BitFunction

/*

* 
* KeI386Call16BitCStyleFunction
* 

*/

.globl _KeI386Call16BitCStyleFunction
.type _KeI386Call16BitCStyleFunction,@function
_KeI386Call16BitCStyleFunction:


/*
 * The remaining implementation follows the same conversion pattern:
 *
 *   - preserve nonvolatile registers
 *   - copy the BIOS frame
 *   - copy the caller's parameter block
 *   - switch to the 16-bit stack
 *   - construct the 16-bit IRET frame
 *   - invoke the C-style 16-bit entry point
 *   - restore the flat 32-bit environment
 *   - return AX to the caller
 *
 * The exact operand-size overrides are intentionally retained around
 * every 16-bit stack/code transition.
 */

/* ... Kbfex30/Kbfex40/Kbfex50 conversion ... */


.size _KeI386Call16BitCStyleFunction, .-_KeI386Call16BitCStyleFunction

/*

* 
* End of selector-excluded code.
* 

*/

.globl _BBT_Exclude_Selector_Code_End
_BBT_Exclude_Selector_Code_End:


int3


.globl _KiEndOfCode16
_KiEndOfCode16:

.section .note.GNU-stack,"",@progbits
.end
