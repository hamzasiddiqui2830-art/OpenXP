/*
 * page    78,132
 *
 * Copyright (c) OpenXP.
 *
 * Module Name:
 *
 *   emxcptn.S
 *
 * Abstract:
 *       Module to support getting/setting context to and from the R3
 *       emulator.
 *
 */

        /*
         * .386p is implicit for the i386 GAS target.
         *
         * ASSUME DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING
         * is likewise implicit/not required by GAS.
         */

#define NT386 1

#include <asm.h>
#include <ks386.inc>
#include <em387.inc>              /* Emulator TEB data layout */
#include <callconv.inc>
#include <irqli386.inc>


/*
 * External procedures/data.
 *
 * These names assume the same i386 symbol-decoration convention used
 * by the existing OpenXP C/assembly build.
 */
        .extern _KeBugCheck@4
        .extern _RtlRaiseStatus@4
        .extern _Ki387RoundModeTable


        .section .text$PAGE,"xr"


/*
 * =========================================================================
 *
 * _KiEm87StateToNpxFrame
 *
 * Translates the R3 emulator state to the NpxFrame.
 *
 * Returns TRUE if NpxFrame successfully completed.
 *         FALSE otherwise.
 *
 * Warning:
 * This function can only be called at IRQL 0 with interrupts enabled.
 * It is intended to be called only to deal with R3 exceptions when the
 * emulator is being used.
 *
 * =========================================================================
 */

        .globl _KiEm87StateToNpxFrame
        .type  _KiEm87StateToNpxFrame,@function
_KiEm87StateToNpxFrame:

        pushl   %ebp
        movl    %esp, %ebp

        pushl   %ebx                    /* Save C runtime variables */
        pushl   %edi
        pushl   %esi

        /*
         * Link our exception handler.
         *
         * MASM:
         *
         *   push esp
         *   push offset stnpx_30
         *   push PCR[PcExceptionList]
         *   mov  PCR[PcExceptionList],esp
         *
         * Keep PCR access through the OpenXP/ReactOS-compatible macro.
         */

        pushl   %esp                    /* Pass current Esp to handler */
        pushl   $stnpx_30               /* Set handler address */
        pushl   PCR[PcExceptionList]    /* Set next pointer */
        movl    %esp, PCR[PcExceptionList]

#if DBG

        pushfl
        popl    %ecx                    /* Make sure interrupts are enabled */
        testl   $EFLAGS_INTERRUPT_MASK, %ecx
        jz      .Lstnpx_err

        CurrentIrql                    /* Sanity check */
        cmpb    $DISPATCH_LEVEL, %al
        jnc     .Lstnpx_err             /* IRQL must be below DPC level */

#endif

        xorl    %eax, %eax              /* Set FALSE */

        movl    PCR[PcPrcbData+PbCurrentThread], %ebx
        movl    ThApcState+AsProcess(%ebx), %ebx

        cmpl    $0, PrVdmObjects(%ebx)   /* Is this a VDM process? */
        jne     .Lstnpx_10              /* Yes, then not supported */

        movl    PCR[PcTeb], %ebx         /* R3 TEB */

        cmpl    $0, Einstall(%ebx)       /* Initialized? */
        je      .Lstnpx_10              /* No */

        testl   $Summary, CURErr(%ebx)   /* Completed? */
        jz      .Lstnpx_10              /* No */

        movl    8(%ebp), %esi            /* ESI = NpxFrame */

        call    SaveState

        movl    $1, %eax                 /* Return TRUE */

.Lstnpx_10:

        popl    PCR[PcExceptionList]     /* Remove exception handler */
        addl    $8, %esp                 /* Clear handler stack */

        popl    %esi
        popl    %edi
        popl    %ebx
        popl    %ebp

        ret     $4                       /* stdRET _KiEm87StateToNpxFrame */

#if DBG

.Lstnpx_err:

        pushl   $IRQL_NOT_LESS_OR_EQUAL
        call    _KeBugCheck@4

