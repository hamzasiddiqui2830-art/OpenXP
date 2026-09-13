;++
;
; Copyright (c) OpenXP
;
; Refactored for MASM 14.x (Visual Studio 2022/2026)
;
; Module Name:
;
;       MI.INC
;
; Abstract:
;
;       This module contains equates for x86 machine instructions.
;
;
; Notes:
;
;       This information used to reside in Trap.asm, but is now needed in
;       multiple source files.
;
; Revision History:
;--

; Maximum instruction and prefix lengths
MAX_INSTRUCTION_LENGTH          EQU 15
MAX_INSTRUCTION_PREFIX_LENGTH   EQU 4

; Instruction prefixes and opcodes
MI_LOCK_PREFIX                  EQU 0F0h
MI_ADDR_PREFIX                  EQU 067h
MI_TWO_BYTE                     EQU 0Fh
MI_HLT                          EQU 0F4h
MI_REP_INS_OUTS                 EQU 0F3h
MI_MIN_INS_OUTS                 EQU 06Ch
MI_MAX_INS_OUTS                 EQU 06Fh

; System instruction opcodes and masks
MI_LTR_LLDT                     EQU 0
MI_LGDT_LIDT_LMSW               EQU 01h
MI_LMSW_OPCODE                  EQU 001h  ; second byte of lmsw
MI_CLTS_OPCODE                  EQU 006h  ; second byte of clts
MI_GET_CRx_OPCODE               EQU 020h  ; mov r32, CRx
MI_SET_CRx_OPCODE               EQU 022h  ; mov CRx, r32
MI_GET_TRx_OPCODE               EQU 024h  ; mov r32, TRx
MI_SET_TRx_OPCODE               EQU 026h  ; mov TRx, r32

; ModRM byte masks and shifts
MI_MODRM_MASK                   EQU 38h
MI_REGMASK                      EQU 38h   ; REG field mask
MI_REGSHIFT                     EQU 3     ; REG field shift
MI_REGLMSW                      EQU 30h   ; REG field for lmsw

MI_MODMASK                      EQU 0C0h  ; MOD field mask
MI_MODSHIFT                     EQU 6     ; MOD field shift
MI_MODMOVSPEC                   EQU 0C0h  ; MOD field for mov to/from special
MI_MODNONE                      EQU 0

MI_RMMASK                       EQU 07h   ; RM field mask
MI_RMBP                         EQU 06h   ; RM value for bp reg
MI_RMSIB                        EQU 04h   ; RM value for sib

; Specific instruction masks (Group 6/7)
MI_LLDT_MASK                    EQU 10h
MI_LTR_MASK                     EQU 18h
MI_LGDT_MASK                    EQU 10h
MI_LIDT_MASK                    EQU 18h
MI_LMSW_MASK                    EQU 30h
MI_SPECIAL_MOV_MASK             EQU 20h

; SIB byte masks and shifts
MI_SIB_BASEMASK                 EQU 07h   ; SIB BASE field mask
MI_SIB_BASENONE                 EQU 05h
MI_SIB_BASESHIFT                EQU 0

MI_SIB_INDEXMASK                EQU 38h
MI_SIB_INDEXSHIFT               EQU 3
MI_SIB_INDEXNONE                EQU 20h

MI_SIB_SSMASK                   EQU 0C0h
MI_SIB_SSSHIFT                  EQU 6
