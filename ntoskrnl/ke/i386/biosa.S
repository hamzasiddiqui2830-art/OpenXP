/*

* PROJECT:         OpenXP
* LICENSE:         See project license
*
* MODULE:          ntoskrnl/ke/i386/spinlock.S
*
* DESCRIPTION:
* 
  This module implements the support routine used to execute
  
* 
  BIOS interrupt calls in V86 mode.
  
*
* 
  The routine constructs a temporary user-mode trap frame,
  
* 
  switches the current thread to the temporary V86 execution
  
* 
  environment, invokes VdmControl, and exits through the
  
* 
  kernel exception-exit path.
  
*
* 
  When the BIOS BOP returns to the kernel, the temporary trap
  
* 
  frame and NPX state are discarded and the original thread
  
* 
  environment is restored.
  
*
* ENVIRONMENT:
* 
  Kernel mode only.
  

*/

#include <asm.h>
#include <ks386.inc>
#include <callconv.inc>
#include <i386/kimacro.inc>

/*

* 
* Constants
* 

*/

#define VDM_START_EXECUTION     0
#define V86_STACK_POINTER       0x11ffe

/*

* 
* External symbols
* 

*/

.extern _NtVdmControl
.extern _KiExceptionExit

/*

* 
* Code section
* 

*/

.text
.align 4

/*

* ==========================================================================
* Ki386SetupAndExitToV86Code
* ==========================================================================
*
* VOID
* Ki386SetupAndExitToV86Code(
* 
  VOID
  
* 
  );
  
*
* DESCRIPTION:
*
* 
  Set up a temporary user-mode trap frame, switch the current
  
* 
  thread to the V86 execution environment, and enter V86 mode
  
* 
  through KiExceptionExit.
  
*
* 
  The caller supplies a structure through the calling convention
  
* 
  used by the original WRK routine.  The saved stack pointer and
  
* 
  original TEB values are retained on the temporary stack until
  
* 
  the BIOS operation completes.
  
*
* 
  It is important that the trap frame constructed here appears to
  
* 
  the kernel as a USER-mode trap frame.  Context management code
  
* 
  depends on the top trap frame for a user-mode thread being a
  
* 
  user-mode frame.
  
*
* ARGUMENTS:
*
* 
  BiosArguments
  
* 
      Supplies a pointer to the BIOS/V86 argument structure.
  
*
* RETURN VALUE:
*
* 
  None.
  

*/

.globl _Ki386SetupAndExitToV86Code
.type _Ki386SetupAndExitToV86Code,@function

_Ki386SetupAndExitToV86Code:

/*

* 
* Local stack layout.
*
* ECX points at the saved ESP area:
*
* 
  0(%ecx)   = original/saved stack
  
* 
  4(%ecx)   = original thread TEB
  
* 
  8(%ecx)   = original PCR TEB
  
*
* The fourth value at 32(%ecx) is the new/fake TEB.
* 

*/

#define NEW_TEB                 32(%ecx)
#define INITIAL_STACK           0(%ecx)
#define ORIGINAL_THREAD_TEB     4(%ecx)
#define ORIGINAL_PCR_TEB        8(%ecx)

/*

* Save the nonvolatile register state.
  */

  pushl   %ebp
  pushl   %ebx
  pushl   %esi
  pushl   %edi

  /*

  * Reserve local storage.
    */
    subl    $12, %esp

  movl    %esp, %ecx

/*

* 
* Allocate NPX state and the new trap frame.
* 

*/


subl    $NPX_FRAME_LENGTH, %esp

/*
 * FXSAVE requires 16-byte alignment.
 */
andl    $0xfffffff0, %esp

subl    $KTRAP_FRAME_LENGTH, %esp

/*
 * EAX = new base trap frame.
 */
movl    %esp, %eax


/*

* 
* Initialize the newly allocated trap frame.
* 
*
* The CS value intentionally identifies an invalid user-mode return
* path.  If an attempt is made to return through this fabricated
* context, the kernel traps the return and transfers control back to
* Ki386BiosCallReturnAddress.
* 

*/


