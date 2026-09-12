;
; Copyright (c) OpenXP
;
;

;****************************CallConv.Inc************************************

;****************************Public Macro************************************
;
;   ComposeInst Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9
;
;       This macro simply concatenates all arguments into one string.
;
;****************************************************************************

ComposeInst macro   Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9
        @CatStr(Inst,p1,p2,p3,p4,p5,p6,p7,p8,p9)
endm

;****************************Public Macro************************************
;
;   CountArg    cCount,ArgList
;
;       This macro count the number of arguments in the ArgList and returns
;       the value in cCount.
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

;**************************** Calling convention macros *********************

if      @Version GE 600
        option  nokeyword:<stdcall>
endif

;----------------------------------------------------------------------------
; PUBLICP
;
; Standard-call public symbol:
;
;       Func@N
;----------------------------------------------------------------------------

PUBLICP macro   Func,N

        ifb    <N>

            public  @CatStr(Func,<@>,<0>)

        else

            PUBLICP2 Func,%(N*4)

        endif

endm


PUBLICP2 macro   Func,N

        public  @CatStr(Func,<@>,%N)

endm

;----------------------------------------------------------------------------
; EXTRNP
;
; Standard-call external symbol:
;
;       Func@N
;
; Fast-call external symbol:
;
;       @Func@N
;
; Also records the number of stack bytes used by the fast-call function in:
;
;       cFCall@Func
;----------------------------------------------------------------------------

EXTRNP macro   Func,N,Thunk,FastCall

        ifb    <N>

            Local SymbolName
            SymbolName TEXTEQU @CatStr(Func,<@>,<0>)

            IFNDEF &SymbolName&
                extrn &SymbolName&:NEAR
            ENDIF

        else

            ifb <FastCall>

                ifb <Thunk>

                    EXTRNP2 Func,%(N*4)

                else

                    EXTRNTHUNK Func,%(N*4)

                endif

            else

                cFCallName TEXTEQU @CatStr(<cFCall@>,Func)
                cFCallName = (N*4)

                ifb <Thunk>

                    EXTRNP2 @CatStr(<@>,Func),%(N*4)

                else

                    EXTRNTHUNK @CatStr(<@>,Func),%(N*4)

                endif

            endif

        endif

endm


EXTRNP2 macro   Func,N

        Local SymbolName

        SymbolName TEXTEQU @CatStr(Func,<@>,%N)

        IFNDEF &SymbolName&
            extrn &SymbolName&:NEAR
        ENDIF

endm


EXTRNTHUNK macro   Func,N

        Local SymbolName

        SymbolName TEXTEQU @CatStr(<__imp_>,Func,<@>,%N)

        IFNDEF &SymbolName&
            extrn &SymbolName&:DWORD
        ENDIF

endm

;----------------------------------------------------------------------------
; LABELP
;----------------------------------------------------------------------------

LABELP macro   Func,N

        ifb <N>

            @CatStr(Func,<@>,<0>) label near

        else

            LABELP2 Func,%(N*4)

        endif

endm


LABELP2 macro   Func,N

        @CatStr(Func,<@>,%N) label near

endm

;----------------------------------------------------------------------------
; ProcName
;----------------------------------------------------------------------------

ProcName macro  Name,Func,N

        ifb <N>

            cByteName TEXTEQU @CatStr(<cByte>,Func)
            cByteName = 0

            Name TEXTEQU @CatStr(Func,<@>,<0>)

        else

            cByteName TEXTEQU @CatStr(<cByte>,Func)
            cByteName = N

            Name TEXTEQU @CatStr(Func,<@>,%N)

        endif

endm

;----------------------------------------------------------------------------
; stdPROC
;----------------------------------------------------------------------------

stdPROC macro   Func,N,ArgList

        ProcName    Func,Func,%(N*4)

        Func        proc    ArgList

endm

;----------------------------------------------------------------------------
; cPublicProc
;----------------------------------------------------------------------------

cPublicProc macro Func,N,ArgList

        align   dword

        PUBLICP Func,N

        ifb <N>

            stdPROC Func,0,<ArgList>

        else

            stdPROC Func,N,<ArgList>

        endif

