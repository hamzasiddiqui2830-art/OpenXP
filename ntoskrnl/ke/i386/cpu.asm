        title   "Processor type and stepping detection"
;++
;
; Copyright (c) OpenXP.
; Refactored for MASM 14.x (Visual Studio 2022/2026) - x86 (32-bit) Target
;
; Module Name:
;
;    cpu.asm
;
; Abstract:
;
;    This module implements the assembly code necessary to determine
;    cpu type and stepping information.
;
; Environment:
;
;    80x86
;
;--

        .xlist
include ks386.inc
include i386\cpu.inc
include callconv.inc
include mac386.inc
        .list

;
; IRET frame offsets (for temporary exception handlers)
;
IretEip     equ 0
IretCs      equ 4
IretEFlags  equ 8

;
; constant for i386 32-bit multiplication test
;

MULTIPLIER            equ     00000081h
MULTIPLICAND          equ     0417a000h
RESULT_HIGH           equ     00000002h
RESULT_LOW            equ     0fe7a000h

;
; Constants for Floating Point test
;

REALLONG_LOW          equ     00000000
REALLONG_HIGH         equ     3FE00000h
PSEUDO_DENORMAL_LOW   equ     00000000h
PSEUDO_DENORMAL_MID   equ     80000000h
PSEUDO_DENORMAL_HIGH  equ     0000h

;
; Constants for GenuineIntel cpuid.0 vendor string
;
CPUID_0_INTEL_EBX     equ     0756e6547h
CPUID_0_INTEL_EDX     equ     049656e69h
CPUID_0_INTEL_ECX     equ     06c65746eh


.586p

INIT    SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; USHORT
; KiSetProcessorType (
;    VOID
;    )
;
;--
cPublicProc _KiSetProcessorType,0

        mov     byte ptr PCR[PcPrcbData+PbCpuID], 0

        push    edi
        push    esi
        push    ebx                     ; Save C registers
        mov     eax, cr0
        push    eax
        pushfd                          ; save Cr0 & flags

        pop     ebx                     ; Get flags into eax
        push    ebx                     ; Save original flags

        mov     ecx, ebx
        xor     ecx, EFLAGS_ID          ; flip ID bit
        push    ecx
        popfd                           ; load it into flags
        pushfd                          ; re-save flags
        pop     ecx                     ; get flags into eax
        cmp     ebx, ecx                ; did bit stay flipped?
        jne     short cpu_has_cpuid     ; Yes, go use CPUID

cpuid_unsupported:
        pop     ebx                     ; Get flags into eax
        push    ebx                     ; Save original flags

        mov     ecx, ebx
        xor     ecx, EFLAGS_AC          ; flip AC bit
        push    ecx
        popfd                           ; load it into flags
        pushfd                          ; re-save flags
        pop     ecx                     ; get flags into eax
        cmp     ebx, ecx                ; did bit stay flipped?
        je      short cpu_is_386        ; No, then this is a 386

cpu_is_486:
        mov     byte ptr PCR[PcPrcbData+PbCpuType], 4h    ; Save CPU Type
        call    Get486Stepping
        jmp     cpu_save_stepping

cpu_is_386:
        mov     byte ptr PCR[PcPrcbData+PbCpuType], 3h    ; Save CPU Type
        call    Get386Stepping
        jmp     cpu_save_stepping

cpu_has_cpuid:
        or      ebx, EFLAGS_ID
        push    ebx
        popfd                           ; Make sure ID bit is set

        mov     ecx, PCR[PcIdt]         ; Address of IDT
        push    dword ptr [ecx+30h]     ; Save Trap06 handler incase
        push    dword ptr [ecx+34h]     ; the CPUID instruction faults

        mov     eax, offset CpuIdTrap6Handler
        mov     word ptr [ecx+30h], ax  ; Set LowWord
        shr     eax, 16
        mov     word ptr [ecx+36h], ax  ; Set HighWord

        mov     eax, 0                  ; argument to CPUID
        cpuid                           ; Uses eax, ebx, ecx, edx

        mov     ecx, PCR[PcIdt]         ; Address of IDT
        pop     dword ptr [ecx+34h]     ; restore trap6 handler
        pop     dword ptr [ecx+30h]

        cmp     eax, 1                  ; make sure level 1 is supported
        jc      short cpuid_unsupported ; no, then punt

        mov     eax, 0                  ; get the vendor string
        cpuid

        cmp     ebx, CPUID_0_INTEL_EBX
        jne     short cpu_non_genuineintel

        cmp     ecx, CPUID_0_INTEL_ECX
        jne     short cpu_non_genuineintel

        cmp     edx, CPUID_0_INTEL_EDX
        jne     short cpu_non_genuineintel


        ;
        ; GenuineIntel, check for Family=6
        ;
        mov     eax, 1                  ; get the family and stepping
        cpuid

        mov     ebx, eax
        mov     edx, eax
        mov     ecx, eax                

        and     edx, 0F00h              ; get the Family
        cmp     edx, 0600h
        jne     short cpu_not_family_6  ; Family not 6

                                        ; Family 6, ExtendedModel is valid
        and     ebx, 0F00h              ; (bh) = CpuType
        jmp     short extended_model