movl    $(KGDT_R0_CODE | RPL_MASK), TsSegCs(%eax)

movl    $0, TsSegEs(%eax)
movl    $0, TsSegDs(%eax)
movl    $0, TsSegFs(%eax)
movl    $0, TsSegGs(%eax)

movl    $0, TsErrCode(%eax)


/*

* Obtain the current PCR.
  */

  movl    PCR[PcSelfPcr], %ebx

/*

* Obtain the current thread.
  */

  movl    PcPrcbData+PbCurrentThread(%ebx), %edi

/*

* Save the location of the NPX frame associated with the thread.
  */

  movl    ThInitialStack(%edi), %edx
  subl    $NPX_FRAME_LENGTH, %edx

  movl    %edx, INITIAL_STACK

/*

* Save the thread's original TEB.
  */

  movl    ThTeb(%edi), %edx
  movl    %edx, ORIGINAL_THREAD_TEB

/*

* Save the PCR's original TEB.
  */

  movl    PCR[PcTeb], %edx
  movl    %edx, ORIGINAL_PCR_TEB

/*

* 
* Construct the fabricated user-mode return frame.
* 

*/


movl    $Ki386BiosCallReturnAddress, %edi

/*
 * Save the stack pointer used to recover the original environment.
 */
movl    %ecx, TsEsi(%eax)

/*
 * BIOS completion returns through this address.
 */
movl    %edi, TsEip(%eax)


/*

* Sanitize EFLAGS.
*
* Preserve the WRK mask exactly:
*
* 
  flags &= 0x60dd7
  
* 
  flags |= IF
  

*/


pushfl
popl    %edi

andl    $0x60dd7, %edi
orl     $0x200, %edi

movl    $(KGDT_R3_DATA | RPL_MASK), TsHardwareSegSs(%eax)
movl    $V86_STACK_POINTER, TsHardwareEsp(%eax)

movl    %edi, TsEflags(%eax)

movl    $EXCEPTION_CHAIN_END, TsExceptionList(%eax)

movl    $0xffffffff, TsPreviousPreviousMode(%eax)

/*
 * Disable hardware debugging state in the fabricated frame.
 */
andl    $0, TsDr7


#ifdef DBG


movl    $0xBADB0D00, TsDbgArgMark(%eax)


#endif

/*

* Advance past the trap frame.
*
* EAX now points at the NPX save area.
  */

  addl    $KTRAP_FRAME_LENGTH, %eax

/*

* 
* Switch to the temporary trap-frame environment.
* 
*
* Interrupts must remain disabled while the thread stack and TSS
* pointers are changed.
* 

*/


cli


/*

* 
* Copy the current floating-point/NPX state into the temporary
* NPX frame.
* 

*/


pushl   %ecx

movl    PcPrcbData+PbCurrentThread(%ebx), %edi
movl    ThInitialStack(%edi), %esi
subl    $NPX_FRAME_LENGTH, %esi

movl    $(NPX_FRAME_LENGTH / 4), %ecx

movl    %eax, %edi
rep
movsl

popl    %ecx


/*

* 
* Install the temporary NPX frame and trap frame in the current
* processor/thread state.
* 

*/


movl    PcPrcbData+PbCurrentThread(%ebx), %edi
movl    PcTss(%ebx), %esi

/*
 * Bias EAX because the V86 segment fields are not represented in
 * the hardware portion of the trap frame.
 */
subl    $(TsV86Gs - TsHardwareSegSs), %eax

movl    $EXCEPTION_CHAIN_END, PcExceptionList(%ebx)

movl    %eax, TssEsp0(%esi)

addl    $(NPX_FRAME_LENGTH + (TsV86Gs - TsHardwareSegSs)), %eax

movl    %eax, ThInitialStack(%edi)


/*

* 
* Install the fake TEB used by the V86 BIOS execution.
* 

*/


movl    NEW_TEB, %eax

movl    %eax, PCR[PcTeb]
movl    %eax, ThTeb(%edi)


