/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.


Module Name:

    FsRtlP.h

Abstract:

    This module defines private part of the File System Rtl component

--*/

#ifndef _FSRTLP_
#define _FSRTLP_

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4706)   // assignment within conditional expression

/*
 * FsRtlP.h is used as a PCH and must establish the public NT types before
 * pulling in the kernel umbrella header.  In particular ntrtl.h consumes
 * RTL_ATOM/PRTL_ATOM and CSHORT, while the normal ntos.h include path is not
 * guaranteed to have initialized those headers when the PCH is built.
 */
#include <ntdef.h>
#include <ntexapi.h>
#include <ntos.h>
#include <FsRtl.h>
#include <NtDdFt.h>
#include <zwapi.h>

#define FsRtlAllocatePool(PoolType, NumberOfBytes )                \
    ExAllocatePoolWithTag((POOL_TYPE)((PoolType) | POOL_RAISE_IF_ALLOCATION_FAILURE), \
                          NumberOfBytes,                                      \
                          'trSF')


#define FsRtlAllocatePoolWithQuota(PoolType, NumberOfBytes )           \
    ExAllocatePoolWithQuotaTag((POOL_TYPE)((PoolType) | POOL_RAISE_IF_ALLOCATION_FAILURE), \
                               NumberOfBytes,                                 \
                               'trSF')

#define FsRtlpAllocatePool(a,b)  FsRtlAllocatePoolWithTag((a),(b),MODULE_POOL_TAG)

//
//  The global FsRtl debug level variable, its values are:
//
//      0x00000000      Always gets printed (used when about to bugcheck)
//      0x00000001      Error conditions
//      0x00000002      Debug hooks
//      0x00000004
//      0x00000008
//      0x00000010
//      0x00000020
//      0x00000040
//      0x00000080
//      0x00000100
//      0x00000200
//      0x00000400
//      0x00000800
//      0x00001000
//      0x00002000
//      0x00004000
//      0x00008000
//      0x00010000
//      0x00020000
//      0x00040000
//      0x00080000
//      0x00100000
//      0x00200000
//      0x00400000
//      0x00800000
//      0x01000000
//      0x02000000
//      0x04000000      NotifyChange routines
//      0x08000000      Oplock routines
//      0x10000000      Name routines
//      0x20000000      FileLock routines
//      0x40000000      Vmcb routines
//      0x80000000      Mcb routines
//

//
//  Debug trace support
//

#ifdef FSRTLDBG

extern LONG FsRtlDebugTraceLevel;
extern LONG FsRtlDebugTraceIndent;

#define DebugTrace(INDENT,LEVEL,X,Y) {                        \
    LONG _i;                                                  \
    if (((LEVEL) == 0) || (FsRtlDebugTraceLevel & (LEVEL))) { \
        _i = (ULONG)PsGetCurrentThread();                     \
        DbgPrint("%08lx:",_i);                               \
        if ((INDENT) < 0) {                                   \
            FsRtlDebugTraceIndent += (INDENT);                \
        }                                                     \
        if (FsRtlDebugTraceIndent < 0) {                      \
            FsRtlDebugTraceIndent = 0;                        \
        }                                                     \
        for (_i=0; _i<FsRtlDebugTraceIndent; _i+=1) {         \
            DbgPrint(" ");                                   \
        }                                                     \
        DbgPrint(X,Y);                                        \
        if ((INDENT) > 0) {                                   \
            FsRtlDebugTraceIndent += (INDENT);                \
        }                                                     \
    }                                                         \
}

#define DebugDump(STR,LEVEL,PTR) {                            \
    ULONG _i;                                                 \
    VOID FsRtlDump();                                         \
    if (((LEVEL) == 0) || (FsRtlDebugTraceLevel & (LEVEL))) { \
        _i = (ULONG)PsGetCurrentThread();                     \
        DbgPrint("%08lx:",_i);                               \
        DbgPrint(STR);                                        \
        if (PTR != NULL) {FsRtlDump(PTR);}                    \
        DbgBreakPoint();                                      \
    }                                                         \
}

