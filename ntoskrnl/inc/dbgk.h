/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    dbgk.h

Abstract:

    This header file describes public data structures and functions
    that make up the kernel mode portion of the Dbg subsystem.

--*/

#ifndef _DBGK_
#define _DBGK_

//
// Define the debug object thats used to attatch to processes that are being debugged.
//
#define DEBUG_OBJECT_DELETE_PENDING (0x1) // Debug object is delete pending.
#define DEBUG_OBJECT_KILL_ON_CLOSE  (0x2) // Kill all debugged processes on close

typedef struct _DEBUG_OBJECT {
    //
    // Event thats set when the EventList is populated.
    //
    KEVENT EventsPresent;
    //
    // Mutex to protect the structure
    //
    FAST_MUTEX Mutex;
    //
    // Queue of events waiting for debugger intervention
    //
    LIST_ENTRY EventList;
    //
    // Flags for the object
    //
    ULONG Flags;
} DEBUG_OBJECT, *PDEBUG_OBJECT;

VOID
DbgkCreateThread(
    PETHREAD Thread,
    PVOID StartAddress
    );

VOID
DbgkExitThread(
    NTSTATUS ExitStatus
    );

VOID
DbgkExitProcess(
    NTSTATUS ExitStatus
    );

VOID
DbgkMapViewOfSection(
    IN HANDLE SectionHandle,
    IN PVOID BaseAddress,
    IN ULONG SectionOffset,
    IN ULONG_PTR ViewSize
    );

VOID
DbgkUnMapViewOfSection(
    IN PVOID BaseAddress
    );

BOOLEAN
DbgkForwardException (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN BOOLEAN DebugException,
    IN BOOLEAN SecondChance
    );

NTSTATUS
DbgkInitialize (
    VOID
    );

VOID
DbgkCopyProcessDebugPort (
    IN PEPROCESS TargetProcess,
    IN PEPROCESS SourceProcess
    );

NTSTATUS
DbgkOpenProcessDebugPort (
    IN PEPROCESS TargetProcess,
    IN KPROCESSOR_MODE PreviousMode,
    OUT HANDLE *pHandle
    );

NTSTATUS
DbgkClearProcessDebugObject (
    IN PEPROCESS Process,
    IN PDEBUG_OBJECT SourceDebugObject
    );


extern POBJECT_TYPE DbgkDebugObjectType;


#endif // _DBGK_
