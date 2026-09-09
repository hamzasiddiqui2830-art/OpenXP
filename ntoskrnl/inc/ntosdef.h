/*++ BUILD Version: 0003    // Increment this if a change has global effects

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    ntosdef.h

Abstract:

    Common type definitions for the NTOS component that are private to
    NTOS, but shared between NTOS sub-components.

Author:

    Steve Wood (stevewo) 08-May-1989

Revision History:

--*/

#ifndef _NTOSDEF_
#define _NTOSDEF_

#pragma warning(disable:4214)
#pragma warning(disable:4201)
#pragma warning(disable:4127)
#pragma warning(disable:4115)

struct _NPAGED_LOOKASIDE_LIST;
typedef struct _PP_LOOKASIDE_LIST {
    struct _GENERAL_LOOKASIDE *P;
    struct _GENERAL_LOOKASIDE *L;
} PP_LOOKASIDE_LIST, *PPP_LOOKASIDE_LIST;

#define POOL_SMALL_LISTS 32

#ifndef DBG
#define DBG 0
#endif

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif

#if DEVL
extern ULONG NtGlobalFlag;
#define IF_NTOS_DEBUG( FlagName ) if (NtGlobalFlag & (FLG_ ## FlagName))
#else
#define IF_NTOS_DEBUG( FlagName ) if (FALSE)
#endif

typedef CCHAR KPROCESSOR_MODE;
typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

struct _KAPC;
typedef VOID (*PKNORMAL_ROUTINE)(IN PVOID NormalContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
typedef VOID (*PKKERNEL_ROUTINE)(IN struct _KAPC *Apc, IN OUT PKNORMAL_ROUTINE *NormalRoutine, IN OUT PVOID *NormalContext, IN OUT PVOID *SystemArgument1, IN OUT PVOID *SystemArgument2);
typedef VOID (*PKRUNDOWN_ROUTINE)(IN struct _KAPC *Apc);
typedef BOOLEAN (*PKSYNCHRONIZE_ROUTINE)(IN PVOID SynchronizeContext);
typedef BOOLEAN (*PKTRANSFER_ROUTINE)(VOID);

typedef struct _KAPC {
    CSHORT Type;
    CSHORT Size;
    ULONG Spare0;
    struct _KTHREAD *Thread;
    LIST_ENTRY ApcListEntry;
    PKKERNEL_ROUTINE KernelRoutine;
    PKRUNDOWN_ROUTINE RundownRoutine;
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    CCHAR ApcStateIndex;
    KPROCESSOR_MODE ApcMode;
    BOOLEAN Inserted;
} KAPC, *PKAPC, *RESTRICTED_POINTER PRKAPC;

struct _KDPC;
typedef VOID (*PKDEFERRED_ROUTINE)(IN struct _KDPC *Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);

typedef enum _KDPC_IMPORTANCE {
    LowImportance,
    MediumImportance,
    HighImportance
} KDPC_IMPORTANCE;

#define DPC_NORMAL 0
#define DPC_THREADED 1

typedef struct _KDPC {
    CSHORT Type;
    UCHAR Number;
    UCHAR Importance;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PVOID DpcData;
} KDPC, *PKDPC, *RESTRICTED_POINTER PRKDPC;

typedef PVOID PKIPI_CONTEXT;
typedef VOID (*PKIPI_WORKER)(IN PKIPI_CONTEXT PacketContext, IN PVOID Parameter1, IN PVOID Parameter2, IN PVOID Parameter3);

typedef struct _KIPI_COUNTS {
    ULONG Freeze;
    ULONG Packet;
    ULONG DPC;
    ULONG APC;
    ULONG FlushSingleTb;
    ULONG FlushMultipleTb;
    ULONG FlushEntireTb;
    ULONG GenericCall;
    ULONG ChangeColor;
    ULONG SweepDcache;
    ULONG SweepIcache;
    ULONG SweepIcacheRange;
    ULONG FlushIoBuffers;
    ULONG GratuitousDPC;
} KIPI_COUNTS, *PKIPI_COUNTS;

#if defined(NT_UP)
#define HOT_STATISTIC(a) a
#else
#define HOT_STATISTIC(a) (KeGetCurrentPrcb()->a)
#endif

typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_FREE_EXTRA_PTES         0x0200
#define MDL_DESCRIBES_AWE           0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000
#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA | MDL_PAGES_LOCKED | MDL_SOURCE_IS_NONPAGED_POOL | MDL_PARTIAL_HAS_BEEN_MAPPED | MDL_PARENT_MAPPED_SYSTEM_VA | MDL_SYSTEM_VA | MDL_IO_SPACE)

#if DBG
#define PAGED_CODE() { if (KeGetCurrentIrql() > APC_LEVEL) { KdPrint(( "EX: Pageable code called at IRQL %d\n", KeGetCurrentIrql() )); ASSERT(FALSE); } }
#else
#define PAGED_CODE() NOP_FUNCTION;
#endif

#if (defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_) || defined(_NTOSP_)) && !defined(_BLDR_)
#if defined(_NTSYSTEM_)
#define NTKERNELAPI
#else
#define NTKERNELAPI DECLSPEC_IMPORT
#endif
#else
#define NTKERNELAPI
#endif

#if defined(_NTSYSTEM_) || defined(_NTHAL_) || defined(_BLDR_)
#define NTHALAPI
#else
#define NTHALAPI DECLSPEC_IMPORT
#endif

typedef struct _DISPATCHER_HEADER {
    union {
        struct {
            UCHAR Type;
            UCHAR Absolute;
            UCHAR Size;
            union {
                UCHAR Inserted;
                BOOLEAN DebugActive;
            };
        };
        volatile LONG Lock;
    };
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    LONG Period;
} KTIMER, *PKTIMER, *RESTRICTED_POINTER PRKTIMER;

typedef enum _LOCK_OPERATION {
    IoReadAccess,
    IoWriteAccess,
    IoModifyAccess
} LOCK_OPERATION;

#endif // _NTOSDEF_