/*

* Update the R3 TEB descriptor in the GDT.
  */

  movl    PCR[PcGdt], %ebx

  movw    %ax, (KGDT_R3_TEB + KgdtBaseLow)(%ebx)

  shrl    $16, %eax

  movb    %al, (KGDT_R3_TEB + KgdtBaseMid)(%ebx)
  movb    %ah, (KGDT_R3_TEB + KgdtBaseHi)(%ebx)

/*

* The temporary environment is now installed.
  */

  sti

/*

* 
* Enter VDM execution.
* 
*
* NtVdmControl(
* 
  VdmStartExecution,
  
* 
  0
  
* );
*
* Use the kernel calling-convention helper supplied by callconv.inc.
* 

*/


stdCall _NtVdmControl, <VDM_START_EXECUTION, 0>


/*

* 
* Exit into the V86 execution context.
* 
*
* EBP must point to the exit trap frame expected by KiExceptionExit.
* 

*/


movl    %esp, %ebp

jmp     _KiExceptionExit


/*

* ==========================================================================
* Ki386BiosCallReturnAddress
* ==========================================================================
*
* The ROM BIOS executes a BOP after completing the BIOS INT call.
* The BOP traps into the kernel.  The BOP handler restores the
* fabricated 32-bit user-mode context.
*
* Because the fabricated context contains an invalid return CS/EIP,
* EXIT_ALL eventually faults and the GP handler recognizes the
* fabricated frame and transfers execution here.
*
* At entry:
*
* 
  - segment registers have been restored
  
* 
  - interrupts are disabled
  
* 
  - EBP points at the fabricated trap frame
  

*/

.globl Ki386BiosCallReturnAddress
.type Ki386BiosCallReturnAddress,@function

Ki386BiosCallReturnAddress:

/*

* 
* Restore the original processor/thread pointers.
* 

*/


movl    PCR[PcSelfPcr], %eax


/*

* Recover the original stack pointer.
  */

  movl    TsEsi(%ebp), %edi

  /*

  * The original initial-stack value was saved at the top of
  * the temporary stack.
    */
    movl    (%edi), %edi

/*

* 
* Copy the temporary NPX state back to the normal thread NPX area.
* 

*/


movl    PcPrcbData+PbCurrentThread(%eax), %ecx

movl    ThInitialStack(%ecx), %esi
subl    $NPX_FRAME_LENGTH, %esi

movl    $(NPX_FRAME_LENGTH / 4), %ecx

rep
movsl


/*

* Restore the original stack.
*
* The saved stack pointer itself occupies the first DWORD and is
* discarded after restoring ESP.
  */

  movl    TsEsi(%ebp), %esp
  addl    $4, %esp

/*

* 
* Restore Thread.InitialStack.
* 

*/


movl    PcPrcbData+PbCurrentThread(%eax), %ecx

movl    %edi, ThInitialStack(%ecx)


/*

* 
* Restore TSS.Esp0.
* 

*/


movl    PcTss(%eax), %eax

subl    $((TsV86Gs - TsHardwareSegSs) + NPX_FRAME_LENGTH), %edi

movl    %edi, TssEsp0(%eax)


/*

* 
* Restore the original TEB pointers.
* 

*/


popl    %edx
movl    %edx, ThTeb(%ecx)

popl    %edx
movl    %edx, PCR[PcTeb]


/*

* Restore the R3 TEB descriptor in the GDT.
  */

  movl    PCR[PcGdt], %ebx

  movw    %dx, (KGDT_R3_TEB + KgdtBaseLow)(%ebx)

  shrl    $16, %edx

  movb    %dl, (KGDT_R3_TEB + KgdtBaseMid)(%ebx)
  movb    %dh, (KGDT_R3_TEB + KgdtBaseHi)(%ebx)

/*

* Re-enable interrupts now that the original environment is restored.
  */

  sti

/*

* Restore nonvolatile registers.
  */

  popl    %edi
  popl    %esi
  popl    %ebx
  popl    %ebp

  ret

.size _Ki386SetupAndExitToV86Code, .-_Ki386SetupAndExitToV86Code

.section .note.GNU-stack,"",@progbits
.end
