;
; Copyright (c) OpenXP
;
;
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
        Inst    @CatStr(p1,p2,p3,p4,p5,p6,p7,p8,p9)
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
            public      @CatStr(Func,<@>,%0)
        else
            PUBLICP2    Func,%(N*4)
        endif
endm

PUBLICP2 macro   Func,N

        public  @CatStr(Func,<@>,%N)
endm

EXTRNP  macro   Func,N,Thunk,FastCall
        ifb    <N>
            IFNDEF  @CatStr(Func,<@>,%0)
                extrn       @CatStr(Func,<@>,%0):NEAR
            ENDIF
        else
            ifb     <FastCall>
                ifb     <Thunk>
                    EXTRNP2     Func,%(N*4)
                else
                    EXTRNTHUNK  Func,%(N*4)
                endif
            else
                @CatStr(cFCall,<@>,Func) equ   (N*4)
                ifb     <Thunk>
                    EXTRNP2     @CatStr(<@>,Func),%(N*4)
                else
                    EXTRNTHUNK  @CatStr(<@>,Func),%(N*4)
                endif
            endif
        endif
endm

EXTRNP2 macro   Func,N
        IFNDEF  @CatStr(Func,<@>,%N)
            extrn   @CatStr(Func,<@>,%N):NEAR
        ENDIF
endm

EXTRNTHUNK macro   Func,N
        IFNDEF  @CatStr(<__imp_>,Func,<@>,%N)
            extrn       @CatStr(<__imp_>,Func,<@>,%N):DWORD
        ENDIF
endm

LABELP  macro   Func,N

        ifb    <N>
            @CatStr(Func,<@>,%0) label   near
        else
            LABELP2 Func,%(N*4)
        endif
endm

LABELP2 macro   Func,N

@CatStr(Func,<@>,%N)    label   near

endm

ProcName macro   Name,Func,N

        ifb <N>
            @CatStr(cByte,<@>,Func)  equ     0
            Name        equ     <@CatStr(Func,<@>,%0)>
        else
            @CatStr(cByte,<@>,Func)  equ     N
            Name        equ     <@CatStr(Func,<@>,%N)>
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

        @CatStr(cByte,<@>,Func)  equ     M
        @CatStr(cFCall,<@>,Func) equ     N
        Name        equ     <@CatStr(Func,<@>,%N)>

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
        PUBLICP @CatStr(<@>,Func),N
        ifb <N>
            stdPROCF @CatStr(<@>,Func),0,<ArgList>
        else
            stdPROCF @CatStr(<@>,Func),N,<ArgList>
        endif
endm

fstRET  macro   Func
        ret     @CatStr(cByte,<@>,Func)
endm

stdRET  macro   Func
        ret     @CatStr(cByte,<@>,Func)
endm

cPublicFpo macro FpoLocals, FpoParams

.FPO ( FpoParams, FpoLocals, 0, 0, 0, 0 )

endm


fstENDP macro   Func

        @CatStr(<@>,Func)    endp
endm

stdENDP macro   Func

        Func    endp
endm

endMod  macro   Func

        end     Func
endm

stdCallCall macro  Func,N
    IFDEF   @CatStr(<__imp_>,Func,<@>,%N)
        call    dword ptr [@CatStr(<__imp_>,Func,<@>,%N)]
    ELSE
        call    @CatStr(Func,<@>,%N)
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

        RevPush <ArgList>,Bytes
        Bytes = Bytes*4

        if Bytes eq 0
            stdCallCall   @CatStr(<@>,Func),%cFCall@Func
        else
            ; must have 2 register params
            stdCallCall   @CatStr(<@>,Func),%(Bytes+8)
        endif
endm


MovAddr macro   dest,addr,n

        ComposeInst <mov >,dest,<,offset FLAT:>,addr,<@>,n

endm
