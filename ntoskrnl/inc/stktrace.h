/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    stktrace.h

Abstract:

    This header file defines the format of the stack trace data base
    used to track caller backtraces.  This is a header file so debugger
    extensions can lookup entries in the database remotely.

--*/

#ifndef _STKTRACE_H_
#define _STKTRACE_H_

//
// RTL_STACK_TRACE_ENTRY
//    

typedef struct _RTL_STACK_TRACE_ENTRY {

    struct _RTL_STACK_TRACE_ENTRY * HashChain;

    ULONG TraceCount;
    USHORT Index;
    USHORT Depth;
    
    PVOID BackTrace [MAX_STACK_DEPTH];

} RTL_STACK_TRACE_ENTRY, *PRTL_STACK_TRACE_ENTRY;

//
// RTL_STACK_TRACE_DATABASE
//    

typedef struct _STACK_TRACE_DATABASE {
    
    union {
        RTL_CRITICAL_SECTION CriticalSection;
        ERESOURCE Resource;
        PVOID Lock; // real lock (the other two kept for compatibility)
    };

    PVOID Reserved[3]; // fields no longer used but kept for compatibility
    
    BOOLEAN PreCommitted;
    BOOLEAN DumpInProgress; 

    PVOID CommitBase;
    PVOID CurrentLowerCommitLimit;
    PVOID CurrentUpperCommitLimit;

    PCHAR NextFreeLowerMemory;
    PCHAR NextFreeUpperMemory;

    ULONG NumberOfEntriesLookedUp;
    ULONG NumberOfEntriesAdded;

    PRTL_STACK_TRACE_ENTRY *EntryIndexArray;    // Indexed by [-1 .. -NumberOfEntriesAdded]

    ULONG NumberOfBuckets;
    PRTL_STACK_TRACE_ENTRY Buckets [1];

} STACK_TRACE_DATABASE, *PSTACK_TRACE_DATABASE;

PSTACK_TRACE_DATABASE
RtlpAcquireStackTraceDataBase ( 
    VOID 
    );

VOID
RtlpReleaseStackTraceDataBase ( 
    VOID 
    );

NTSTATUS
RtlInitializeStackTraceDataBase(
    IN PVOID CommitBase,
    IN SIZE_T CommitSize,
    IN SIZE_T ReserveSize
    );

#endif // _STKTRACE_H_
