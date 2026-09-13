/*

* title  "Call Out to User Mode"
*
* ++
*
* Copyright (c) OpenXP
*
*
* Module Name:
*
* callout.S
*
* Abstract:
*
* This module implements the code necessary to call out from kernel
* mode to user mode.
*
* 
  */

#include <asm.h>
#include <ks386.inc>
#include <i386/kimacro.inc>
#include <callconv.inc>
#include <irqli386.inc>

/*

* External routines.
  */

.extern _KiServiceExit
.extern _KeUserCallbackDispatcher

.extern _KeBugCheck2
.extern _MmGrowKernelStack

#define KCB_V86_BIAS       (TsV86Gs - TsHardwareSegSs)

#define SsStkBs            4
#define SsStkLm            8

/*

* 
*
* NTSTATUS
* KiCallUserMode(
* 
  IN PVOID *OutputBuffer,
  
* 
  IN PULONG OutputLength
  
* 
  )
  
*
* Routine Description:
*
* This function calls a user mode function from kernel mode.
*
* N.B. This function calls out to user mode and the NtCallbackReturn
* 
     function returns back to the caller of this function. Therefore,
  
* 
     the stack layout must be consistent between the two routines.
  
*
* Arguments:
*
* OutputBuffer - Supplies a pointer to the variable that receives
* 
     the address of the output buffer.
  
*
* OutputLength - Supplies a pointer to a variable that receives
* 
     the length of the output buffer.
  
*
* Return Value:
*
* The final status of the call out function is returned as the status
* of the function.
*
* N.B. This function does not return to its caller. A return to the
* 
     caller is executed when a NtCallbackReturn system service is
  
* 
     executed.
  
*
* N.B. This function does return to its caller if a kernel stack
* 
      expansion is required and the attempted expansion fails.
  
*
* 

*/

/*

* To support the debugger, the callback stack frame is now defined in
* i386.h. If the stack frame is changed, i386.h must be updated and
* geni386 rebuilt and run, then rebuild this file and ntos\kd.
*
* The FPO record below must also be updated to correctly represent
* the stack frame.
  */

/*

* cPublicProc _KiCallUserMode, 2
*
* The OpenXP/ReactOS assembler compatibility layer should provide
* the appropriate public-function declaration macro.
  */

FUNC KiCallUserMode, 2

/*

* .FPO (3, 2, 4, 4, 0, 0)
*
* FPO metadata is normally generated/expressed by the PE/COFF
* compatibility macros in the ReactOS/OpenXP assembler environment.
  */

/*

* Save nonvolatile registers.
  */

  
   pushl   %ebp
   pushl   %ebx
   pushl   %esi
   pushl   %edi

   movl    PCR[PcPrcbData + PbCurrentThread], %ebx
  

/*

* Check if the current IRQL is above passive level.
  */

#if DBG


    CURRENT_IRQL

    orb     %al, %al
    jz      Kcb00

    /*
     * stdCall _KeBugCheck2,
     *     <IRQL_GT_ZERO_AT_SYSTEM_SERVICE,0,eax,0,0,0>
     */

    pushl   $0
    pushl   $0
    pushl   %eax
    pushl   $0
    pushl   $0
    pushl   $IRQL_GT_ZERO_AT_SYSTEM_SERVICE
    call    _KeBugCheck2
    addl    $24, %esp


/*

* Check if kernel APCs are disabled or a process is attached.
  */

Kcb00:


    movzbl  ThApcStateIndex(%ebx), %eax

    movl    ThCombinedApcDisable(%ebx), %edx

    orl     %eax, %eax
    jnz     Kcb05

    orl     %edx, %edx
    jz      Kcb07


Kcb05:


    /*
     * stdCall _KeBugCheck2,
     *     <APC_INDEX_MISMATCH,0,eax,edx,0,0>
     */

    pushl   $0
    pushl   $0
    pushl   %edx
    pushl   %eax
    pushl   $0
    pushl   $APC_INDEX_MISMATCH
    call    _KeBugCheck2
    addl    $24, %esp


#endif

/*

* Check if sufficient room is available on the kernel stack for another
* system call.
  */

Kcb07:


    leal    -KERNEL_LARGE_STACK_COMMIT(%esp), %eax

    cmpl    ThStackLimit(%ebx), %eax
    jae     Kcb10

    /*
     * stdCall _MmGrowKernelStack,<esp>
     */

    pushl   %esp
    call    _MmGrowKernelStack
    addl    $4, %esp

    orl     %eax, %eax
    jne     Kcb20


/*

* Get the address of the current thread and save the previous trap frame
* and callback stack addresses in the current frame. Also save the new
* callback stack address in the thread object.
  */