cpu_non_genuineintel:

        mov     eax, 1                  ; get the family and stepping
        cpuid

        mov     ebx, eax
        mov     edx, eax                
        mov     ecx, eax

        and     edx, 0F00h              ; get the Family

cpu_not_family_6:
        cmp     edx, 0F00h
        jne     short cpu_not_extended  ; Family less than F
    
        and     ebx, 0FF00000h
        shr     ebx, 12
        add     ebx, edx

extended_model:
        mov     ah, al
        shr     eax, 4
        mov     al, cl
        and     eax, 0FF0Fh

        jmp     short cpu_save_signature

cpu_not_extended:
        and     eax, 0F0h               ; (eax) = Model
        shl     eax, 4
        mov     al, bl
        and     eax, 0F0Fh              ; (eax) = Model[15:8] | Step[7:0]

        and     ebx, 0F00h              ; (bh) = CpuType

cpu_save_signature:
        mov     byte ptr PCR[PcPrcbData+PbCpuID], 1       ; Has ID support
        mov     byte ptr PCR[PcPrcbData+PbCpuType], bh    ; Save CPU Type

cpu_save_stepping:
        mov     word ptr PCR[PcPrcbData+PbCpuStep], ax    ; Save CPU Stepping
        popfd                                   ; Restore flags
        pop     eax
        mov     cr0, eax
        pop     ebx
        pop     esi
        pop     edi
        stdRET  _KiSetProcessorType

PUBLIC cpuid_trap
cpuid_trap:
        mov     ecx, PCR[PcIdt]         ; Address of IDT
        pop     dword ptr [ecx+34h]     ; restore trap6 handler
        pop     dword ptr [ecx+30h]
        jmp     cpuid_unsupported       ; Go get processor information

stdENDP _KiSetProcessorType

;++
;
; CpuIdTrap6Handler
;
;    Temporary int 6 handler - assumes the cause of the exception was the
;    attempted CPUID instruction.
;--

CpuIdTrap6Handler   proc

        mov     [esp+IretEip],offset cpuid_trap
        iretd

CpuIdTrap6Handler  endp

;++
; Get386Stepping
;--

        public  Get386Stepping
Get386Stepping  proc

        call    MultiplyTest            ; Perform multiplication test
        jnc     short G3s00             ; if nc, muttest is ok
        mov     ax, 0
        ret
G3s00:
        call    Check386B0              ; Check for B0 stepping
        jnc     short G3s05             ; if nc, it's B1/later
        mov     ax, 100h                ; It is B0/earlier stepping
        ret

G3s05:
        call    Check386D1              ; Check for D1 stepping
        jc      short G3s10             ; if c, it is NOT D1
        mov     ax, 301h                ; It is D1/later stepping
        ret

G3s10:
        mov     ax, 101h                ; assume it is B1 stepping
        ret

Get386Stepping  endp

;++
; Get486Stepping
;--

        public  Get486Stepping
Get486Stepping          proc

        call    Check486AStepping       ; Check for A stepping
        jnc     short G4s00             ; if nc, it is NOT A stepping

        mov     ax, 0                   ; set to A stepping
        ret

G4s00:  call    Check486BStepping       ; Check for B stepping
        jnc     short G4s10             ; if nc, it is NOT a B stepping

        mov     ax, 100h                ; set to B stepping
        ret

G4s10:
        call    _KiIsNpxPresent         ; Check if cpu has coprocessor support?
        or      ax, ax
        jz      short G4s15             ; it is actually 486sx

        call    Check486CStepping       ; Check for C stepping
        jnc     short G4s20             ; if nc, it is NOT a C stepping
G4s15:
        mov     ax, 200h                ; set to C stepping
        ret

G4s20:  mov     ax, 300h                ; Set to D stepping
        ret

Get486Stepping          endp

;++
; Check486AStepping
;--
        public  Check486AStepping
Check486AStepping       proc    near
        mov     eax, cr0                ; reset ET bit in cr0
        and     eax, NOT CR0_ET
        mov     cr0, eax

        mov     eax, cr0                ; get cr0 back
        test    eax, CR0_ET             ; if ET bit still set?
        jnz     short cas10             ; if nz, yes, still set, it's NOT A step
        stc
        ret

cas10:  clc
        ret
Check486AStepping       endp

;++
; Check486BStepping
;--
        public  Check486BStepping