#else

#define DebugTrace(INDENT,LEVEL,X,Y)     {NOTHING;}

#define DebugDump(STR,LEVEL,PTR)         {NOTHING;}

#endif // FSRTLDBG


//
//  Miscellaneous support routines
//

VOID
FsRtlInitializeFileLocks (
    VOID
    );

VOID
FsRtlInitializeLargeMcbs (
    VOID
    );

VOID
FsRtlInitializeTunnels(
    VOID
    );

NTSTATUS
FsRtlInitializeWorkerThread (
    VOID
    );

#define FlagOn(Flags,SingleFlag)        ((Flags) & (SingleFlag))
#define BooleanFlagOn(Flags,SingleFlag) ((BOOLEAN)(((Flags) & (SingleFlag)) != 0))
#define SetFlag(F,SF) { (F) |= (SF); }
#define ClearFlag(F,SF) { (F) &= ~(SF); }

#define WordAlign(Ptr) ((((ULONG_PTR)(Ptr)) + 1) & -2)
#define LongAlign(Ptr) ((((ULONG_PTR)(Ptr)) + 3) & -4)
#define QuadAlign(Ptr) ((((ULONG_PTR)(Ptr)) + 7) & -8)
#define SectorAlign(Ptr) ((((ULONG_PTR)(Ptr)) + 511) & -512)
#define SectorsFromBytes(bytes) (((bytes) + 511) / 512)
#define BytesFromSectors(sectors) ((sectors) * 512)

typedef union _UCHAR1 {
    UCHAR  Uchar[1];
    UCHAR  ForceAlignment;
} UCHAR1, *PUCHAR1;

typedef union _UCHAR2 {
    UCHAR  Uchar[2];
    USHORT ForceAlignment;
} UCHAR2, *PUCHAR2;

typedef union _UCHAR4 {
    UCHAR  Uchar[4];
    ULONG  ForceAlignment;
} UCHAR4, *PUCHAR4;

#define CopyUchar1(Dst,Src) { *((UCHAR1 *)(Dst)) = *((UNALIGNED UCHAR1 *)(Src)); }
#define CopyUchar2(Dst,Src) { *((UCHAR2 *)(Dst)) = *((UNALIGNED UCHAR2 *)(Src)); }
#define CopyUchar4(Dst,Src) { *((UCHAR4 *)(Dst)) = *((UNALIGNED UCHAR4 *)(Src)); }

#include "../inc/seh.h"

#define try_return(S) { S; goto try_exit; }

#define GET_FAST_IO_DISPATCH(DevObj) \
    ((DevObj)->DriverObject->FastIoDispatch)

#define GET_FS_FILTER_CALLBACKS(DevObj) \
    ((DevObj)->DriverObject->DriverExtension->FsFilterCallbacks)

#define VALID_FAST_IO_DISPATCH_HANDLER(FastIoDispatchPtr, FieldName) \
    (((FastIoDispatchPtr) != NULL) && \
     (((FastIoDispatchPtr)->SizeOfFastIoDispatch) >= \
      (FIELD_OFFSET(FAST_IO_DISPATCH, FieldName) + sizeof(VOID *))) && \
     ((FastIoDispatchPtr)->FieldName != NULL))

#define VALID_FS_FILTER_CALLBACK_HANDLER(FsFilterCallbackPtr, FieldName) \
    (((FsFilterCallbackPtr) != NULL) && \
     (((FsFilterCallbackPtr)->SizeOfFsFilterCallbacks) >= \
      (FIELD_OFFSET(FS_FILTER_CALLBACKS, FieldName) + sizeof(VOID *))) && \
     ((FsFilterCallbackPtr)->FieldName != NULL))

