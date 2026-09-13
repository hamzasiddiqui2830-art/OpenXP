/*
 * Processor type and stepping detection
 *
 * Copyright (c) Open XP.
 *
 * Module Name:
 *
 *    cpu.S
 *
 * Abstract:
 *
 *    This module implements the assembly code necessary to determine
 *    cpu type and stepping information.
 *
 * Environment:
 *
 *    80x86
 */

#include <i386/cpu.inc>
#include <ks386.inc>
#include <callconv.inc>
#include <mac386.inc>

#define MULTIPLIER              0x00000081
#define MULTIPLICAND             0x0417a000
#define RESULT_HIGH              0x00000002
#define RESULT_LOW               0x0fe7a000

#define REALLONG_LOW             0x00000000
#define REALLONG_HIGH            0x3FE00000
#define PSEUDO_DENORMAL_LOW      0x00000000
#define PSEUDO_DENORMAL_MID      0x80000000
#define PSEUDO_DENORMAL_HIGH     0x0000

#define CPUID_0_INTEL_EBX        0x756e6547
#define CPUID_0_INTEL_EDX        0x49656e69
#define CPUID_0_INTEL_ECX        0x6c65746e

/*
 * KiSetProcessorType
 *
 * USHORT
 * KiSetProcessorType(VOID)
 *
 * Determines processor type and stepping information.
 */

    .section .init
    .align  4

    FUNC _KiSetProcessorType, 0

    movb    $0, PCR[PcPrcbData + PbCpuID]

    pushl   %edi
    pushl   %esi
    pushl   %ebx

    movl    %cr0, %eax
    pushl   %eax
    pushfl

    popl    %ebx
    pushl   %ebx

    movl    %ebx, %ecx
    xorl    $EFLAGS_ID, %ecx
    pushl   %ecx
    popfl
    pushfl
    popl    %ecx

    cmpl    %ebx, %ecx
    jne     cpu_has_cpuid

cpuid_unsupported:

    popl    %ebx
    pushl   %ebx

    movl    %ebx, %ecx
    xorl    $EFLAGS_AC, %ecx
    pushl   %ecx
    popfl
    pushfl
    popl    %ecx

    cmpl    %ebx, %ecx
    je      cpu_is_386

cpu_is_486:

    movb    $4, PCR[PcPrcbData + PbCpuType]
    call    Get486Stepping
    jmp     cpu_save_stepping

cpu_is_386:

    movb    $3, PCR[PcPrcbData + PbCpuType]
    call    Get386Stepping
    jmp     cpu_save_stepping

cpu_has_cpuid:

    orl     $EFLAGS_ID, %ebx
    pushl   %ebx
    popfl

    movl    PCR[PcIdt], %ecx

    pushl   0x30(%ecx)
    pushl   0x34(%ecx)

    movl    $CpuIdTrap6Handler, %eax
    movw    %ax, 0x30(%ecx)

    shrl    $16, %eax
    movw    %ax, 0x36(%ecx)

    xorl    %eax, %eax
    cpuid

    movl    PCR[PcIdt], %ecx

    popl    0x34(%ecx)
    popl    0x30(%ecx)

    cmpl    $1, %eax
    jc      cpuid_unsupported

    /*
     * Get vendor string.
     */

    xorl    %eax, %eax
    cpuid

    cmpl    $CPUID_0_INTEL_EBX, %ebx
    jne     cpu_non_genuineintel

    cmpl    $CPUID_0_INTEL_ECX, %ecx
    jne     cpu_non_genuineintel

    cmpl    $CPUID_0_INTEL_EDX, %edx
    jne     cpu_non_genuineintel

    /*
     * GenuineIntel, check for Family=6.
     */

    movl    $1, %eax
    cpuid

    movl    %eax, %ebx
    movl    %eax, %edx
    movl    %eax, %ecx

    andl    $0x0F00, %edx
    cmpl    $0x0600, %edx
    jne     cpu_not_family_6

    /*
     * Family 6: ExtendedModel is valid.
     */

    andl    $0x0F00, %ebx
    jmp     extended_model

cpu_non_genuineintel:

    movl    $1, %eax
    cpuid

    movl    %eax, %ebx
    movl    %eax, %edx
    movl    %eax, %ecx

    andl    $0x0F00, %edx

cpu_not_family_6:

    cmpl    $0x0F00, %edx
    jne     cpu_not_extended

    andl    $0xFF00000, %ebx
    shrl    $12, %ebx
    addl    %edx, %ebx

extended_model:

    movb    %al, %ah
    shrl    $4, %eax
    movb    %cl, %al
    andl    $0xFF0F, %eax

    jmp     cpu_save_signature

cpu_not_extended:

    andl    $0xF0, %eax
    shll    $4, %eax
    movb    %bl, %al
    andl    $0xF0F, %eax

    andl    $0x0F00, %ebx

cpu_save_signature:

    movb    $1, PCR[PcPrcbData + PbCpuID]
    movb    %bh, PCR[PcPrcbData + PbCpuType]

