        title   "Interval Clock Interrupt"
;++
;
; Copyright (c) Microsoft Corporation. All rights reserved.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    clockint.asm
;
; Abstract:
;
;    This module implements the code necessary to field and process the
;    interval clock interrupt.
;
;--

.586p
        .xlist
KERNELONLY  EQU     1
include ks386.inc
include callconv.inc
include i386\kimacro.inc
include mac386.inc
        .list

        EXTRN   _HalEndSystemInterrupt@8:NEAR
        EXTRN   __imp_Kei386EoiHelper@0:DWORD     ; <-- ADD THIS LINE
        EXTRN   _ExpInterlockedPopEntrySListEnd@0:PROC
        EXTRN   _ExpInterlockedPopEntrySListResume@0:PROC
        EXTRN   _KeTimeIncrement:DWORD
        EXTRN   _KeMaximumIncrement:DWORD
        EXTRN   _KeTickCount:DWORD
        EXTRN   _KeTimeAdjustment:DWORD
        EXTRN   _KiAdjustDpcThreshold:DWORD
        EXTRN   _KiIdealDpcRate:DWORD
        EXTRN   _KiMaximumDpcQueueDepth:DWORD
        EXTRN   _KiTickOffset:DWORD
        EXTRN   _KiTimerTableListHead:DWORD
        EXTRN   _KiProfileListHead:DWORD
        EXTRN   _KiProfileLock:DWORD
        EXTRN   _KiProfileInterval:DWORD
        EXTRN   _KdDebuggerEnabled:BYTE
        EXTRNP  _DbgBreakPoint
        EXTRNP  _DbgBreakPointWithStatus, 1
        EXTRNP  _KdPollBreakIn
        EXTRNP  _KiDeliverApc, 3
        EXTRN   _KeI386MachineType:DWORD
        EXTRN   _PPerfGlobalGroupMask:DWORD

; Fastcall imports - explicit declarations with @ decoration
        EXTRN   @HalRequestSoftwareInterrupt@4:NEAR
        EXTRN   @PerfProfileInterrupt@8:NEAR
        EXTRN   @KiCheckForSListAddress@4:NEAR

IF DBG
        EXTRN   _DbgPrint:NEAR
        EXTRN   _KiDPCTimeout:DWORD
        EXTRN   _MsgDpcTimeout:BYTE
ENDIF

_DATA   SEGMENT  DWORD PUBLIC 'DATA'
        PUBLIC  ProfileCount
ProfileCount    DD      0
_DATA   ENDS

        PAGE ,132
        SUBTTL  "Update System Time"

_TEXT$00   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

cPublicProc _KeUpdateSystemTime, 0

; .FPO (2, 0, 0, 0, 0, 1)
; NOTE: .FPO is obsolete in MASM 14.x.

IF DBG
        CMP     BYTE PTR PCR[PcPrcbData+PbSkipTick], 0
        JNZ     kust_skiptick
ENDIF

        MOV     ECX, USER_SHARED_DATA
        MOV     EDI, [ECX+UsInterruptTime]
        MOV     ESI, [ECX+UsInterruptTime+4]
        ADD     EDI, EAX
        ADC     ESI, 0
        MOV     [ECX+UsInterruptTime+8], ESI
        MOV     [ECX+UsInterruptTime], EDI
        MOV     [ECX+UsInterruptTime+4], ESI

IFNDEF NT_UP
   LOCK SUB     _KiTickOffset, EAX
ELSE
        SUB     _KiTickOffset, EAX
