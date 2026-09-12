;
; Copyright (c) OpenXP
;
; You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
; If you do not agree to the terms, do not use the code.
;

;****************************CallConv.Inc************************************

;****************************Public Macro************************************
;
;   ComposeInst Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9
;
;       This macro simply concatenates all arguments into one string.
;
;
;****************************************************************************

ComposeInst macro   Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9
        &Inst   p1&p2&p3&p4&p5&p6&p7&p8&p9
endm

;****************************Public Macro************************************
;
;   CountArg    cCount,ArgList
;
;       This macro count the number of arguments in the ArgList and returns
;       the value in cCount.
;
;
;****************************************************************************

CountArg    macro   cCount,ArgList

        cCount = 0

        irp arg,<ArgList>
            cCount = cCount+1
        endm
endm

;****************************Public Macro************************************
;
;   RevPush     ArgList,cCount
;
;       This macro pushes the arguments in ArgList in the reverse order
;       and returns the number of arguments in cCount.
;
;
;****************************************************************************

RevPush macro   ArgList,cCount
        Local   index,x

        CountArg cCount,<ArgList>

        index  = cCount
        rept    cCount
            x = 0
            irp arg,<ArgList>
                x = x+1
                ife index-x
                    push    arg
                    exitm
                endif
            endm
            index = index-1
        endm
endm

;****************************Public Macro************************************
;
;   The following sections contain calling-convention related macros for:
;
;   PUBLICP     Func,N
;       to define a public label
;
;   EXTRNP      Func,N,Thunk
;       to define a external near label
;
;   LABELP      Func,N
;       to label an address as a routine entry point
;
;   stdPROC       Func,N,ArgList
;       to declare a routine header
;
;   ProcName    Name,Func,N
;       to rename a function Func to Name. Using it in conjunction with
;       normal function declaration (with the new name) will solve an error
;       caused by a long parameter list routine that exhausts page width.
;
;   stdRET        Func
;       to return from Func routines (declared with stdPROC or ProcName.)
;
;   stdENDP     Func
;       to declare the end of routine (declared with stdPROC or ProcName.)
;
;   endMod      Func
;       to declare the end of module with an entry point at Func (declared
;       with stdPROC or ProcName.)
;
;   stdCall     Func,ArgList
;       to call to a routine--Func--with the arguments pushed on the stack
;
;   MovAddr     dest,Func,n
;       to move the address of the routine--Func--into dest.
;
;   Note that for the standard calling convention all the function names,
;   Func, are automatically converted to Func@N where N is the number of
;   bytes (decimal) in the argument list.
;
;
;****************************************************************************

if      @Version GE 600
        option  nokeyword:<stdcall>
endif

PUBLICP macro   Func,N

        ifb    <N>
            public      Func&@0
        else
            PUBLICP2    Func,%(N*4)
        endif
endm

PUBLICP2 macro   Func,N
        Local   SymbolName

        SymbolName TEXTEQU @CatStr(Func,<@>,%N)
        public  &SymbolName&
endm

EXTRNP  macro   Func,N,Thunk,FastCall
        ifb     <N>
            Local SymbolName
            SymbolName TEXTEQU @CatStr(Func,<@>,<0>)
            IFNDEF &SymbolName&
                extrn   &SymbolName&:NEAR
            ENDIF
        else
            ifb     <FastCall>
                ifb     <Thunk>
                    EXTRNP2     Func,%(N*4)
                else
                    EXTRNTHUNK  Func,%(N*4)
                endif
            else
                Local CallName
                CallName TEXTEQU @CatStr(<@>,Func)
                cFCall&CallName equ (N*4)

                ifb     <Thunk>
                    EXTRNP2     &CallName,%(N*4)
                else
                    EXTRNTHUNK  &CallName,%(N*4)
                endif
            endif
        endif
endm

EXTRNP2 macro   Func,N
        Local   SymbolName

        SymbolName TEXTEQU @CatStr(Func,<@>,%N)

        IFNDEF &SymbolName&
            extrn   &SymbolName&:NEAR
        ENDIF