cpu_save_stepping:

    movw    %ax, PCR[PcPrcbData + PbCpuStep]

    popfl
    popl    %eax
    movl    %eax, %cr0

    popl    %ebx
    popl    %esi
    popl    %edi

    stdRET  _KiSetProcessorType

cpuid_trap:

    movl    PCR[PcIdt], %ecx
    popl    0x34(%ecx)
    popl    0x30(%ecx)

    jmp     cpuid_unsupported

    ENDPROC _KiSetProcessorType


/*
 * Temporary CPUID #UD/#GP handler.
 */

    FUNC CpuIdTrap6Handler, 0

    movl    $cpuid_trap, IretEip(%esp)
    iretd

    ENDPROC CpuIdTrap6Handler


/*
 * Get386Stepping
 */

    FUNC Get386Stepping, 0

    call    MultiplyTest
    jnc     G3s00

    xorl    %eax, %eax
    ret

G3s00:

    call    Check386B0
    jnc     G3s05

    movl    $0x100, %eax
    ret

G3s05:

    call    Check386D1
    jc      G3s10

    movl    $0x301, %eax
    ret

G3s10:

    movl    $0x101, %eax
    ret

    ENDPROC Get386Stepping


/*
 * Get486Stepping
 */

    FUNC Get486Stepping, 0

    call    Check486AStepping
    jnc     G4s00

    xorl    %eax, %eax
    ret

G4s00:

    call    Check486BStepping
    jnc     G4s10

    movl    $0x100, %eax
    ret

G4s10:

    call    _KiIsNpxPresent
    testl   %eax, %eax
    jz      G4s15

    call    Check486CStepping
    jnc     G4s20

G4s15:

    movl    $0x200, %eax
    ret

G4s20:

    movl    $0x300, %eax
    ret

    ENDPROC Get486Stepping


/*
 * Check486AStepping
 */

    FUNC Check486AStepping, 0

    movl    %cr0, %eax
    andl    $~CR0_ET, %eax
    movl    %eax, %cr0

    movl    %cr0, %eax
    testl   $CR0_ET, %eax
    jnz     cas10

    stc
    ret

cas10:

    clc
    ret

    ENDPROC Check486AStepping


/*
 * Check486BStepping
 */

    FUNC Check486BStepping, 0

    pushl   %ebx

    movl    PCR[PcIdt], %ebx

    pushl   0x30(%ebx)
    pushl   0x34(%ebx)

    movl    $Temporary486Int6, %eax
    movw    %ax, 0x30(%ebx)

    shrl    $16, %eax
    movw    %ax, 0x36(%ebx)

c4bs50:

    /*
     * mov eax, DR4
     *
     * Keep the raw opcode because modern assemblers may reject
     * explicit DR4 usage.
     */
    .byte   0x0f, 0x21, 0xe0

    nop
    nop
    nop
    nop
    nop

    clc
    jmp     c4bs70

c4bs60:

    stc

c4bs70:

    popl    0x34(%ebx)
    popl    0x30(%ebx)

    popl    %ebx
    ret

    ENDPROC Check486BStepping


/*
 * Temporary486Int6
 */

    FUNC Temporary486Int6, 0

    movl    $c4bs60, IretEip(%esp)
    iretd

    ENDPROC Temporary486Int6


/*
 * Check486CStepping
 */

    .equ FpControl,      -2
    .equ RealLongSt1,   -10
    .equ PseudoDenormal,-20
    .equ FscaleResult,  -30

    FUNC Check486CStepping, 0

    pushl   %ebp
    movl    %esp, %ebp
    subl    $0x30, %esp

    movl    %cr0, %eax
    andl    $~(CR0_ET + CR0_MP + CR0_TS + CR0_EM), %eax
    movl    %eax, %cr0

    movl    $REALLONG_LOW, RealLongSt1(%ebp)
    movl    $REALLONG_HIGH, RealLongSt1+4(%ebp)

    movl    $PSEUDO_DENORMAL_LOW, PseudoDenormal(%ebp)
    movl    $PSEUDO_DENORMAL_MID, PseudoDenormal+4(%ebp)
    movw    $PSEUDO_DENORMAL_HIGH, PseudoDenormal+8(%ebp)

    fnstcw  FpControl(%ebp)
    fwait

    orw     $0xFF, FpControl(%ebp)
    fldcw   FpControl(%ebp)

    fldl    RealLongSt1(%ebp)
    fldt    PseudoDenormal(%ebp)

    fscale

    fstpt   FscaleResult(%ebp)

    cmpw    $PSEUDO_DENORMAL_HIGH, FscaleResult+8(%ebp)
    jz      c4ds00

    clc
    jmp     c4ds10

c4ds00:

    stc

c4ds10:

    movl    %ebp, %esp
    popl    %ebp
    ret

    ENDPROC Check486CStepping


/*
 * Check386B0
 */

    FUNC Check386B0, 0

    pushl   %ebx

    movl    PCR[PcIdt], %ebx

    pushl   0x30(%ebx)
    pushl   0x34(%ebx)

    movl    $TemporaryInt6, %eax
    movw    %ax, 0x30(%ebx)

    shrl    $16, %eax
    movw    %ax, 0x36(%ebx)

    xorl    %eax, %eax
    movl    %eax, %edx

    movl    $0xFF00, %ecx

