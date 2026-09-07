/*++

Copyright (c) ReactOS Project. All rights reserved.

Module Name:

    arm.h

Abstract:

    ARM architecture specific definitions for the NT OS Kernel.

Author:

    ReactOS Team

Revision History:

--*/

#pragma once

/* Architecture Identifier */
#define _ARM_ 1
#define _M_ARM 4

/* Processor Feature Bits */
#define PF_ARM_VFP_32_REGISTERS     0x00000001
#define PF_ARM_NEON_SUPPORTED       0x00000002
#define PF_ARM_THUMB_SUPPORTED      0x00000004
#define PF_ARM_DSP_SUPPORTED        0x00000008
#define PF_ARM_JAZELLE_SUPPORTED    0x00000010
#define PF_ARM_BIG_ENDIAN           0x00000020
#define PF_ARM_VFP_DOUBLE_PRECISION 0x00000040

/* Calling Convention Macros */
#ifndef __cdecl
#define __cdecl __attribute__((__cdecl__))
#endif

#ifndef __stdcall
#define __stdcall __attribute__((__stdcall__))
#endif

#ifndef __fastcall
#define __fastcall __attribute__((__fastcall__))
#endif

/* Force Inline */
#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif

/* Declspec Compatibility */
#ifndef __declspec
#define __declspec(x) __attribute__((x))
#endif

/* Built-in Functions */
#define _ReturnAddress() __builtin_return_address(0)
#define _AddressOfReturnAddress() ((PVOID)__builtin_frame_address(0))

/* Memory Barriers */
#define MemoryBarrier() __asm__ __volatile__ ("dmb ish" ::: "memory")
#define ReadMemoryBarrier() __asm__ __volatile__ ("dmb ishld" ::: "memory")
#define WriteMemoryBarrier() __asm__ __volatile__ ("dmb ishst" ::: "memory")

/* Cache Operations */
#define KeFlushInstructionCache(Process, Base, Length) \
    __asm__ __volatile__ ("mcr p15, 0, %0, c7, c5, 0" :: "r"(Base))

/* Interrupt Related */
#define KeRaiseIrql(new_irql, old_irql) \
    do { *old_irql = __builtin_arm_get_fpscr(); __builtin_arm_set_fpscr(new_irql); } while(0)

#define KeLowerIrql(old_irql) \
    __builtin_arm_set_fpscr(old_irql)

/* Atomic Operations */
#define InterlockedIncrement(dest) __sync_add_and_fetch((dest), 1)
#define InterlockedDecrement(dest) __sync_sub_and_fetch((dest), 1)
#define InterlockedExchange(dest, val) __sync_lock_test_and_set((dest), (val))
#define InterlockedCompareExchange(dest, exchange, compare) \
    __sync_val_compare_and_swap((dest), (compare), (exchange))

/* Stack Alignment */
#define STACK_ALIGNMENT 8

/* Page Size */
#define PAGE_SIZE 4096

/* Maximum Number of Processors */
#define MAXIMUM_PROCESSORS 32

/* Exception Handling (GCC-compatible stubs) */
#define __try
#define __except(x) if(0)
#define __finally
#define __leave goto __end
#define _exception_code() 0
#define _exception_info() NULL
#define AbnormalTermination() FALSE

/* End of file */
