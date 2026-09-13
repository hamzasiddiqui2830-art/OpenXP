;++
;
; Copyright (c) Microsoft Corporation. All rights reserved.
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
        ; Attempt to assert the lock
        lock bts dword ptr [LockAddress], 0     ; test and set the spinlock
        jc SpinLabel                            ; spinlock owned, go SpinLabel

        IFDEF DBG
            IFB <NoChecking>
                push edi                        ; save edi
                mov edi, fs:PcPrcb
                mov edi, [edi].PbCurrentThread
                or edi, 1                       ; spinlock owned
                mov [LockAddress], edi          ; remember current thread
                pop edi                         ; restore edi
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
a:          test dword ptr [LockAddress], 1     ; Was spinlock cleared?
            jz AcquireLabel                     ; Yes, go get it
            YIELD
            jmp short a
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
    cmp dword ptr [LockAddress], 0              ; spinlock clear?
    jnz BusyLabel                               ; No, then busy
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

                push edi                        ; save edi
                mov edi, fs:PcPrcb
                mov edi, [edi].PbCurrentThread
                or edi, 1                       ; assume current thread owns the lock
                cmp edi, [LockAddress]          ; Does current thread own the lock?
                pop edi                         ; restore edi
                jz short a                      ; if z, yes, goto a and release lock
                stdCall _KeBugCheckEx, <SPIN_LOCK_NOT_OWNED, LockAddress, 0, 0, 0>
a:
            ENDIF
            lock and dword ptr [LockAddress], 0
        ELSE
            lock and byte ptr [LockAddress], 0
        ENDIF
    ENDIF
endm