#define FSRTL_FILTER_MEMORY_TAG    'gmSF'

typedef struct _FS_FILTER_COMPLETION_NODE {
    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;
    PVOID CompletionContext;
    PFS_FILTER_COMPLETION_CALLBACK CompletionCallback;
} FS_FILTER_COMPLETION_NODE, *PFS_FILTER_COMPLETION_NODE;

#define FS_FILTER_DEFAULT_STACK_SIZE    15

typedef struct _FS_FILTER_COMPLETION_STACK {
    USHORT StackLength;
    USHORT NextStackPosition;
    PFS_FILTER_COMPLETION_NODE Stack;
    FS_FILTER_COMPLETION_NODE DefaultStack[FS_FILTER_DEFAULT_STACK_SIZE];
} FS_FILTER_COMPLETION_STACK, *PFS_FILTER_COMPLETION_STACK;

typedef struct _FS_FILTER_CTRL {
    FS_FILTER_CALLBACK_DATA Data;
    ULONG Flags;
    ULONG Reserved;
    FS_FILTER_COMPLETION_STACK CompletionStack;
} FS_FILTER_CTRL, *PFS_FILTER_CTRL;

#define FS_FILTER_ALLOCATED_COMPLETION_STACK    0x00000001
#define FS_FILTER_USED_RESERVE_POOL             0x00000002
#define FS_FILTER_CHANGED_DEVICE_STACKS         0x00000004

NTSTATUS
FsFilterInit(
    );

NTSTATUS
FsFilterAllocateCompletionStack (
    IN PFS_FILTER_CTRL FsFilterCtrl,
    IN BOOLEAN CanFail,
    OUT PULONG AllocationSize
    );

VOID
FsFilterFreeCompletionStack (
    IN PFS_FILTER_CTRL FsFilterCtrl
    );

NTSTATUS
FsFilterCtrlInit (
    IN OUT PFS_FILTER_CTRL FsFilterCtrl,
    IN UCHAR Operation,
    IN PDEVICE_OBJECT DeviceObject,
    IN PDEVICE_OBJECT BaseFsDeviceObject,
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN CanFail
    );

VOID
FsFilterCtrlFree (
    IN PFS_FILTER_CTRL FsFilterCtrl
    );

#define PUSH_COMPLETION_NODE(completionStack) \
    (((completionStack)->NextStackPosition < (completionStack)->StackLength) ? \
        &(completionStack)->Stack[(completionStack)->NextStackPosition++] : \
        (ASSERT(FALSE), NULL))

#define POP_COMPLETION_NODE(completionStack) \
    (ASSERT((completionStack)->NextStackPosition > 0), \
     ((completionStack)->NextStackPosition--))

#define GET_COMPLETION_NODE(completionStack) \
    (ASSERT((completionStack)->NextStackPosition > 0), \
     (&(completionStack)->Stack[(completionStack)->NextStackPosition-1]))

#define FS_FILTER_HAVE_COMPLETIONS(fsFilterCtrl) \
    ((fsFilterCtrl)->CompletionStack.NextStackPosition > 0)

VOID
FsFilterGetCallbacks (
    IN UCHAR Operation,
    IN PDEVICE_OBJECT DeviceObject,
    OUT PFS_FILTER_CALLBACK *PreOperationCallback,
    OUT PFS_FILTER_COMPLETION_CALLBACK *PostOperationCallback
    );

NTSTATUS
FsFilterPerformCallbacks (
    IN PFS_FILTER_CTRL FsFilterCtrl,
    IN BOOLEAN AllowFilterToFail,
    IN BOOLEAN AllowBaseFsToFail,
    OUT BOOLEAN *BaseFsFailedOp
    );

VOID
FsFilterPerformCompletionCallbacks(
    IN PFS_FILTER_CTRL FsFilterCtrl,
    IN NTSTATUS OperationStatus
    );

#endif // _FSRTLP_
