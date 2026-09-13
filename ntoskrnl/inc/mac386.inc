;++
;
; Copyright (c) Microsoft Corporation. All rights reserved.
; Project OpenXP Internal
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    mac386.inc - 386 machine specific assembler macros
;
; Abstract:
;
;   This module contains 386 machine specific (assembler) macros
;   applicable to code outside the kernel.  Note that
;   ACQUIRE_SPINLOCK_DIRECT assumes the PCR is handy, so it won't
;   work in user mode (with debugging turned on.)
;
;--

IFDEF _CAPKERN
    EXTRN _CAP_Log_NInt:PROC
ENDIF

;++
;  YIELD
;
;  Macro Description:
;     This macro implements the yield instruction (pause).
;--
YIELD macro
    IFNDEF NT_UP
        pause   ; Modern MASM supports 'pause' directly (translates to F3 90)
    ENDIF
endm


IFNDEF NT_INST

;++
;  ACQUIRE_SPINLOCK LockAddress, SpinLabel, NoChecking
;
;  Macro Description:
;     This macro acquires a kernel spin lock.
;     N.B. This macro assumes that the current IRQL is set properly.
;        It neither raises nor lowers IRQL.
;--
ACQUIRE_SPINLOCK macro LockAddress, SpinLabel, NoChecking
    IFB <LockAddress>
        .err <LockAddress argument is required>
    ENDIF
    IFB <SpinLabel>
        .err <SpinLabel argument is required>
    ENDIF

    IFNDEF NT_UP
        IFDEF CAPKERN_SYNCH_POINTS
            push LockAddress
            push 000010101h    ; 1 Dword, Timestamp, Subcode = 1
            call _CAP_Log_NInt
            add esp, 8
        ENDIF

        lock bts dword ptr [LockAddress], 0     ; test and set the spinlock
        jc SpinLabel                              ; spinlock owned, go SpinLabel

        IFDEF DBG
            IFB <NoChecking>
                push edi                          ; save edi
                mov edi, fs:PcPrcb
                mov edi, [edi].PbCurrentThread
                or edi, 1                         ; spinlock owned
                mov [LockAddress], edi            ; remember current thread
                pop edi                           ; restore edi
            ENDIF
        ENDIF
    ENDIF
endm

;++
;  SPIN_ON_SPINLOCK LockAddress, AcquireLabel, NoChecking, PollDebugger, NoTimeout
;
;  Macro Description:
;     This macro spins on a kernel spin lock.
;     N.B. This macro assumes that the current IRQL is set properly.
;        It neither raises nor lowers IRQL.
;--
SPIN_ON_SPINLOCK macro LockAddress, AcquireLabel, NoChecking, PollDebugger, NoTimeout
    LOCAL a, b, flag

    IFB <LockAddress>
        .err <LockAddress argument is required>
    ENDIF
    IFB <AcquireLabel>
        .err <AcquireLabel argument is required>
    ENDIF

    IFNDEF NT_UP
        IFDEF DBG
            EXTRNP Kii386SpinOnSpinLock, 2
            flag = 0

            IFB <NoChecking>
                flag = flag + 1
            ENDIF

            IFNB <PollDebugger>
                flag = flag + 2
            ENDIF

            IFB <NoTimeout>
                flag = flag + 4
            ENDIF
            
            stdCall Kii386SpinOnSpinLock, <LockAddress, flag>
            jmp AcquireLabel

        ELSE
            ; Non-Debug version
            IFDEF CAPKERN_SYNCH_POINTS
                push eax
                push ebx
                mov eax, LockAddress
                xor ebx, ebx
a:              inc ebx
                test dword ptr [eax], 1           ; Was spinlock cleared?
                jz short b                        ; Yes, go get it
                YIELD
                jmp short a

b:              
                push eax
                push ebx
                push 000020102h                   ; 2 Dwords, Timestamp, Subcode = 2
                call _CAP_Log_NInt
                add esp, 12
                pop ebx
                pop eax
                jmp AcquireLabel
            ELSE