Kcb10:


    pushl   ThCallbackStack(%ebx)

    movl    ThTrapFrame(%ebx), %edx
    pushl   %edx

    movl    ThInitialStack(%ebx), %esi
    pushl   %esi

    movl    %esp, ThCallbackStack(%ebx)


KcbPrologEnd:

/*

* Copy the numeric save area from the previous save area to the new save
* area and establish a new initial kernel stack.
*
* Make sure that the destination NPX Save area is 16-byte aligned
* as required by fxsave/fxrstor.
  */

  
   andl    $0xfffffff0, %esp

   movl    %esp, %edi

   subl    $NPX_FRAME_LENGTH, %esp
   subl    $NPX_FRAME_LENGTH, %esi

   cli

   movl    FpControlWord(%esi), %ecx
   movl    %ecx, FpControlWord(%esp)

   movl    FpStatusWord(%esi), %ecx
   movl    %ecx, FpStatusWord(%esp)

   movl    FpTagWord(%esi), %ecx
   movl    %ecx, FpTagWord(%esp)

   movl    FxMXCsr(%esi), %ecx
   movl    %ecx, FxMXCsr(%esp)

   movl    FpCr0NpxState(%esi), %ecx
   movl    %ecx, FpCr0NpxState(%esp)

   movl    PCR[PcTss], %esi

   movl    %edi, ThInitialStack(%ebx)

   movl    %esp, %ecx

   subl    $KCB_V86_BIAS, %esp
  

#if ((EFLAGS_V86_MASK & 0xff00ffff) != 0)
#error "EFLAGS_V86_MASK layout is incompatible with callout.S"
#endif


    testb   $(EFLAGS_V86_MASK / 0x01000000), TsEFlags+2(%edx)
    jne     1f

    /*
     * Normal trap-frame case: use the adjusted ESP.
     */

    movl    %esp, %ecx


1:
movl    %ecx, TssEsp0(%esi)

/*

* Construct a trap frame to facilitate the transfer into user mode via
* the standard system call exit.
  */

  
   subl    $(TsHardwareSegSs + 4), %esp

   movl    %esp, %ebp

   movl    $((TsHardwareSegSs - TsSegFs + 4) / 4), %ecx

   leal    TsSegFs(%esp), %edi
   leal    TsSegFs(%edx), %esi

   rep
   movsl
  

/*

* Fetch control register.
*
* Do we need to restore debug registers?
  */

  
   movl    TsDr7(%edx), %edi

   testl   $(~DR7_RESERVED_MASK), %edi

   movl    %edi, TsDr7(%esp)

   jnz     Kcb18
  

Kcb15:


    /*
     * Get address of callback dispatcher.
     */

    movl    _KeUserCallbackDispatcher, %eax
    movl    %eax, TsEip(%esp)

    /*
     * Get current exception list.
     */

    movl    PCR[PcExceptionList], %eax
    movl    %eax, TsExceptionList(%esp)

    /*
     * Get previous mode.
     */

    movl    TsPreviousPreviousMode(%edx), %eax
    movl    %eax, TsPreviousPreviousMode(%esp)

    sti


    /*
     * SET_DEBUG_DATA
     */

    SET_DEBUG_DATA


    /*
     * Exit through the standard system-call dispatcher.
     */

    jmp     _KiServiceExit


Kcb18:


    /*
     * Copy debug registers from the current frame.
     */

    movl    $((TsDr6 - TsDr0 + 4) / 4), %ecx

    leal    TsDr0(%esp), %edi
    leal    TsDr0(%edx), %esi

    rep
    movsl

    jmp     Kcb15


/*

* An attempt to grow the kernel stack failed.
  */

Kcb20:


    popl    %edi
    popl    %esi
    popl    %ebx
    popl    %ebp

    RET_FUNC KiCallUserMode


ENDPROC KiCallUserMode

/*

* 
*
* PVOID
* KeSwitchKernelStack(
* 
  IN PVOID StackBase,
  
* 
  IN PVOID StackLimit
  
* 
  )
  
*
* Routine Description:
*
* This function switches to the specified large kernel stack.
*
* N.B. This function can ONLY be called when there are no variables
* 
     in the stack that refer to other variables in the stack, i.e.,
  
* 
     there are no pointers into the stack.
  
*
* Arguments:
*
* StackBase (esp + 4) - Supplies a pointer to the base of the new kernel
* 
     stack.
  
*
* StackLimit (esp + 8) - Supplies a pointer to the limit of the new
* 
     kernel stack.
  
*
* Return Value:
*
* The old kernel stack is returned as the function value.
*
* 

*/

FUNC KeSwitchKernelStack, 2

/*

* Save string move registers.
  */

  
   pushl   %esi
   pushl   %edi
  

/*

* Get current thread address.
  */

  
   movl    PCR[PcPrcbData + PbCurrentThread], %edx
  

