/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    kix86.h

Abstract:

    Private x86 platform-specific kernel declarations used by ki.h.

--*/

#if !defined(_KIX86_)
#define _KIX86_

#define KiGetCurrentReadySummary() \
    (KAFFINITY)__readfsdword(FIELD_OFFSET(KPCR, PrcbData.ReadySummary))

#define KiIpiSendSynchronousPacket(Prcb,Target,Function,P1,P2,P3)       \
    {                                                                   \
        extern PKPRCB KiSynchPacket;                                    \
        (Prcb)->CurrentPacket[0] = (PVOID)(P1);                          \
        (Prcb)->CurrentPacket[1] = (PVOID)(P2);                          \
        (Prcb)->CurrentPacket[2] = (PVOID)(P3);                          \
        (Prcb)->TargetSet = (Target);                                    \
        (Prcb)->WorkerRoutine = (Function);                              \
        if (((Target) & ((Target) - 1)) == 0) {                          \
            KiSynchPacket = (PKPRCB)((ULONG_PTR)(Prcb) | 1);             \
        } else {                                                         \
            KiSynchPacket = (Prcb);                                      \
            (Prcb)->PacketBarrier = 1;                                   \
        }                                                                \
        KiIpiSend((Target), IPI_SYNCH_REQUEST);                          \
    }

VOID
KiInitializePcr (
    IN ULONG Processor,
    IN PKPCR Pcr,
    IN PKIDTENTRY Idt,
    IN PKGDTENTRY Gdt,
    IN PKTSS Tss,
    IN PKTHREAD Thread,
    IN PVOID DpcStack
    );

VOID
KiFlushNPXState (
    PFLOATING_SAVE_AREA SaveArea
    );

#if defined(_MSC_VER) && defined(_X86_)

__inline
VOID
Kix86FxSave(
    PFX_SAVE_AREA NpxFrame
    )
{
    __asm {
        mov eax, NpxFrame
        _emit 0fh
        _emit 0aeh
        _emit 0
    }
}

__inline
VOID
Kix86FnSave(
    PFX_SAVE_AREA NpxFrame
    )
{
    __asm {
        mov eax, NpxFrame
        fnsave [eax]
    }
}

__inline
VOID
Kix86LdMXCsr(
    PULONG MXCsr
    )
{
    __asm {
        mov eax, MXCsr
        _emit 0fh
        _emit 0aeh
        _emit 10h
    }
}

__inline
VOID
Kix86StMXCsr(
    PULONG MXCsr
    )
{
    __asm {
        mov eax, MXCsr
        _emit 0fh
        _emit 0aeh
        _emit 18h
    }
}

#else

VOID Kix86FxSave(PFX_SAVE_AREA NpxFrame);
VOID Kix86FnSave(PFX_SAVE_AREA NpxFrame);
VOID Kix86LdMXCsr(PULONG MXCsr);
VOID Kix86StMXCsr(PULONG MXCsr);

#endif

VOID
Ke386ConfigureCyrixProcessor (
    VOID
    );

ULONG
KiCopyInformation (
    IN OUT PEXCEPTION_RECORD ExceptionRecord1,
    IN PEXCEPTION_RECORD ExceptionRecord2
    );

VOID
KiSetHardwareTrigger (
    VOID
    );

extern const ULONG KiDebugRegisterTrapOffsets[];
extern const ULONG KiDebugRegisterContextOffsets[];

BOOLEAN
FASTCALL
KiRecordDr7 (
    IN OUT PULONG Dr7Ptr,
    IN OUT PUCHAR Mask OPTIONAL
    );

BOOLEAN
FASTCALL
KiProcessDebugRegister (
    IN OUT PKTRAP_FRAME TrapFrame,
    IN ULONG Register
    );

ULONG
FASTCALL
KiUpdateDr7 (
    IN ULONG Dr7
    );

#ifdef DBGMP
VOID
KiPollDebugger (
    VOID
    );
#endif

VOID
FASTCALL
KiIpiSignalPacketDoneAndStall (
    IN PKIPI_CONTEXT Signaldone,
    IN ULONG volatile *ReverseStall
    );

extern KIRQL KiProfileIrql;

#define CONTEXT_ALIGNED_SIZE ((sizeof(CONTEXT) + CONTEXT_ROUND) & ~CONTEXT_ROUND)
C_ASSERT ((CONTEXT_ALIGNED_SIZE & CONTEXT_ROUND) == 0);

#define MAX_IDENTITYMAP_ALLOCATIONS 30

typedef struct _IDENTITY_MAP {
    PHARDWARE_PTE TopLevelDirectory;
    ULONG IdentityCR3;
    ULONG IdentityAddr;
    ULONG PagesAllocated;
    PVOID PageList[MAX_IDENTITYMAP_ALLOCATIONS];
} IDENTITY_MAP, *PIDENTITY_MAP;

VOID Ki386ClearIdentityMap(PIDENTITY_MAP IdentityMap);
VOID Ki386EnableTargetLargePage(PIDENTITY_MAP IdentityMap);
BOOLEAN Ki386CreateIdentityMap(
    IN OUT PIDENTITY_MAP IdentityMap,
    IN PVOID StartVa,
    IN PVOID EndVa
    );
BOOLEAN Ki386EnableCurrentLargePage(IN ULONG IdentityAddr, IN ULONG IdentityCr3);
extern PVOID Ki386EnableCurrentLargePageEnd;

#if defined(_X86PAE_)
#define PPI_BITS 2
#define PDI_BITS 9
#define PTI_BITS 9
#else
#define PPI_BITS 0
#define PDI_BITS 10
#define PTI_BITS 10
#endif

#define PPI_MASK ((1 << PPI_BITS) - 1)
#define PDI_MASK ((1 << PDI_BITS) - 1)
#define PTI_MASK ((1 << PTI_BITS) - 1)

#define KiGetPpeIndex(va) ((((ULONG)(va)) >> PPI_SHIFT) & PPI_MASK)
#define KiGetPdeIndex(va) ((((ULONG)(va)) >> PDI_SHIFT) & PDI_MASK)
#define KiGetPteIndex(va) ((((ULONG)(va)) >> PTI_SHIFT) & PTI_MASK)

extern LONG64 KiMtrrMaskBase;
extern LONG64 KiMtrrMaskMask;
extern LONG64 KiMtrrOverflowMask;
extern LONG64 KiMtrrResBitMask;
extern UCHAR KiMtrrMaxRangeShift;

#endif /* _KIX86_ */