#endif

        .size _KiEm87StateToNpxFrame,.-_KiEm87StateToNpxFrame


/*
 * =========================================================================
 *
 * stnpx_30
 *
 * Exception handler.
 *
 * WARNING:
 * Here we directly unlink the exception handler from the exception
 * registration chain. NO unwind is performed. We can take this short cut
 * because we know that our handler is a leaf-node.
 *
 * =========================================================================
 */

        .type stnpx_30,@function
stnpx_30:

        movl    8(%esp), %esp             /* ESP -> ExceptionList */
        xorl    %eax, %eax                /* Return FALSE */
        jmp     .Lstnpx_10


/*
 * =========================================================================
 *
 * SaveEnv
 *
 * Arguments:
 *       ESI = NpxFrame
 *       EBX = PcTeb
 *
 * =========================================================================
 */

        .type SaveEnv,@function
SaveEnv:

        xorl    %eax, %eax

        movw    %ax, reserved1(%esi)
        movw    %ax, reserved2(%esi)
        movw    %ax, reserved3(%esi)
        movw    %ax, reserved4(%esi)
        movw    %ax, reserved5(%esi)

        movw    ControlWord(%ebx), %ax
        movw    %ax, E32_ControlWord(%esi)

        call    GetEMSEGStatusWord
        movw    %ax, E32_StatusWord(%esi)

        call    GetTagWord
        movw    %ax, E32_TagWord(%esi)

        movw    %cs, %ax
        movw    %ax, E32_CodeSeg(%esi)     /* NOTE: Not R0 code & stack */

        movw    %ss, %ax
        movw    %ax, E32_DataSeg(%esi)

        movl    PrevCodeOff(%ebx), %eax
        movl    %eax, E32_CodeOff(%esi)

        movl    PrevDataOff(%ebx), %eax
        movl    %eax, E32_DataOff(%esi)

        ret


/*
 * =========================================================================
 *
 * SaveState
 *
 * Arguments:
 *       ESI = where to store environment
 *       EBX = PcTeb
 *
 * Registers:
 *       Destroys ALL, but EBX
 *
 * =========================================================================
 */

        .type SaveState,@function
SaveState:

        movl    $CR0_EM, FpCr0NpxState(%esi)

        call    SaveEnv

        addl    $Env80x87_32, %esi         /* Skip over environment */

        movl    $NumLev, %ebp              /* Save entire stack */

        movl    CURstk(%ebx), %edi

.Lss_loop:

        movl    ExpSgn(%ebx,%edi,1), %eax

        call    StoreTempReal              /* In emstore.asm */

        addl    $10, %esi

        movl    CURstk(%ebx), %edi

        cmpl    $INITstk, %edi

        jae     .Lss_wrap

        addl    $Reg87Len, %edi

.Lss_continue:

        movl    %edi, CURstk(%ebx)

        decl    %ebp
        jnz     .Lss_loop

        ret

.Lss_wrap:

        movl    $BEGstk, %edi
        jmp     .Lss_continue


/*
 * =========================================================================
 *
 * GetTagWord
 *
 * Figures out the tag word from the numeric stack.
 *
 * Arguments:
 *       EBX = PcTeb
 *
 * Returns:
 *       AX = tag word
 *
 * =========================================================================
 */

        .type GetTagWord,@function
GetTagWord:

        pushl   %esi

        xorl    %eax, %eax

        movl    $NumLev, %ecx
        movl    $INITstk, %esi

.LGetTagLoop:

        movb    bTag(%ebx,%esi,1), %dh

        /*
         * SHLD AX,DX,2
         *
         * GAS operates on the 32-bit destination register, so preserve
         * the original 16-bit AX operation using DX:EAX.
         */
        shldl   $2, %edx, %eax

        subl    $Reg87Len, %esi

        loop    .LGetTagLoop

        rolw    $2, %ax

        popl    %esi
        ret