ENDIF

        MOV     EAX, _KeTickCount
        MOV     EBX, EAX
        JG      kust10

        MOV     EBX, USER_SHARED_DATA
        MOV     ECX, [EBX+UsSystemTime]
        MOV     EDX, [EBX+UsSystemTime+4]
        ADD     ECX, _KeTimeAdjustment
        ADC     EDX, 0
        MOV     [EBX+UsSystemTime+8], EDX
        MOV     [EBX+UsSystemTime], ECX
        MOV     [EBX+UsSystemTime+4], EDX
        MOV     EBX, EAX

        MOV     ECX, EAX
        MOV     EDX, _KeTickCount+4
        ADD     ECX, 1
        ADC     EDX, 0
        MOV     _KeTickCount+8, EDX
        MOV     _KeTickCount, ECX
        MOV     _KeTickCount+4, EDX
        MOV     USERDATA[UsTickCount+8], EDX
        MOV     USERDATA[UsTickCount], ECX
        MOV     USERDATA[UsTickCount+4], EDX

        .ERRNZ  (TIMER_ENTRY_SIZE - 16)

        AND     EAX, TIMER_TABLE_SIZE-1
        SHL     EAX, 4
        CMP     ESI, [EAX+_KiTimerTableListHead+TtTime+4]
        JB      SHORT kust5
        JA      SHORT kust15
        CMP     EDI, [EAX+_KiTimerTableListHead+TtTime]
        JAE     SHORT kust15
kust5:  INC     EBX
        MOV     EAX, EBX

kust10: AND     EAX, TIMER_TABLE_SIZE-1
        SHL     EAX, 4
        CMP     ESI, [EAX+_KiTimerTableListHead+TtTime+4]
        JB      kustxx
        JA      SHORT kust15
        CMP     EDI, [EAX+_KiTimerTableListHead+TtTime]
        JB      kustxx
kust15:

        MOV     ECX, PCR[PcPrcb]
        CMP     DWORD PTR [ECX+PbTimerRequest], 0
        JNE     SHORT kustxx
        MOV     [ECX+PbTimerRequest], ESP
        MOV     [ECX+PbTimerHand], EBX
        MOV     ECX, DISPATCH_LEVEL
        call    @HalRequestSoftwareInterrupt@4    ; fstCall replacement

kustxx: CMP     _KdDebuggerEnabled, 0
        JNZ     SHORT kust45

kust30: CMP     _KiTickOffset, 0
        JG      SHORT Kust40

        MOV     EAX, _KeMaximumIncrement
        ADD     _KiTickOffset, EAX

        PUSH    [ESP]
        CALL    _KeUpdateRunTime@4

        INTERRUPT_EXIT

kust40:
        INC     DWORD PTR PCR[PcPrcbData+PbInterruptCount]
        INTERRUPT_EXIT

kust45: stdCall _KdPollBreakIn
        OR      AL, AL
        JZ      SHORT kust30
        stdCall _DbgBreakPointWithStatus, <DBG_STATUS_CONTROL_C>
        JMP     SHORT kust30

IF DBG
kust_skiptick:
        MOV     BYTE PTR PCR[PcPrcbData+PbSkipTick], 0
        JMP     SHORT kust40
ENDIF

stdENDP _KeUpdateSystemTime


        PAGE ,132
        SUBTTL  "Update Thread and Process Runtime"

cPublicProc _KeUpdateRunTime, 1
; cPublicFpo 1, 1  ; REMOVED: obsolete in MASM 14.x

        MOV     EAX, PCR[PcSelfPcr]
IF DBG
        CMP     BYTE PTR [EAX+PcPrcbData+PbSkipTick], 0
        JNZ     kutp_skiptick
ENDIF
        PUSH    EBX
        INC     DWORD PTR [EAX+PcPrcbData+PbInterruptCount]
        MOV     EBX, [EAX+PcPrcbData+PbCurrentThread]
        MOV     ECX, [EBX+ThApcState+AsProcess]

        TEST    DWORD PTR [EBP+TsEFlags], EFLAGS_V86_MASK
        JNE     Kutp20

        TEST    BYTE PTR [EBP+TsSegCs], MODE_MASK
        JNE     Kutp20

        INC     DWORD PTR [EAX+PcPrcbData+PbKernelTime]
        CMP     BYTE PTR [ESP+8], DISPATCH_LEVEL
        JC      SHORT Kutp4
        JA      SHORT Kutp3

        CMP     BYTE PTR PCR[PcPrcbData+PbDpcRoutineActive], 0
        JZ      SHORT Kutp4

        INC     DWORD PTR [EAX+PcPrcbData+PbDpcTime]
