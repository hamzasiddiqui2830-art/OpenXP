/*++

Copyright (c) ReactOS Project. All rights reserved.

Module Name:

    arm64.h

Abstract:

    ARM64 (AArch64) architecture specific definitions for the NT OS Kernel.

Author:

    ReactOS Team

Revision History:

--*/

#pragma once

/* Architecture Identifier */
#define _ARM64_ 1
#define _M_ARM64 4

/* Processor Feature Bits */
#define PF_ARM64_VFP_SUPPORTED      0x00000001
#define PF_ARM64_NEON_SUPPORTED     0x00000002
#define PF_ARM64_CRYPTO_SUPPORTED   0x00000004
#define PF_ARM64_CRC32_SUPPORTED    0x00000008
#define PF_ARM64_LSE_SUPPORTED      0x00000010
#define PF_ARM64_SHA1_SUPPORTED     0x00000020
#define PF_ARM64_SHA256_SUPPORTED   0x00000040

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
    __asm__ __volatile__ ("dc ivac, %0; dsb ish; ic ialluis; dsb ish; isb" :: "r"(Base))

/* Interrupt Related */
#define KeRaiseIrql(new_irql, old_irql) \
    do { *old_irql = __builtin_arm_get_pstate(); __builtin_arm_set_pstate(new_irql); } while(0)

#define KeLowerIrql(old_irql) \
    __builtin_arm_set_pstate(old_irql)

/* Atomic Operations (AArch64 has native atomics) */
#define InterlockedIncrement(dest) __atomic_add_fetch((dest), 1, __ATOMIC_SEQ_CST)
#define InterlockedDecrement(dest) __atomic_sub_fetch((dest), 1, __ATOMIC_SEQ_CST)
#define InterlockedExchange(dest, val) __atomic_exchange_n((dest), (val), __ATOMIC_SEQ_CST)
#define InterlockedCompareExchange(dest, exchange, compare) \
    __atomic_compare_exchange_n((dest), &(compare), (exchange), 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* Stack Alignment */
#define STACK_ALIGNMENT 16

/* Page Size */
#define PAGE_SIZE 4096

/* Maximum Number of Processors */
#define MAXIMUM_PROCESSORS 256

/* Exception Handling (GCC-compatible stubs) */
#define __try
#define __except(x) if(0)
#define __finally
#define __leave goto __end
#define _exception_code() 0
#define _exception_info() NULL
#define AbnormalTermination() FALSE

/* End of file */