/*
 * =========================================================================
 *
 * GetEMSEGStatusWord
 *
 * User status word returned in AX.
 * Uses status word in per-thread data area.
 *
 * Arguments:
 *       EBX = PcTeb
 *
 * =========================================================================
 */

        .type GetEMSEGStatusWord,@function
GetEMSEGStatusWord:

        movl    CURstk(%ebx), %eax
        subl    $BEGstk, %eax

        /*
         * Make sure the DIV won't overflow.
         */
        cmpl    $(Reg87Len * (NumLev + 2)), %eax
        ja      .LGetEMStatusOverflow

        movb    $Reg87Len, %dl

        divb    %dl

        incl    %eax
        andl    $7, %eax

        shlw    $11, %ax

        orw     StatusWord(%ebx), %ax

        ret

.LGetEMStatusOverflow:

        movl    $STATUS_INTEGER_OVERFLOW, %eax

        pushl   %eax
        call    _RtlRaiseStatus@4

        ret                             /* Should never come here */


/*
 * =========================================================================
 *
 * StoreTempReal
 *
 * Arguments:
 *       EBX = PcTeb
 *       EDI = emulator stack offset
 *       ESI = destination
 *
 * EAX contains exponent/sign/tag.
 *
 * =========================================================================
 */

        .type StoreTempReal,@function
StoreTempReal:

        movl    lManHi(%ebx,%edi,1), %edx
        movl    lManLo(%ebx,%edi,1), %edi

        /*
         * Mantissa in EDX:EDI
         * Exponent in high EAX
         * Sign in AH bit 7
         * Tag in AL
         *
         * Memory destination is ESI.
         */

        movl    %eax, %ecx
        shrl    $16, %ecx

        cmpb    $bTAG_ZERO, %al
        je      .LStoreIEEE80

        addl    $(IexpBias-TexpBias), %ecx

        cmpb    $bTAG_DEN, %al
        je      .LDenorm80

.LStoreIEEE80:

        andl    $(bSign << 8), %eax
        orl     %eax, %ecx

        movl    %edi, 0(%esi)
        movl    %edx, 4(%esi)
        movw    %cx, 8(%esi)

        ret


.LDenorm80:

        decl    %ecx
        negl    %ecx

        cmpb    $32, %cl
        jae     .LLongDenorm

        shrdl   %cl, %edx, %edi
        shrl    %cl, %edx

        xorl    %ecx, %ecx

        jmp     .LStoreIEEE80


.LLongDenorm:

        xchgl   %edx, %edi

        shrl    %cl, %edi

        xorl    %ecx, %ecx

        jmp     .LStoreIEEE80


/*
 * =========================================================================
 *
 * _KiNpxFrameToEm87State
 *
 * Translates the NpxFrame to the R3 emulator state.
 *
 * Returns TRUE if NpxFrame state successfully transferred.
 *         FALSE otherwise.
 *
 * =========================================================================
 */

        .globl _KiNpxFrameToEm87State
        .type  _KiNpxFrameToEm87State,@function
_KiNpxFrameToEm87State:

        pushl   %ebp
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %edi
        pushl   %esi

        pushl   %esp
        pushl   $npxts_30
        pushl   PCR[PcExceptionList]
        movl    %esp, PCR[PcExceptionList]

#if DBG

        pushfl
        popl    %ecx

        testl   $EFLAGS_INTERRUPT_MASK, %ecx
        jz      .Lnpxts_err

        CurrentIrql
        cmpb    $DISPATCH_LEVEL, %al
        jnc     .Lnpxts_err

#endif

        xorl    %eax, %eax

        movl    PCR[PcPrcbData+PbCurrentThread], %ebx
        movl    ThApcState+AsProcess(%ebx), %ebx

        cmpl    $0, PrVdmObjects(%ebx)
        jne     .Lnpxts_10

        movl    PCR[PcTeb], %ebx

        cmpl    $0, Einstall(%ebx)
        je      .Lnpxts_10

        movl    8(%ebp), %esi

        call    StorState

        orl     $Summary, CURErr(%ebx)

        movl    $1, %eax