IF DBG
        INC     DWORD PTR [EAX+PcPrcbData+PbDebugDpcTime]
        MOV     EDX, _KiDPCTimeout
        CMP     DWORD PTR [EAX+PcPrcbData+PbDebugDpcTime], EDX
        JC      Kutp50

        PUSH    OFFSET FLAT:_MsgDpcTimeout
        CALL    _DbgPrint
        ADD     ESP, 1 * 4

        CMP     _KdDebuggerEnabled, 0
        JE      SHORT Kutp6
        stdCall _DbgBreakPoint

Kutp6:  MOV     EAX, PCR[PcSelfPcr]
        MOV     DWORD PTR [EAX+PcPrcbData+PbDebugDpcTime], 0
ENDIF
        JMP     Kutp50

ALIGN 4
Kutp3:
        INC     DWORD PTR [EAX+PcPrcbData+PbInterruptTime]
        JMP     Kutp50

ALIGN 4
Kutp4:
        INC     DWORD PTR [EBX+ThKernelTime]
        JMP     Kutp50

ALIGN 4
Kutp20:
        INC     DWORD PTR [EAX+PcPrcbData+PbUserTime]
        INC     DWORD PTR [EBX+ThUserTime]

ALIGN 4
Kutp50: MOV     ECX, [EAX+PcPrcbData+PbDpcCount]
        MOV     EDX, [EAX+PcPrcbData+PbDpcLastCount]
        MOV     [EAX+PcPrcbData+PbDpcLastCount], ECX
        SUB     ECX, EDX
        ADD     ECX, [EAX+PcPrcbData+PbDpcRequestRate]
        SHR     ECX, 1
        MOV     [EAX+PcPrcbData+PbDpcRequestRate], ECX

        CMP     DWORD PTR [EAX+PcPrcbData+PbDpcQueueDepth], 0
        JE      SHORT Kutp53
        CMP     BYTE PTR [EAX+PcPrcbData+PbDpcRoutineActive], 0
        JNE     SHORT Kutp53
        CMP     BYTE PTR [EAX+PcPrcbData+PbDpcInterruptRequested], 0
        JNE     SHORT Kutp53
        MOV     ECX, DISPATCH_LEVEL
        call    @HalRequestSoftwareInterrupt@4    ; fstCall replacement
        MOV     EAX, PCR[PcSelfPcr]
        MOV     ECX, [EAX+PcPrcbData+PbDpcRequestRate]
        MOV     EDX, _KiAdjustDpcThreshold
        MOV     [EAX+PcPrcbData+PbAdjustDpcThreshold], EDX
        CMP     ECX, _KiIdealDpcRate
        JGE     SHORT Kutp55
        CMP     DWORD PTR [EAX+PcPrcbData+PbMaximumDpcQueueDepth], 1
        JE      SHORT Kutp55
        DEC     DWORD PTR [EAX+PcPrcbData+PbMaximumDpcQueueDepth]
        JMP     SHORT Kutp55

Kutp53: DEC     DWORD PTR [EAX+PcPrcbData+PbAdjustDpcThreshold]
        JNZ     SHORT Kutp55
        MOV     ECX, _KiAdjustDpcThreshold
        MOV     [EAX+PcPrcbData+PbAdjustDpcThreshold], ECX
        MOV     ECX, _KiMaximumDpcQueueDepth
        CMP     ECX, [EAX+PcPrcbData+PbMaximumDpcQueueDepth]
        JE      SHORT Kutp55
        INC     DWORD PTR [EAX+PcPrcbData+PbMaximumDpcQueueDepth]

