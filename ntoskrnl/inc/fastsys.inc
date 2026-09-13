;/*
;
; Copyright (c) OpenXP
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    fastsys.inc
;
; Abstract:
;
;    This module implements the header for Fast System Calls for Intel
;    x86 family processors.
;
;*/

#if 0

#define KI_FAST_SYSCALL_SUPPORTED   0x0800
#define MSR_SYSENTER_CS             0x00000174
#define MSR_SYSENTER_ESP            0x00000175
#define MSR_SYSENTER_EIP            0x00000176
#define MSR_EXT_FEATURE_ENABLE      0xC0000080
#define MSR_SYSCALL_TARGET_ADDR     0xC0000081
#define MSR_EFER_SCE                0x1

#endif

/*
;---------------------------------------------------------------------------
; Assembly definitions
;---------------------------------------------------------------------------

KI_FAST_SYSCALL_SUPPORTED EQU 0800h
MSR_SYSENTER_CS         EQU 00000174h
MSR_SYSENTER_ESP        EQU 00000175h
MSR_SYSENTER_EIP        EQU 00000176h
MSR_EXT_FEATURE_ENABLE  EQU 0C0000080h
MSR_SYSCALL_TARGET_ADDR EQU 0C0000081h
MSR_EFER_SCE            EQU 1

iSYSENTER macro
    SYSENTER
endm

iSYSEXIT macro
    SYSEXIT
endm

iSYSCALL macro
    SYSCALL
endm

iSYSRET macro
    SYSRET
endm
*/