endm

EXTRNTHUNK macro   Func,N
        Local   SymbolName

        SymbolName TEXTEQU @CatStr(<__imp_>,Func,<@>,%N)

        IFNDEF &SymbolName&
            extrn   &SymbolName&:DWORD
        ENDIF
endm

LABELP  macro   Func,N

        ifb    <N>
            Func&@0 label   near
        else
            LABELP2 Func,%(N*4)
        endif
endm

LABELP2 macro   Func,N
        Local   SymbolName

        SymbolName TEXTEQU @CatStr(Func,<@>,%N)

&SymbolName& label   near
endm

ProcName macro  Name,Func,N

        ifb <N>
            cByte&Func  equ     0
            Name        equ     <Func&@0>
        else
            cByte&Func  equ     N
            Name        equ     <Func&@&N>
        endif
endm

stdPROC   macro   Func,N,ArgList

        ProcName    Func,Func,%(N*4)

        Func        proc    ArgList
endm

cPublicProc macro Func,N,ArgList
        align   dword
        PUBLICP Func,N
        ifb <N>
            stdPROC Func,0,<ArgList>
        else
            stdPROC Func,N,<ArgList>
        endif
endm

ProcNameF macro  Name,Func,N,M
        Local   ByteName
        Local   CallName
        Local   SymbolName

        ByteName   TEXTEQU @CatStr(<cByte>,Func)
        CallName   TEXTEQU @CatStr(<cFCall>,Func)
        SymbolName TEXTEQU @CatStr(Func,<@>,%N)

        &ByteName& equ M
        &CallName& equ N
        Name       equ <&SymbolName&>
endm

stdPROCF  macro   Func,N,ArgList

        if N gt 2
            ProcNameF   Func,Func,%(N*4),%((N-2)*4)
        else
            ProcNameF   Func,Func,%(N*4),0
        endif

        Func        proc    ArgList
endm

cPublicFastCall macro Func,N,ArgList
        align   dword
        PUBLICP &@&Func,N
        ifb <N>
            stdPROCF &@&Func,0,<ArgList>
        else
            stdPROCF &@&Func,N,<ArgList>
        endif
endm

fstRET  macro   Func
        Local ByteName

        ByteName TEXTEQU @CatStr(<cByte>,<@>,Func)

        ret     &ByteName&
endm

stdRET  macro   Func
        ret     cByte&Func
endm

cPublicFpo macro FpoLocals, FpoParams

.FPO ( FpoParams, FpoLocals, 0, 0, 0, 0 )

endm


fstENDP macro   Func
        Local ProcName

        ProcName TEXTEQU @CatStr(<@>,Func)

        &ProcName& endp
endm

stdENDP macro   Func

        Func    endp
endm

endMod  macro   Func

        end     Func
endm

stdCallCall macro  Func,N
        Local   ImportName
        Local   CallName

        ImportName TEXTEQU @CatStr(<__imp_>,Func,<@>,%N)
        CallName   TEXTEQU @CatStr(Func,<@>,%N)

        IFDEF &ImportName&
            call    dword ptr [&ImportName&]
        ELSE
            call    &CallName&
        ENDIF
endm


stdCall macro   Func,ArgList
        Local   Bytes

        RevPush <ArgList>,Bytes
        Bytes = Bytes*4

        stdCallCall   Func,%(Bytes)
endm

fstCall macro   Func,ArgList
        Local   Bytes
        Local   CallName
        Local   CallBytes

        RevPush <ArgList>,Bytes
        Bytes = Bytes*4

        CallName TEXTEQU @CatStr(<@>,Func)

        if Bytes eq 0
            CallBytes = %cFCall&CallName
            stdCallCall &CallName,%CallBytes
        else
            ; must have 2 register params
            CallBytes = Bytes+8
            stdCallCall &CallName,%CallBytes
        endif
endm


MovAddr macro   dest,addr,n

        ComposeInst <mov >,dest,<,offset FLAT:>,addr,<@>,n
endm