ALIGN 4
Kutp55: SUB     BYTE PTR [EBX+ThQuantum], CLOCK_QUANTUM_DECREMENT
        JG      Kutp75

        CMP     EBX, [EAX+PcPrcbData+PbIdleThread]
        JZ      Kutp75
        MOV     BYTE PTR [EAX+PcPrcbData+PbQuantumEnd], 1
        MOV     ECX, DISPATCH_LEVEL
        call    @HalRequestSoftwareInterrupt@4    ; fstCall replacement
Kutp75:
        POP     EBX
        stdRET    _KeUpdateRunTime

IF DBG
kutp_skiptick:
        MOV     BYTE PTR [EAX+PcPrcbData+PbSkipTick], 0
        stdRET    _KeUpdateRunTime
ENDIF

stdENDP _KeUpdateRunTime


;++
;   PROFILING SUPPORT
;--

cPublicProc _KeProfileInterrupt, 1
        POP     EAX
        POP     EBX
        PUSH    0
        PUSH    EBX
        PUSH    EAX
        JMP     SHORT _KeProfileInterruptWithSource@8
stdENDP _KeProfileInterrupt


cPublicProc _KeProfileInterruptWithSource, 2

kipieip         EQU     <DWORD PTR [EBP+TsEip]>
kipsegcs        EQU     <WORD PTR [EBP+TsSegCs]>
kipeflags       EQU     <DWORD PTR [EBP+TsEFlags]>

        MOV     EBP, DWORD PTR [ESP+4]
        INC     DWORD PTR PCR[PcPrcbData+PbInterruptCount]

        CMP     _PPerfGlobalGroupMask, 0
        JE      SHORT kipi03

        MOV     ECX, [ESP+8]              ; ProfileSource -> ECX (1st fastcall arg)
        MOV     EDX, kipieip              ; EIP -> EDX (2nd fastcall arg)
        call    @PerfProfileInterrupt@8   ; fstCall replacement
        MOV     EBP, DWORD PTR [ESP+4]

kipi03:
        CMP     kipieip, OFFSET FLAT:_ExpInterlockedPopEntrySListResume@0
        JB      kipi04
        CMP     kipieip, OFFSET FLAT:_ExpInterlockedPopEntrySListEnd@0
        JA      kipi04
        MOV     ECX, EBP                  ; TrapFrame -> ECX (1st fastcall arg)
        call    @KiCheckForSListAddress@4 ; fstCall replacement

kipi04:

IFNDEF NT_UP
        LEA     EAX, _KiProfileLock
kipi05: ACQUIRE_SPINLOCK    EAX, kipi96
ENDIF

        INC     ProfileCount

        MOV     EBX, kipieip
        MOV     EDX, OFFSET FLAT:_KiProfileListHead
        MOV     ESI, [EDX+LsFlink]
IFNDEF NT_UP
        MOV     EDI, PCR[PcSetMember]
ENDIF
        MOV     ECX, [ESP+8]
        CMP     ESI, EDX
        JE      kipi30

ALIGN 4
kipi10: CMP     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        JB      kipi20
        CMP     EBX, [ESI+PfRangeLimit-PfProfileListEntry]
        JAE     kipi20
        CMP     CX, WORD PTR [ESI+PfSource-PfProfileListEntry]
        JNE     kipi20
IFNDEF NT_UP
        TEST    EDI, [ESI+PfAffinity-PfProfileListEntry]
        JZ      kipi20
ENDIF

        SUB     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        MOV     CL, [ESI+PfBucketShift-PfProfileListEntry]
        SHR     EBX, CL
        AND     EBX, NOT 3
        MOV     EDI, [ESI+PfBuffer-PfProfileListEntry]
        INC     DWORD PTR [EDI+EBX]
        MOV     EBX, kipieip
        MOV     ECX, [ESP+8]