.Lnpxts_10:

        popl    PCR[PcExceptionList]
        addl    $8, %esp

        popl    %esi
        popl    %edi
        popl    %ebx
        popl    %ebp

        ret     $4

#if DBG

.Lnpxts_err:

        pushl   $IRQL_NOT_LESS_OR_EQUAL
        call    _KeBugCheck@4

#endif

        .size _KiNpxFrameToEm87State,.-_KiNpxFrameToEm87State


/*
 * =========================================================================
 *
 * npxts_30
 *
 * Direct exception-chain unlink handler.
 *
 * =========================================================================
 */

        .type npxts_30,@function
npxts_30:

        movl    8(%esp), %esp
        xorl    %eax, %eax
        jmp     .Lnpxts_10


/*
 * =========================================================================
 *
 * StorState
 *
 * Emulates FRSTOR [address].
 *
 * Arguments:
 *       ESI = where to get environment
 *       EBX = PcTeb
 *
 * =========================================================================
 */

        .type StorState,@function
StorState:

        /*
         * First set up the status word so CURstk is initialized.
         */
        movw    E32_StatusWord(%esi), %ax
        call    SetEmStatusWord

        addl    $Env80x87_32, %esi

        /*
         * Mask invalid-operation exception so unsupported formats
         * are converted to Indefinite.
         */
        movw    $0x003f, CWmask(%ebx)

        movl    CURstk(%ebx), %edi
        movl    $NumLev, %ebp

.LFrstorLoadLoop:

        pushl   %esi

        call    LoadTempReal

        popl    %esi

        addl    $10, %esi

        cmpl    $INITstk, %edi
        jae     .Lfr_wrap

.Lfr_continue:

        decl    %ebp
        jnz     .LFrstorLoadLoop

        subl    $(NumLev * 10 + Env80x87_32), %esi

        /*
         * Store environment.
         */

        movw    E32_StatusWord(%esi), %ax
        call    SetEmStatusWord

        movw    E32_ControlWord(%esi), %ax
        call    SetControlWord

        movw    E32_TagWord(%esi), %ax
        call    UseTagWord

        movl    E32_CodeOff(%esi), %eax
        movl    %eax, PrevCodeOff(%ebx)

        movl    E32_DataOff(%esi), %eax
        movl    %eax, PrevDataOff(%ebx)

        ret

.Lfr_wrap:

        movl    $BEGstk, %edi
        jmp     .Lfr_continue


/*
 * =========================================================================
 *
 * SetEmStatusWord
 *
 * Given user status word in AX, set into emulator.
 *
 * Destroys EBX.
 *
 * =========================================================================
 */

        .type SetEmStatusWord,@function
SetEmStatusWord:

        andw    $0x7f7f, %ax

        movw    %ax, %cx
        andw    $0x003f, %cx

        movb    %cl, CURerr(%ebx)

        movl    %eax, %ecx
        andl    $~(7 << 11), %ecx

        movw    %cx, StatusWord(%ebx)

        /*
         * Adjust for emulator stack layout.
         */
        subb    $8, %ah

        andb    $(7 << 3), %ah

        movb    %ah, %al

        shrb    $1, %ah

        addb    %ah, %al

        /*
         * Reg87Len must be 12.
         *
         * MASM:
         *     .erre Reg87Len eq 12
         */
        .if Reg87Len != 12
        .error "Reg87Len must equal 12"
        .endif

        andl    $255, %eax

        addl    $BEGstk, %eax

        movl    %eax, CURstk(%ebx)

        ret


/*
 * =========================================================================
 *
 * SetControlWord
 *
 * =========================================================================
 */

        .type SetControlWord,@function