/*

* Get new kernel stack base address.
*
* The original MASM expression:
*
* 
  [esp]+SsStkBs+8
  
*
* is:
*
* 
  12(%esp)
  
*
* after the two saved registers.
  */

  
   movl    SsStkBs+8(%esp), %edi

   movl    ThStackBase(%edx), %ecx
  

/*

* Relocate caller's frame pointer.
  */

  
   subl    %ecx, %ebp
   addl    %edi, %ebp
  

/*

* Relocate current trap frame.
  */

  
   movl    ThTrapFrame(%edx), %eax

   subl    %ecx, %eax
   addl    %edi, %eax

   movl    %eax, ThTrapFrame(%edx)
  

/*

* Compute length of copy.
  */

  
   subl    %esp, %ecx

   subl    %ecx, %edi

   movl    %esp, %esi

   pushl   %edi

   rep
   movsb

   popl    %edi
  

/*

* Switch to the new kernel stack and return the address of the old kernel
* stack.
  */

  
   movl    ThStackBase(%edx), %eax

   movl    SsStkBs+8(%esp), %ecx
   movl    SsStkLm+8(%esp), %esi

   cli

   movl    %ecx, ThStackBase(%edx)
   movl    %esi, ThStackLimit(%edx)

   movb    $1, ThLargeStack(%edx)

   movl    %ecx, ThInitialStack(%edx)

   movl    ThTrapFrame(%edx), %esi

   movl    PCR[PcTss], %edx
  

#if ((EFLAGS_V86_MASK & 0xff00ffff) != 0)
#error "EFLAGS_V86_MASK layout is incompatible with callout.S"
#endif


    testb   $(EFLAGS_V86_MASK / 0x01000000), TsEFlags+2(%esi)

    leal    -NPX_FRAME_LENGTH(%ecx), %ecx

    jne     1f

    subl    $KCB_V86_BIAS, %ecx


1:
movl    %ecx, TssEsp0(%edx)


    movl    %edi, %esp

    sti

    popl    %edi
    popl    %esi

    RET_FUNC KeSwitchKernelStack


ENDPROC KeSwitchKernelStack

/*

* 
*
* PULONG
* KiGetUserModeStackAddress(
* 
  VOID
  
* 
  )
  
*
* Routine Description:
*
* This function returns the address of the user stack address in the
* current trap frame.
*
* Arguments:
*
* None.
*
* Return Value:
*
* The address of the user stack address.
*
* 

*/

FUNC KiGetUserModeStackAddress, 0


    movl    PCR[PcPrcbData + PbCurrentThread], %eax

    movl    ThTrapFrame(%eax), %eax

    leal    TsHardwareEsp(%eax), %eax

    RET_FUNC KiGetUserModeStackAddress


ENDPROC KiGetUserModeStackAddress

/*

* 
*
* NTSTATUS
* NtCallbackReturn(
* 
  IN PVOID OutputBuffer OPTIONAL,
  
* 
  IN ULONG OutputLength,
  
* 
  IN NTSTATUS Status
  
* 
  )
  
*
* Routine Description:
*
* This function returns from a user mode callout to the kernel
* mode caller of the user mode callback function.
*
* N.B. This function returns to the function that called out to
* 
     user mode and the KiCallUserMode function calls out to user
  
* 
     mode. Therefore, the stack layout must be consistent between
  
* 
     the two routines.
  
*
* Arguments:
*
* OutputBuffer - Supplies an optional pointer to an output buffer.
*
* OutputLength - Supplies the length of the output buffer.
*
* Status - Supplies the status value returned to the caller of the
* 
     callback function.
  
*
* Return Value:
*
* If the callback return cannot be executed, then an error status is
* returned. Otherwise, the specified callback status is returned to
* the caller of the callback function.
*
* N.B. This function returns to the function that called out to user
* 
      mode if a callout is currently active.
  
*
* 

*/

FUNC NtCallbackReturn, 3


    movl    PCR[PcPrcbData + PbCurrentThread], %eax

    movl    ThCallbackStack(%eax), %ecx

    testl   %ecx, %ecx

    je      CbExit


/*

* Restore the current exception list from the saved exception list in
* the current trap frame, restore the trap frame and callback stack
* addresses, store the output buffer address and length, and set the
* service status.
  */

  
   movl    ThTrapFrame(%eax), %ebx

   movl    TsExceptionList(%ebx), %edx

   movl    %edx, PCR[PcExceptionList]

   movl    4(%esp), %edi
   movl    8(%esp), %esi
   movl    12(%esp), %ebp

   movl    CuOutBf(%ecx), %ebx

   movl    %edi, (%ebx)

   movl    CuOutLn(%ecx), %ebx

   movl    %esi, (%ebx)

   movl    (%ecx), %ebx
  