IFNDEF NT_UP
        MOV     EDI, PCR[PcSetMember]
ENDIF

ALIGN 4
kipi20: MOV     ESI, [ESI+LsFlink]
        CMP     ESI, EDX
        JNE     kipi10

ALIGN 4
kipi30: MOV     EAX, PCR[PcPrcbData+PbCurrentThread]
        MOV     EAX, [EAX+ThApcState+AsProcess]
        LEA     EDX, [EAX+PrProfileListHead]
        MOV     ESI, [EDX+LsFlink]
        CMP     ESI, EDX
        JE      kipi60

        MOVZX   ECX, WORD PTR kipsegcs
        TEST    kipeflags, EFLAGS_V86_MASK
        JNZ     kipi100

        CMP     CX, KGDT_R0_CODE
        JE      SHORT kipi40

        CMP     CX, KGDT_R3_CODE OR RPL_MASK
        JNE     kipi110

ALIGN 4
kipi40: CMP     WORD PTR [ESI+PfSegment-PfProfileListEntry], 0
        JNE     kipi50
        CMP     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        JB      kipi50
        CMP     EBX, [ESI+PfRangeLimit-PfProfileListEntry]
        JAE     kipi50
        MOV     ECX, [ESP+8]
        CMP     CX, WORD PTR [ESI+PfSource-PfProfileListEntry]
        JNE     kipi50
IFNDEF NT_UP
        MOV     EDI, PCR[PcSetMember]
        TEST    EDI, [ESI+PfAffinity-PfProfileListEntry]
        JZ      kipi50
ENDIF

        SUB     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        MOV     CL, [ESI+PfBucketShift-PfProfileListEntry]
        SHR     EBX, CL
        AND     EBX, NOT 3
        MOV     EDI, [ESI+PfBuffer-PfProfileListEntry]
        INC     DWORD PTR [EDI+EBX]
        MOV     EBX, kipieip
        MOV     ECX, [ESP+8]

ALIGN 4
kipi50: MOV     ESI, [ESI+LsFlink]
        CMP     ESI, EDX
        JNE     kipi40

ALIGN 4
kipi60:

IFNDEF NT_UP
        LEA     EAX, _KiProfileLock
        RELEASE_SPINLOCK    EAX
ENDIF
        stdRET  _KeProfileInterruptWithSource

IFNDEF NT_UP
ALIGN 4
kipi96: SPIN_ON_SPINLOCK    EAX, kipi05, , DbgMp
ENDIF

ALIGN 4
kipi100:
        SHL     ECX, 4
        ADD     EBX, ECX
        JMP     kipi40

ALIGN 4
kipi110:
        CMP     [ESI+PfSegment-PfProfileListEntry], CX
        JNE     kipi120
        CMP     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        JB      kipi120
        CMP     EBX, [ESI+PfRangeLimit-PfProfileListEntry]
        JAE     kipi120
        MOV     ECX, [ESP+8]
        CMP     CX, WORD PTR [ESI+PfSource-PfProfileListEntry]
        JNE     kipi120
IFNDEF NT_UP
        MOV     EDI, PCR[PcSetMember]
        TEST    EDI, [ESI+PfAffinity-PfProfileListEntry]
        JNZ     kipi120
ENDIF

        SUB     EBX, [ESI+PfRangeBase-PfProfileListEntry]
        MOV     CL, [ESI+PfBucketShift-PfProfileListEntry]
        SHR     EBX, CL
        AND     EBX, NOT 3
        MOV     EDI, [ESI+PfBuffer-PfProfileListEntry]
        INC     DWORD PTR [EDI+EBX]
        MOV     EBX, kipieip
        MOV     CX, kipsegcs

ALIGN 4
kipi120:
        MOV     ESI, [ESI+LsFlink]
        CMP     ESI, EDX
        JNE     kipi110

        JMP     kipi60

stdENDP _KeProfileInterruptWithSource
_TEXT$00   ENDS
        END