SetControlWord:

        andw    $0x0f3f, %ax

        movw    %ax, ControlWord(%ebx)

        notb    %al

        andb    $0x3f, %al

        movb    %al, ErrMask(%ebx)

        andl    $((RoundControl + PrecisionControl) << 8), %eax

        movl    _Ki387RoundModeTable, %ecx
        movl    (%ecx,%eax,1), %ecx

        movl    %ecx, RoundMode(%ebx)
        movl    %ecx, SavedRoundMode(%ebx)

        andl    $(RoundControl << (8-6)), %eax

        movl    _Ki387RoundModeTable, %ecx

        /*
         * Original:
         *
         * mov ecx,[ecx+(eax+PC64 shl (8-6))]
         *
         */
        addl    $(PC64 << (8-6)), %eax
        movl    (%ecx,%eax,1), %ecx

        movl    %ecx, TransRound(%ebx)

        ret


/*
 * =========================================================================
 *
 * UseTagWord
 *
 * Arguments:
 *       AX = tag word
 *
 * Destroys:
 *       AX, BX, CX, DX, DI
 *
 * =========================================================================
 */

        .type UseTagWord,@function
UseTagWord:

        rorw    $2, %ax

        movl    $INITstk, %edi
        movl    $NumLev, %ecx

.LUseTagLoop:

        movb    $bTAG_EMPTY, %dl

        cmpb    $0xc0, %ah
        jae     .LSetTag

        movb    bTag(%ebx,%edi,1), %dl

        cmpb    $bTAG_EMPTY, %dl
        je      .LSetTagNotEmpty

.LSetTag:

        movb    %dl, bTag(%ebx,%edi,1)

.LUseTagLoopCheck:

        subl    $Reg87Len, %edi

        shll    $2, %eax

        loop    .LUseTagLoop

        ret


/*
 * =========================================================================
 *
 * SetTagEmpty
 *
 * =========================================================================
 */

        .type SetTagEmpty,@function
SetTagEmpty:

        movb    $bTAG_EMPTY, bTag(%ebx,%edi,1)

        jmp     .LUseTagLoopCheck


/*
 * =========================================================================
 *
 * SetTagNotEmpty
 *
 * Register is currently tagged empty, but new tag word says it is
 * not empty. Determine the appropriate tag.
 *
 * =========================================================================
 */

        .type SetTagNotEmpty,@function
SetTagNotEmpty:

        cmpl    $0, lManHi(%ebx,%edi,1)

        movb    $bTAG_ZERO, %dl

        je      .LSetTag

        movl    ExpSgn(%ebx,%edi,1), %edx

        movb    $bTAG_DEN, %dl

        cmpl    $(TexpMin << 16), %edx
        jl      .LSetTag

        cmpl    $0, lManLo(%ebx,%edi,1)

        /*
         * bTAG_VALID == 1
         * bTAG_SNGL  == 0
         */
        setnz   %dl

        cmpl    $(TexpMax << 16), %edx

        jl      .LSetTag

        /*
         * bTAG_VALID - bTAG_SNGL == bTAG_NAN - bTAG_INF
         */
        shlb    $TAG_SHIFT, %dl

        addb    $(bTAG_INF - bTAG_SNGL), %dl

        /*
         * If low bits were zero:
         *     bTAG_SNGL -> bTAG_INF
         *
         * Otherwise:
         *     bTAG_VALID -> bTAG_NAN
         *
         * See if infinity is really possible:
         * high half mantissa must be 80000000h.
         */

        cmpl    $(1 << 31), lManHi(%ebx,%edi,1)

        je      .LSetTag

        movb    $bTAG_NAN, %dl

        jmp     .LSetTag


/*
 * =========================================================================
 *
 * LoadTempReal
 *
 * =========================================================================
 */

        .type LoadTempReal,@function