a:              test dword ptr [LockAddress], 1   ; Was spinlock cleared?
                jz AcquireLabel                   ; Yes, go get it
                YIELD
                jmp short a
            ENDIF
        ENDIF
    ENDIF
endm

;++
;  TEST_SPINLOCK LockAddress, BusyLabel
;
;  Macro Description:
;     This macro tests a kernel spin lock to see if it's busy.
;     If it's not busy, ACQUIRE_SPINLOCK still needs to be called
;     to obtain the spinlock in a locked manner.
;--
TEST_SPINLOCK macro LockAddress, BusyLabel
    test dword ptr [LockAddress], 1               ; spinlock clear?
    jnz BusyLabel                                 ; No, then busy
endm

;++
;  RELEASE_SPINLOCK LockAddress, NoChecking
;
;  Macro Description:
;     This macro releases a kernel spin lock.
;     N.B. This macro assumes that the current IRQL is set properly.
;        It neither raises nor lowers IRQL.
;--
RELEASE_SPINLOCK macro LockAddress, NoChecking
    LOCAL a
    
    IFB <LockAddress>
        .err <LockAddress argument is required>
    ENDIF

    IFNDEF NT_UP
        IFDEF DBG
            IFB <NoChecking>
                EXTRNP _KeBugCheckEx, 5

                push edi                          ; save edi
                mov edi, fs:PcPrcb
                mov edi, [edi].PbCurrentThread
                or edi, 1                         ; assume current thread owns the lock
                cmp edi, [LockAddress]            ; Does current thread own the lock?
                pop edi                           ; restore edi
                jz short a                        ; if z, yes, goto a and release lock
                stdCall _KeBugCheckEx, <SPIN_LOCK_NOT_OWNED, LockAddress, 0, 0, 0>
a:
            ENDIF
            lock and dword ptr [LockAddress], 0
        ELSE
            lock and byte ptr [LockAddress], 0
        ENDIF

        IFDEF CAPKERN_SYNCH_POINTS
            push LockAddress
            push 000010107h                       ; 1 Dword, Timestamp, Subcode = 7
            call _CAP_Log_NInt
            add esp, 8
        ENDIF
    ENDIF
endm

ENDIF ; IFNDEF NT_INST


IFDEF NT_INST
;
; These are the instrumentation versions of the above functions.
; Internal use only.
;

ACQUIRE_SPINLOCK macro LockAddress, SpinLabel, NoChecking
    EXTRNP KiInst_AcquireSpinLock, 0
    IFIDNI <&LockAddress>, <eax>
        stdCall KiInst_AcquireSpinLock
    ELSE
        push eax
        mov eax, LockAddress
        stdCall KiInst_AcquireSpinLock
        pop eax
    ENDIF
    jc SpinLabel
endm

SPIN_ON_SPINLOCK macro LockAddress, AcquireLabel, NoChecking, PollDebugger
    EXTRNP KiInst_SpinOnSpinLock, 0
    IFIDNI <&LockAddress>, <eax>
        stdCall KiInst_SpinOnSpinLock
    ELSE
        push eax
        mov eax, LockAddress
        stdCall KiInst_SpinOnSpinLock
        pop eax
    ENDIF
    jmp AcquireLabel
endm

TEST_SPINLOCK macro LockAddress, BusyLabel
    EXTRNP KiInst_TestSpinLock, 0
    IFIDNI <&LockAddress>, <eax>
        stdCall KiInst_TestSpinLock
    ELSE
        push eax
        mov eax, LockAddress
        stdCall KiInst_TestSpinLock
        pop eax
    ENDIF
    jnc BusyLabel  ; Fixed typo from original source (was incorrectly 'AcquireLabel')
endm

RELEASE_SPINLOCK macro LockAddress, NoChecking
    EXTRNP KiInst_ReleaseSpinLock, 0
    IFIDNI <&LockAddress>, <eax>
        stdCall KiInst_ReleaseSpinLock
    ELSE
        push eax
        mov eax, LockAddress
        stdCall KiInst_ReleaseSpinLock
        pop eax
    ENDIF
endm

ENDIF ; IFDEF NT_INST
