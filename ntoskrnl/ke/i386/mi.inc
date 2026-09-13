;++
;
; Copyright (c) OpenXP
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
; Revision History:
;       
;       This information used to reside in Trap.asm, but is now needed in
;       multiple source files.
;
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

; ModRM byte masks and system instruction groups
MI_MODRM_MASK                   EQU 38h
MI_LTR_LLDT                     EQU 0
MI_LGDT_LIDT_LMSW               EQU 01h
MI_LLDT_MASK                    EQU 10h
MI_LTR_MASK                     EQU 18h
MI_LGDT_MASK                    EQU 10h
MI_LIDT_MASK                    EQU 18h
MI_LMSW_MASK                    EQU 30h
MI_SPECIAL_MOV_MASK             EQU 20h

; Specific system instruction opcodes
MI_CLTS                         EQU 006h
MI_INVD                         EQU 008h
MI_WBINVD                       EQU 009h

; Move to/from control, debug, and test registers
MI_MOV_FROM_CR                  EQU 020h
MI_MOV_FROM_DR                  EQU 021h
MI_MOV_TO_CR                    EQU 022h
MI_MOV_TO_DR                    EQU 023h
MI_MOV_FROM_TR                  EQU 024h
MI_MOV_TO_TR                    EQU 026h

; Model-specific register, timestamp, and fast syscall instructions
MI_WRMSR                        EQU 030h
MI_RDTSC                        EQU 031h
MI_RDMSR                        EQU 032h
MI_RDPMC                        EQU 033h
MI_SYSEXIT                      EQU 035h