b1c50:

    /*
     * xbts cx,dx,ax,cl
     *
     * Keep the legacy instruction as raw bytes.
     */
    .byte   0x0f, 0xa6, 0xca

    nop
    nop
    nop
    nop
    nop

    stc
    jecxz   b1c70

b1c60:

    clc

b1c70:

    popl    0x34(%ebx)
    popl    0x30(%ebx)

    popl    %ebx
    ret

    ENDPROC Check386B0


/*
 * TemporaryInt6
 */

    FUNC TemporaryInt6, 0

    movl    $b1c60, IretEip(%esp)
    iretd

    ENDPROC TemporaryInt6


/*
 * Check386D1
 */

    FUNC Check386D1, 0

    pushl   %ebx

    movl    PCR[PcIdt], %ebx

    pushl   0x08(%ebx)
    pushl   0x0c(%ebx)

    movl    $TemporaryInt1, %eax
    movw    %ax, 0x08(%ebx)

    shrl    $16, %eax
    movw    %ax, 0x0e(%ebx)

    subl    $4, %esp

    movl    $TemporaryInt1, %esi
    movl    %esp, %edi

    movl    $2, %ecx

    pushfl
    orl     $EFLAGS_TF, (%esp)
    popfl

    rep
    movsb

d1c60:

    addl    $4, %esp

    popl    0x0c(%ebx)
    popl    0x08(%ebx)

    stc
    jecxz   d1cx

    clc

d1cx:

    popl    %ebx
    ret

    ENDPROC Check386D1


/*
 * TemporaryInt1
 */

    FUNC TemporaryInt1, 0

    andl    $~EFLAGS_TF, IretEFlags(%esp)
    movl    $d1c60, IretEip(%esp)
    iretd

    ENDPROC TemporaryInt1


/*
 * MultiplyTest
 */

    FUNC MultiplyTest, 0

    xorl    %ecx, %ecx

mlt00:

    pushw   %cx
    call    Multiply
    popw    %cx

    jc      mltx

    loop    mlt00

    clc

mltx:

    ret

    ENDPROC MultiplyTest


/*
 * Multiply
 */

    FUNC Multiply, 0

    movl    $MULTIPLIER, %ecx
    movl    $MULTIPLICAND, %eax
    mull    %ecx

    cmpl    $RESULT_HIGH, %edx
    stc
    jnz     mlpx

    cmpl    $RESULT_LOW, %eax
    stc
    jnz     mlpx

    clc

mlpx:

    ret

    ENDPROC Multiply


/*
 * KiIsNpxPresent
 */

    FUNC _KiIsNpxPresent, 0

    pushl   %ebp

    movl    %cr0, %eax
    andl    $~(CR0_ET + CR0_MP + CR0_TS + CR0_EM), %eax
    movl    %eax, %cr0

    xorl    %edx, %edx

    fninit

    movl    $0x5A5A5A5A, %ecx
    pushl   %ecx

    movl    %esp, %ebp

    fnstsw  (%ebp)

    cmpb    $0, (%ebp)
    jne     Inp10

    orl     $CR0_ET, %eax
    movl    $1, %edx

    cmpb    $3, PCR[PcPrcbData + PbCpuType]
    jbe     Inp10

    orl     $CR0_NE, %eax

Inp10:

    orl     $(CR0_EM + CR0_TS), %eax
    movl    %eax, %cr0

    popl    %eax
    popl    %ebp

    movl    %edx, %eax

    stdRET  _KiIsNpxPresent

    ENDPROC _KiIsNpxPresent


/*
 * CPUID
 */

    FUNC _CPUID, 5

    pushl   %ebx
    pushl   %esi

    movl    12(%esp), %eax
    cpuid

    movl    16(%esp), %esi
    movl    %eax, (%esi)

    movl    20(%esp), %esi
    movl    %ebx, (%esi)

    movl    24(%esp), %esi
    movl    %ecx, (%esi)

    movl    28(%esp), %esi
    movl    %edx, (%esi)

    popl    %esi
    popl    %ebx

    stdRET  _CPUID

    ENDPROC _CPUID


/*
 * RDTSC
 */

    .text
    .align  4

    FUNC _RDTSC, 0

    rdtsc

    stdRET  _RDTSC

    ENDPROC _RDTSC


/*
 * RDMSR
 */

    FUNC RDMSR, 1

    rdmsr

    fstRET  RDMSR

    ENDPROC RDMSR


/*
 * WRMSR
 */

    FUNC _WRMSR, 3

    movl    4(%esp), %ecx
    movl    8(%esp), %eax
    movl    12(%esp), %edx

    wrmsr

    stdRET  _WRMSR

    ENDPROC _WRMSR


/*
 * KeYieldProcessor
 */

    FUNC _KeYieldProcessor, 0

    YIELD

    stdRET  _KeYieldProcessor

    ENDPROC _KeYieldProcessor
