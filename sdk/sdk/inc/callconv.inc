;****************************CallConv.Inc************************************
; Simplified for MASM 14.x - No dynamic fastcall decoration

ComposeInst macro Inst, p1, p2, p3, p4, p5, p6, p7, p8, p9
    &Inst p1&p2&p3&p4&p5&p6&p7&p8&p9
endm

CountArg macro cCount, ArgList
    cCount = 0
    irp arg, <ArgList>
        cCount = cCount + 1
    endm
endm

RevPush macro ArgList, cCount
    LOCAL index, x
    CountArg cCount, <ArgList>
    index = cCount
    rept cCount
        x = 0
        irp arg, <ArgList>
            x = x + 1
            ife index - x
                push arg
                exitm
            endif
        endm
        index = index - 1
    endm
endm

option nokeyword:<stdcall>

PUBLICP macro Func, N
    IFB <N>
        PUBLIC Func&@0
    ELSE
        PUBLICP2 Func, %(N*4)
    ENDIF
endm

PUBLICP2 macro Func, N
    PUBLIC Func&@&N
endm

; Simplified EXTRNP - no fastcall support via macros
EXTRNP macro Func, N, Thunk, FastCall
    IFB <N>
        IFNDEF Func&@0
            EXTRN Func&@0:NEAR
        ENDIF
    ELSE
        IFB <Thunk>
            EXTRNP2 Func, %(N*4)
        ELSE
            EXTRNTHUNK Func, %(N*4)
        ENDIF
    ENDIF
endm

EXTRNP2 macro Func, N
    IFNDEF Func&@&N
        EXTRN Func&@&N:NEAR
    ENDIF
endm

EXTRNTHUNK macro Func, N
    IFNDEF __imp_&Func&@&N
        EXTRN __imp_&Func&@&N:DWORD
    ENDIF
endm

LABELP macro Func, N
    IFB <N>
        Func&@0 label near
    ELSE
        LABELP2 Func, %(N*4)
    ENDIF
endm

LABELP2 macro Func, N
    Func&@&N label near
endm

ProcName macro Name, Func, N
    IFB <N>
        cByte&Func equ 0
        Name equ <Func&@0>
    ELSE
        cByte&Func equ N
        Name equ <Func&@&N>
    ENDIF
endm

stdPROC macro Func, N, ArgList
    ProcName Func, Func, %(N*4)
    Func proc ArgList
endm

cPublicProc macro Func, N, ArgList
    align dword
    PUBLICP Func, N
    IFB <N>
        stdPROC Func, 0, <ArgList>
    ELSE
        stdPROC Func, N, <ArgList>
    ENDIF
endm

stdRET macro Func
    ret cByte&Func
endm

stdENDP macro Func
    Func endp
endm

endMod macro Func
    end Func
endm

stdCallCall macro Func, N
    IFDEF __imp_&Func&@&N
        call dword ptr [__imp_&Func&@&N]
    ELSE
        call Func&@&N
    ENDIF
endm

stdCall macro Func, ArgList
    LOCAL Bytes
    RevPush <ArgList>, Bytes
    Bytes = Bytes * 4
    stdCallCall Func, %(Bytes)
endm

MovAddr macro dest, addr, n
    ComposeInst <mov >, dest, <, offset FLAT:>, addr, <@>, n
endm

; FASTCALL SUPPORT - Use explicit declarations instead of macros
; In your .asm files, instead of:
;   EXTRNP HalRequestSoftwareInterrupt,1,IMPORT,FASTCALL
;   fstCall HalRequestSoftwareInterrupt
;
; Use:
;   EXTRN @HalRequestSoftwareInterrupt@4:NEAR
;   call @HalRequestSoftwareInterrupt@4

fstCall macro Func, ArgList
    ; This macro is now deprecated - use explicit calls
    .err <fstCall is deprecated. Use explicit: call @FuncName@N>
endm

cPublicFastCall macro Func, N, ArgList
    ; This macro is now deprecated - use explicit PROC declarations
    .err <cPublicFastCall is deprecated. Use explicit: @FuncName@N proc>
endm

fstRET macro Func
    ; This macro is now deprecated
    .err <fstRET is deprecated. Use explicit: ret N>
endm

fstENDP macro Func
    ; This macro is now deprecated
    .err <fstENDP is deprecated. Use explicit: @FuncName@N endp>
endm
