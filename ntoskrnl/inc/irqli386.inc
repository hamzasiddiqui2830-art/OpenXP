;++
;
; Copyright (c) Microsoft Corporation. All rights reserved.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    irqli386.inc - 386 machine specific IRQL manipulation routines
;
; Abstract:
;
;   This module provides macros to raise, lower and get current IRQL.
;   These routines are abstracted through macros in order to facilitate
;   generation of a kernel that manipulates IRQL directly.
;
;   If _APIC_TPR_ is defined, IRQL is manipulated directly via the PIC.
;   Otherwise, the appropriate HAL routine is called.
;
; This module exposes four public macros:
;
; CurrentIrql
; RaiseIrql
; LowerIrql
; LowerIrqlRet
;

IFDEF _APIC_TPR_

    APIC_TPR equ dword ptr ds:0FFFE0080h
    EXTRN _HalpIRQLToTPR:DWORD
    EXTRN _HalpVectorToIRQL:DWORD

;++
;   CurrentIrql
;
;   Macro Description:
;       This macro retrieves the current IRQL in eax
;
;   Arguments:
;       None
;
;   Return Value:
;       The current IRQL is returned in EAX.
;--
CurrentIrql macro
    mov eax, APIC_TPR
    shr eax, 4
    add eax, _HalpVectorToIRQL
    movzx eax, BYTE PTR [eax]
endm

;++
;   SetIrql Irql
;
;   Macro Description:
;       This macro sets IRQL to the supplied value. This macro is an
;       internal routine only.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL. It is supplied in
;           one of three forms:
;           cl    - the value is zero-extended into ecx before use
;           ecx   - cl has already been zero-extended
;           const - Constant expression is used directly
;
;   Return Value:
;       None.
;--
SetIrql macro Irql
    IFIDN <Irql>, <ecx>
        add ecx, _HalpIRQLToTPR
        movzx ecx, BYTE PTR [ecx]
    ELSE
        IFIDN <Irql>, <cl>
            movzx ecx, cl
            add ecx, _HalpIRQLToTPR
            movzx ecx, BYTE PTR [ecx]
        ELSE
            IFNB <Irql>
                mov ecx, _HalpIRQLToTPR
                movzx ecx, BYTE PTR [ecx+Irql]
            ELSE
                .err <Irql parm not supplied to SetIrql>
            ENDIF
        ENDIF
    ENDIF
    mov APIC_TPR, ecx
endm

;++
;   LowerIrql Irql
;
;   Macro Description:
;       This macro lowers IRQL to the supplied value.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;
;   Return Value:
;       None.
;--
LowerIrql macro Irql
    SetIrql Irql
    mov ecx, APIC_TPR
endm

;++
;   LowerIrqlRet Irql
;
;   Macro Description:
;       This macro lowers IRQL to the supplied value and returns to the calling
;       routine.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;
;   Return Value:
;       None.
;
;   Comments:
;       This macro should be used only in cases where the function epilogue is
;       known to be identical to that of KfLowerIrql().
;--
LowerIrqlRet macro Irql
    LowerIrql Irql
    ret
endm

;++
;   RaiseIrql Irql [,NoOld]
;
;   Macro Description:
;       This macro raises IRQL to the supplied value.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;       NoOld - Optional. If supplied, no value is returned in eax.
;
;   Return Value:
;       Unless the NoOld flag is supplied, eax contains the zero-extended
;       value of the IRQL before raising.
;--
RaiseIrql macro Irql, nooldirql
    IFIDN <nooldirql>, <NoOld>
        ; Do nothing, no old IRQL returned
    ELSE
        IFNB <nooldirql>
            .err <RaiseIrql used with optional parm other than NoOld>
        ELSE
            CurrentIrql
        ENDIF
    ENDIF
    SetIrql Irql
endm

ELSE ; IFNDEF _APIC_TPR_

    EXTRN __imp_@KfRaiseIrql@4:DWORD
    EXTRN __imp_@KfLowerIrql@4:DWORD
    EXTRN __imp__KeGetCurrentIrql@0:DWORD

;++
;   CurrentIrql
;
;   Macro Description:
;       This macro retrieves the current IRQL in eax
;
;   Arguments:
;       None
;
;   Return Value:
;       The current IRQL is returned in EAX.
;--
CurrentIrql macro
    call __imp__KeGetCurrentIrql@0
endm

;++
;   LowerIrql Irql
;
;   Macro Description:
;       This macro lowers IRQL to the supplied value.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;
;   Return Value:
;       None.
;--
LowerIrql macro Irql
    IFIDN <Irql>, <cl>
    ELSE
        IFIDN <Irql>, <ecx>
        ELSE
            mov ecx, Irql
        ENDIF
    ENDIF
    call __imp_@KfLowerIrql@4
endm

;++
;   LowerIrqlRet Irql
;
;   Macro Description:
;       This macro lowers IRQL to the supplied value and returns to the calling
;       routine.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;
;   Return Value:
;       None.
;
;   Comments:
;       This macro should be used only in cases where the function epilogue is
;       known to be identical to that of KfLowerIrql().
;--
LowerIrqlRet macro Irql
    IFIDN <Irql>, <cl>
    ELSE
        IFIDN <Irql>, <ecx>
        ELSE
            mov ecx, Irql
        ENDIF
    ENDIF
    jmp __imp_@KfLowerIrql@4
endm

;++
;   RaiseIrql Irql [,NoOld]
;
;   Macro Description:
;       This macro raises IRQL to the supplied value.
;
;   Arguments:
;       Irql - provides the value for the desired new IRQL.
;
;   Return Value:
;       Unless the NoOld flag is supplied, eax contains the zero-extended
;       value of the IRQL before raising.
;--
RaiseIrql macro Irql
    IFIDN <Irql>, <cl>
    ELSE
        IFIDN <Irql>, <ecx>
        ELSE
            mov ecx, Irql
        ENDIF
    ENDIF
    call __imp_@KfRaiseIrql@4
endm

ENDIF ; IFDEF _APIC_TPR_