/*

* Disable interrupts while manipulating the NPX save area and stack.
  */

  
   cli

   movl    ThInitialStack(%eax), %esi

   movl    %ebx, ThInitialStack(%eax)

   subl    $NPX_FRAME_LENGTH, %esi
   subl    $NPX_FRAME_LENGTH, %ebx
  

/*

* Copy NPX state to previous frame.
  */

  
   movl    FpControlWord(%esi), %edx
   movl    %edx, FpControlWord(%ebx)

   movl    FpStatusWord(%esi), %edx
   movl    %edx, FpStatusWord(%ebx)

   movl    FpTagWord(%esi), %edx
   movl    %edx, FpTagWord(%ebx)

   movl    FxMXCsr(%esi), %edx
   movl    %edx, FxMXCsr(%ebx)

   movl    FpCr0NpxState(%esi), %edx
   movl    %edx, FpCr0NpxState(%ebx)


   cmpl    $STATUS_CALLBACK_POP_STACK, %ebp

   movl    CuTrFr(%ecx), %edi

   je      _NtCbCopyTrapFrame
  

_NtCbCopyTrapFrameRet:


    /*
     * Assume original trap frame has no debug registers active.
     */

    movl    $0, TsDr7(%edi)

    /*
     * Determine whether the thread has active debug registers.
     */

    testb   $0xff, ThDebugActive(%eax)

    jne     _NtCbGetDebugRegs


_NtCbGetDebugRet:


    /*
     * Get address of task switch segment.
     */

    movl    PCR[PcTss], %edx

    /*
     * Trim stack back to callback frame.
     */

    leal    CuCbStk(%ecx), %esp


#if ((EFLAGS_V86_MASK & 0xff00ffff) != 0)
#error "EFLAGS_V86_MASK layout is incompatible with callout.S"
#endif


    testb   $(EFLAGS_V86_MASK / 0x01000000), TsEFlags+2(%edi)

    jne     1f

    subl    $KCB_V86_BIAS, %ebx


1:
movl    %ebx, TssEsp0(%edx)


    movl    %edi, ThTrapFrame(%eax)

    sti


/*

* Restore callback stack address.
  */

  
   popl    ThCallbackStack(%eax)

   movl    %ebp, %eax
  

/*

* Restore nonvolatile registers, clean call parameters from stack,
* and return to callback caller.
  */

  
   popl    %edi
   popl    %esi
   popl    %ebx
   popl    %ebp

   popl    %edx

   addl    $8, %esp

   jmp     *%edx
  

/*

* 
*
* edi - previous trap frame [in/out]
* ecx - callback stack [in/out]
* eax - current thread [in/out]
* esi/edx: available
*
* 

*/

_NtCbCopyTrapFrame:


    movl    $((TsV86Es - TsSegFs) / 4), %ecx

    movl    ThTrapFrame(%eax), %esi


#if ((EFLAGS_V86_MASK & 0xff00ffff) != 0)
#error "EFLAGS_V86_MASK layout is incompatible with callout.S"
#endif


    testb   $(EFLAGS_V86_MASK / 0x01000000), TsEFlags+2(%esi)

    movl    %edi, %edx

    leal    TsSegFs(%edi), %edi

    jz      1f

    addl    $((KTRAP_FRAME_LENGTH - TsV86Es) / 4), %ecx


1:
leal    TsSegFs(%esi), %esi


    rep
    movsl

    movl    ThCallbackStack(%eax), %ecx

    movl    %edx, %edi

    jmp     _NtCbCopyTrapFrameRet


/*

* 
*
* Copy debug registers from current frame to original frame.
*
* 

*/

_NtCbGetDebugRegs:


    movl    ThTrapFrame(%eax), %esi

    movl    TsDr0(%esi), %edx
    movl    %edx, TsDr0(%edi)

    movl    TsDr1(%esi), %edx
    movl    %edx, TsDr1(%edi)

    movl    TsDr2(%esi), %edx
    movl    %edx, TsDr2(%edi)

    movl    TsDr3(%esi), %edx
    movl    %edx, TsDr3(%edi)

    movl    TsDr6(%esi), %edx
    movl    %edx, TsDr6(%edi)

    movl    TsDr7(%esi), %edx
    movl    %edx, TsDr7(%edi)

    jmp     _NtCbGetDebugRet


/*

* 
*
* No callback is currently active.
*
* 

*/

CbExit:


    movl    $STATUS_NO_CALLBACK_ACTIVE, %eax

    RET_FUNC NtCallbackReturn


ENDPROC NtCallbackReturn

/*

* End of code section.
  */
  .section .text

#if defined(**ELF**)
.size KiCallUserMode, .-KiCallUserMode
.size KeSwitchKernelStack, .-KeSwitchKernelStack
.size KiGetUserModeStackAddress, .-KiGetUserModeStackAddress
.size NtCallbackReturn, .-NtCallbackReturn
#endif
