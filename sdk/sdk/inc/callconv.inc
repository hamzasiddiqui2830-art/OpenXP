;
; Copyright (c) Microsoft Corporation. All rights reserved.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;

;****************************CallConv.Inc************************************

;****************************Public Macro************************************
;   ComposeInst Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9
;   Concatenates all arguments into one string.
;****************************************************************************
ComposeInst macro Inst, p1, p2, p3, p4, p5, p6, p7, p8, p9
    &Inst p1&p2&p3&p4&p5&p6&p7&p8&p9
endm

;****************************Public Macro************************************
;   CountArg cCount,ArgList
;   Counts the number of arguments in the ArgList.
;****************************************************************************
CountArg macro cCount, ArgList
    cCount = 0
    irp arg, <ArgList>
        cCount = cCount + 1
    endm
endm

;****************************Public Macro************************************
;   RevPush ArgList,cCount
;   Pushes arguments in reverse order.
;****************************************************************************
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

;****************************Calling Convention Macros***********************
; Modern MASM 14.x still supports stdcall/fastcall for 32-bit x86.
; Removed obsolete @Version checks.
;****************************************************************************

option nokeyword:<stdcall>

PUBLICP macro Func, N
    IFB <N>
        public Func&@0
    ELSE
        PUBLICP2 Func, %(N*4)
    ENDIF
endm

PUBLICP2 macro Func, N
    public Func&@&N
endm

EXTRNP  macro   Func,N,Thunk,FastCall
    IFB <N>
        IFNDEF Func&@0
            EXTRN Func&@0:NEAR
        ENDIF
    ELSE
        IFB <FastCall>
            IFB <Thunk>
                EXTRNP2 Func, %(N*4)
            ELSE
                EXTRNTHUNK Func, %(N*4)
            ENDIF
        ELSE
            cFCall&@&Func EQU (N*4)
            IFB <Thunk>
                EXTRNP2 &@&Func, %(N*4)
            ELSE
                EXTRNTHUNK &@&Func, %(N*4)
            ENDIF
        ENDIF
    ENDIF
endm

EXTRNP2 macro Func, N
    IFNDEF Func&@&N
        extrn Func&@&N:NEAR
    ENDIF
endm

EXTRNTHUNK macro Func, N
    IFNDEF __imp_&Func&@&N
        extrn __imp_&Func&@&N:DWORD
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

ProcNameF macro Name, Func, N, M
    cByte&Func equ M
    cFCall&Func equ N
    Name equ <Func&@&N>
endm

stdPROCF macro Func, N, ArgList
    IF N gt 2
        ProcNameF Func, Func, %(N*4), %((N-2)*4)
    ELSE
        ProcNameF Func, Func, %(N*4), 0
    ENDIF
    Func proc ArgList
endm

cPublicFastCall macro Func,N,ArgList
    ALIGN DWORD
    PUBLICP &@&Func, N
    IFB <N>
        stdPROCF &@&Func, 0, <ArgList>
    ELSE
        stdPROCF &@&Func, N, <ArgList>
    ENDIF
endm

fstRET  macro   Func
    ret     cByte&@&Func
endm

stdRET macro Func
    ret cByte&Func
endm

; .FPO is obsolete in modern MASM. Frame data is generated automatically.
cPublicFpo macro FpoLocals, FpoParams
    ; Legacy .FPO directive removed. 
    ; Use .ALLOCSTACK / .PUSHREG if manual frame definition is required.
endm

fstENDP macro   Func
    &@&Func endp
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

fstCall macro   Func,ArgList
    LOCAL Bytes

    RevPush <ArgList>,Bytes
    Bytes = Bytes*4

    IF Bytes eq 0
        stdCallCall &@&Func, %cFCall&@&Func
    ELSE
        stdCallCall &@&Func, %(Bytes+8)
    ENDIF
endm

MovAddr macro dest, addr, n
    ComposeInst <mov >, dest, <, offset FLAT:>, addr, <@>, n
endm