Check486BStepping       proc

        push    ebx

        mov     ebx, PCR[PcIdt]           ; Address of IDT
        push    dword ptr [ebx+30h]
        push    dword ptr [ebx+34h]     ; Save Trap06 handler

        mov     eax, offset Temporary486Int6
        mov     word ptr [ebx+30h], ax  ; Set LowWord
        shr     eax, 16
        mov     word ptr [ebx+36h], ax  ; Set HighWord

c4bs50:
        db      0fh, 21h, 0e0h          ; mov eax, DR4
        nop
        nop
        nop
        nop
        nop
        clc                             ; it is C step
        jmp     short c4bs70
PUBLIC c4bs60
c4bs60: stc                             ; it's B step
c4bs70: pop     dword ptr [ebx+34h]     ; restore old int 6 vector
        pop     dword ptr [ebx+30h]

        pop     ebx
        ret

        ret

Check486BStepping       endp

;++
; Temporary486Int6
;--

Temporary486Int6        proc

        mov     [esp+IretEip],offset c4bs60 ; set EIP to stc instruction
        iretd

Temporary486Int6        endp

;++
; Check486CStepping
;--

FpControl       equ     [ebp - 2]
RealLongSt1     equ     [ebp - 10]
PseudoDenormal  equ     [ebp - 20]
FscaleResult    equ     [ebp - 30]

        public  Check486CStepping
Check486CStepping       proc

        push    ebp
        mov     ebp, esp
        sub     esp, 30                 ; Allocate space for temp real variables

        mov     eax, cr0                ; Don't trap while doing math
        and     eax, NOT (CR0_ET+CR0_MP+CR0_TS+CR0_EM)
        mov     cr0, eax

        mov     dword ptr RealLongSt1, REALLONG_LOW
        mov     dword ptr RealLongSt1 + 4, REALLONG_HIGH
        mov     dword ptr PseudoDenormal, PSEUDO_DENORMAL_LOW
        mov     dword ptr PseudoDenormal + 4, PSEUDO_DENORMAL_MID
        mov     word ptr PseudoDenormal + 8, PSEUDO_DENORMAL_HIGH

.387
        fnstcw  FpControl               ; Get FP control word
        fwait
        or      word ptr FpControl, 0FFh ; Mask all the FP exceptions
        fldcw   FpControl               ; Set FP control

        fld     qword ptr RealLongSt1   ; 0 < ST(1) = RealLongSt1 < 1
        fld     tbyte ptr PseudoDenormal; Denormalized operand.
        fscale                          ; try to trigger 486Cx errata
        fstp    tbyte ptr FscaleResult  ; Store ST(0) in FscaleResult
        cmp     word ptr FscaleResult + 8, PSEUDO_DENORMAL_HIGH
        jz      short c4ds00            ; if z, no, it is C step
        clc
        jmp     short c4ds10
c4ds00: stc
c4ds10: mov     esp, ebp
        pop     ebp
        ret

Check486CStepping       endp

;++
; Check386B0
;--

Check386B0      proc

        push    ebx

        mov     ebx, PCR[PcIdt]           ; Address of IDT
        push    dword ptr [ebx+30h]
        push    dword ptr [ebx+34h]     ; Save Trap06 handler

        mov     eax, offset TemporaryInt6
        mov     word ptr [ebx+30h], ax  ; Set LowWord
        shr     eax, 16
        mov     word ptr [ebx+36h], ax  ; Set HighWord

        xor     eax,eax
        mov     edx,eax
        mov     ecx,0ff00h              ; Extract length (CL) == 0, (CX) != 0

b1c50:
        db      0fh, 0a6h, 0cah         ; xbts cx,dx,ax,cl
        nop
        nop
        nop
        nop
        nop
        stc                             ; assume B0
        jecxz    short b1c70            ; jmp if B0
PUBLIC b1c60
b1c60:  clc
b1c70:  pop     dword ptr [ebx+34h]     ; restore old int 6 vector
        pop     dword ptr [ebx+30h]

        pop     ebx
        ret

Check386B0      endp

;++
; TemporaryInt6
;--

TemporaryInt6    proc

        mov     [esp+IretEip],offset b1c60 ; set IP to clc instruction
        iretd

TemporaryInt6   endp

;++
; Check386D1
;--

Check386D1      proc
        push    ebx

        mov     ebx, PCR[PcIdt]           ; Address of IDT
        push    dword ptr [ebx+08h]
        push    dword ptr [ebx+0ch]     ; Save Trap01 handler

        mov     eax, offset TemporaryInt1
        mov     word ptr [ebx+08h], ax  ; Set LowWord
        shr     eax, 16
        mov     word ptr [ebx+0eh], ax  ; Set HighWord

        sub     esp,4                   ; make room for target of movsb
        mov     esi, offset TemporaryInt1 ; (ds:esi) -> some present data
        mov     edi,esp
        mov     ecx,2                   ; 2 iterations
        pushfd
        or      dword ptr [esp], EFLAGS_TF
        popfd                           ; cause a single step trap
        rep movsb