endm

;----------------------------------------------------------------------------
; ProcNameF
;----------------------------------------------------------------------------

ProcNameF macro  Name,Func,N,M

        cByteName TEXTEQU @CatStr(<cByte>,Func)
        cByteName = M

        cFCallName TEXTEQU @CatStr(<cFCall>,Func)
        cFCallName = N

        Name TEXTEQU @CatStr(Func,<@>,%N)

endm

;----------------------------------------------------------------------------
; stdPROCF
;----------------------------------------------------------------------------

stdPROCF macro   Func,N,ArgList

        if N gt 2

            ProcNameF Func,Func,%(N*4),%((N-2)*4)

        else

            ProcNameF Func,Func,%(N*4),0

        endif

        Func        proc    ArgList

endm

;----------------------------------------------------------------------------
; cPublicFastCall
;
; Public fast-call symbol:
;
;       @Func@N
;----------------------------------------------------------------------------

cPublicFastCall macro Func,N,ArgList

        align   dword

        PUBLICP @CatStr(<@>,Func),N

        ifb <N>

            stdPROCF @CatStr(<@>,Func),0,<ArgList>

        else

            stdPROCF @CatStr(<@>,Func),N,<ArgList>

        endif

endm

;----------------------------------------------------------------------------
; fstRET
;----------------------------------------------------------------------------

fstRET macro   Func

        cByteName TEXTEQU @CatStr(<cByte>,<@>,Func)

        ret cByteName

endm

;----------------------------------------------------------------------------
; stdRET
;----------------------------------------------------------------------------

stdRET macro   Func

        cByteName TEXTEQU @CatStr(<cByte>,Func)

        ret cByteName

endm

;----------------------------------------------------------------------------
; cPublicFpo
;----------------------------------------------------------------------------

cPublicFpo macro FpoLocals, FpoParams

.FPO ( FpoParams, FpoLocals, 0, 0, 0, 0 )

endm

;----------------------------------------------------------------------------
; fstENDP
;----------------------------------------------------------------------------

fstENDP macro   Func

        @CatStr(<@>,Func) endp

endm

;----------------------------------------------------------------------------
; stdENDP
;----------------------------------------------------------------------------

stdENDP macro   Func

        Func endp

endm

;----------------------------------------------------------------------------
; endMod
;----------------------------------------------------------------------------

endMod macro   Func

        end Func

endm

;----------------------------------------------------------------------------
; stdCallCall
;
; If the imported __imp_ symbol exists, call through the import address.
;
; Otherwise call the normal decorated symbol directly.
;
;----------------------------------------------------------------------------

stdCallCall macro Func,N

        Local ImpName

        ImpName TEXTEQU @CatStr(<__imp_>,Func,<@>,%N)

        IFDEF &ImpName&

            call dword ptr [&ImpName&]

        ELSE

            call @CatStr(Func,<@>,%N)

        ENDIF

endm

;----------------------------------------------------------------------------
; stdCall
;----------------------------------------------------------------------------

stdCall macro   Func,ArgList

        Local   Bytes

        RevPush <ArgList>,Bytes

        Bytes = Bytes*4

        stdCallCall Func,%(Bytes)

endm

;----------------------------------------------------------------------------
; fstCall
;----------------------------------------------------------------------------

fstCall macro   Func,ArgList

        Local   Bytes

        RevPush <ArgList>,Bytes

        Bytes = Bytes*4

        if Bytes eq 0

            cFCallName TEXTEQU @CatStr(<cFCall@>,Func)

            stdCallCall @CatStr(<@>,Func),%&cFCallName&

        else

            ; must have 2 register params

            stdCallCall @CatStr(<@>,Func),%(Bytes+8)

        endif

endm

;----------------------------------------------------------------------------
; MovAddr
;----------------------------------------------------------------------------

MovAddr macro   dest,addr,n

        ComposeInst <mov >,dest,<,offset FLAT:>,addr,<@>,n

endm