LoadTempReal:

        movl    4(%esi), %ebx             /* Get high half mantissa */
        movw    8(%esi), %cx              /* Get exponent and sign */
        movl    0(%esi), %esi             /* Get low half mantissa */

        movl    %ecx, %eax

        andb    $0x7f, %ch

        shll    $16, %ecx

        movb    %ah, %ch                  /* Restore sign */

        jz      .LZeroOrDenorm80

        /*
         * Check for unsupported format:
         * unnormal (MSB not set).
         */
        testl   %ebx, %ebx
        jns     .LUnsupported

        subl    $((IexpBias-TexpBias) << 16), %ecx

        cmpl    $(TexpMax << 16), %ecx
        jge     .LNANorInf80


.LSetupTag:

        testl   %esi, %esi

        /*
         * bTAG_VALID == 1
         * bTAG_SNGL  == 0
         */
        setnz   %cl

        jmp     .LSaveStack


.LNANorInf80:

        movb    $bTAG_NAN, %cl

        cmpl    $(1 << 31), %ebx
        jne     .LSaveStack

        testl   %esi, %esi
        jnz     .LSaveStack

        movb    $bTAG_INF, %cl

        jmp     .LSaveStack


/*
 * =========================================================================
 *
 * ZeroOrDenorm80
 *
 * =========================================================================
 */

.LZeroOrDenorm80:

        /*
         * Exponent is zero.
         * Number is either zero or denormalized.
         */

        testl   %ebx, %ebx
        jnz     .LShortNorm80

        testl   %esi, %esi
        jnz     .LLongNorm80

        movb    $bTAG_ZERO, %cl

        jmp     .LSaveStack


/*
 * Pseudo-denormals are accepted.
 */

.LLongNorm80:

        xchgl   %ebx, %esi

        subl    $(32 << 16), %ecx


.LShortNorm80:

        addl    $((TexpBias-IexpBias+1-31) << 16), %ecx

        bsrl    %ebx, %edx

        /*
         * Bit number in EDX ranges from 0 to 31.
         */
        movb    %dl, %cl

        notb    %cl

        shldl   %cl, %esi, %ebx

        shll    %cl, %esi

        shll    $16, %edx

        addl    %edx, %ecx

        jmp     .LSetupTag


/*
 * =========================================================================
 *
 * SaveStack
 *
 * =========================================================================
 */

.LSaveStack:

        movl    PCR[PcTeb], %eax

        movl    %edi, CURstk(%eax)

        movl    %esi, lManLo(%eax,%edi,1)
        movl    %ebx, lManHi(%eax,%edi,1)
        movl    %ecx, ExpSgn(%eax,%edi,1)

        movl    %eax, %ebx                 /* EBX = PcTeb */

        ret


/*
 * =========================================================================
 *
 * Unsupported
 *
 * Convert unsupported format to Indefinite/NaN.
 *
 * =========================================================================
 */

.LUnsupported:

        movl    PCR[PcTeb], %ebx

        orl     $Invalid, CURErr(%ebx)

        movl    $0, lManLo(%ebx,%edi,1)

        movl    $0xC0000000, lManHi(%ebx,%edi,1)

        movl    $(TexpMax << 16) + (bSign << 8) + bTAG_NAN, \
                ExpSgn(%ebx,%edi,1)

        movl    %edi, CURstk(%ebx)

        ret


        .size SaveEnv,.-SaveEnv
        .size SaveState,.-SaveState
        .size GetTagWord,.-GetTagWord
        .size GetEMSEGStatusWord,.-GetEMSEGStatusWord
        .size StoreTempReal,.-StoreTempReal
        .size StorState,.-StorState
        .size SetEmStatusWord,.-SetEmStatusWord
        .size SetControlWord,.-SetControlWord
        .size UseTagWord,.-UseTagWord
        .size SetTagEmpty,.-SetTagEmpty
        .size SetTagNotEmpty,.-SetTagNotEmpty
        .size LoadTempReal,.-LoadTempReal


        /*
         * End of emxcptn.S
         */