PUBLIC d1c60
d1c60:  add     esp,4                   ; clean off stack
        pop     dword ptr [ebx+0ch]     ; restore old int 1 vector
        pop     dword ptr [ebx+08h]
        stc                             ; assume B1
        jecxz   short d1cx              ; jmp if <= B1
        clc                             ; else clear carry to indicate >= D1
d1cx:
        pop     ebx
        ret

Check386D1      endp

;++
; TemporaryInt1
;--

TemporaryInt1   proc

        and     DWORD PTR [esp+IretEFlags], NOT EFLAGS_TF ; clear caller's Trace Flag
        mov     [esp+IretEip],offset d1c60     ; set IP to next instruction
        iretd

TemporaryInt1   endp

;++
; MultiplyTest
;--

MultiplyTest    proc

        xor     cx,cx                   ; 64K times is a nice round number
mlt00:  push    cx
        call    Multiply                ; does this chip's multiply work?
        pop     cx
        jc      short mltx              ; if c, No, exit
        loop    mlt00                   ; if nc, YEs, loop to try again
        clc
mltx:
        ret

MultiplyTest    endp

;++
; Multiply
;--

Multiply        proc

        mov     ecx, MULTIPLIER
        mov     eax, MULTIPLICAND
        mul     ecx

        cmp     edx, RESULT_HIGH        ; Q: high order answer OK ?
        stc                             ; assume failure
        jnz     short mlpx              ;   N: exit with error

        cmp     eax, RESULT_LOW         ; Q: low order answer OK ?
        stc                             ; assume failure
        jnz     short mlpx              ;   N: exit with error

        clc                             ; indicate success
mlpx:
        ret

Multiply        endp

;++
; KiIsNpxPresent
;--

cPublicProc _KiIsNpxPresent,0

        push    ebp                     ; Save caller's bp
        mov     eax, cr0
        and     eax, NOT (CR0_ET+CR0_MP+CR0_TS+CR0_EM)
        mov     cr0, eax
        xor     edx, edx

        fninit                          ; Initialize NPX
        mov     ecx, 5A5A5A5Ah          ; Put non-zero value
        push    ecx                     ;   into the memory we are going to use
        mov     ebp, esp
        fnstsw  word ptr [ebp]          ; Retrieve status - must use non-wait
        cmp     byte ptr [ebp], 0       ; All bits cleared by fninit?
        jne     Inp10

        or      eax, CR0_ET
        mov     edx, 1

        cmp     BYTE PTR PCR[PcPrcbData+PbCpuType], 3h
        jbe     Inp10

        or      eax, CR0_NE

Inp10:
        or      eax, CR0_EM+CR0_TS      ; During Kernel Initialization set
                                        ; the EM bit
        mov     cr0, eax
        pop     eax                     ; clear scratch value
        pop     ebp                     ; Restore caller's bp
        mov     eax, edx
        stdRet  _KiIsNpxPresent


stdENDP _KiIsNpxPresent


;++
; CPUID
;--
cPublicProc _CPUID,5

    push    ebx
    push    esi

    mov     eax, [esp+12]

    cpuid

    mov     esi, [esp+16]   ; return EAX
    mov     [esi], eax

    mov     esi, [esp+20]   ; return EBX
    mov     [esi], ebx

    mov     esi, [esp+24]   ; return ECX
    mov     [esi], ecx

    mov     esi, [esp+28]   ; return EDX
    mov     [esi], edx

    pop     esi
    pop     ebx

    stdRET  _CPUID

stdENDP _CPUID

INIT    ENDS

_TEXT   SEGMENT DWORD PUBLIC 'CODE'      ; Put IdleLoop in text section
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
; RDTSC
;--
cPublicProc _RDTSC
    rdtsc
    stdRET  _RDTSC

stdENDP _RDTSC

;++
; RDMSR (fastcall)
;--
; Explicit fastcall declaration (replaces cPublicFastCall/fstRET/fstENDP)
PUBLIC @RDMSR@4
@RDMSR@4 proc
    rdmsr
    ret
@RDMSR@4 endp


;++
; WRMSR
;--
cPublicProc _WRMSR, 3
    mov     ecx, [esp+4]
    mov     eax, [esp+8]
    mov     edx, [esp+12]
    wrmsr
    stdRET  _WRMSR
stdENDP _WRMSR

;++
; KeYieldProcessor
;--
cPublicProc _KeYieldProcessor
    YIELD
    stdRET _KeYieldProcessor
stdENDP _KeYieldProcessor

_TEXT   ENDS
        END
