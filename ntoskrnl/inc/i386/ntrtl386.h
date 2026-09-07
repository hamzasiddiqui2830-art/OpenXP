//
// Copyright (c) OpenXP Contributors
// Copyright (c) OpenXP Team 2026.
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//
// NTOSKRNL.H - Private kernel-mode header for i386 architecture
//

#ifndef _NTRTL386_H_
#define _NTRTL386_H_

#ifdef _M_IX86

//
// i386-specific RTL definitions and inline functions
//

//
// Interlocked operations - use intrinsics
//

#define RtlInterlockedIncrement _InterlockedIncrement
#define RtlInterlockedDecrement _InterlockedDecrement
#define RtlInterlockedExchange _InterlockedExchange
#define RtlInterlockedExchangeAdd _InterlockedExchangeAdd
#define RtlInterlockedCompareExchange _InterlockedCompareExchange

//
// Bit scan operations
//

NTSYSAPI
UCHAR
FASTCALL
RtlFindLeastSignificantBit(
    __in ULONG_PTR Set
    );

NTSYSAPI
UCHAR
FASTCALL
RtlFindMostSignificantBit(
    __in ULONG_PTR Set
    );

//
// Memory barrier
//

#define RtlMemoryBarrier() _mm_mfence()

//
// Processor control register access
//

NTSYSAPI
ULONG
__inline
RtlGetSegmentLimit(
    VOID
    )
{
    return 0xFFFFFFFF;
}

//
// Stack walking support
//

typedef struct _RTL_FRAME {
    struct _RTL_FRAME *NextStackFrame;
    PVOID ReturnAddress;
} RTL_FRAME, *PRTL_FRAME;

//
// Exception handling frame
//

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

//
// Get current TEB/PEB
//

NTSYSAPI
PVOID
__inline
RtlGetCurrentTeb(
    VOID
    )
{
    PVOID Teb;
    __asm mov eax, fs:[0x18]
    __asm mov Teb, eax
    return Teb;
}

//
// Atomic list operations
//

FORCEINLINE
PLIST_ENTRY
RtlInterlockedPopListEntry(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY ListTail
    )
{
    PLIST_ENTRY OldFirstEntry;
    PLIST_ENTRY NextEntry;
    
    OldFirstEntry = ListHead->Flink;
    NextEntry = OldFirstEntry->Flink;
    
    if (ListHead == ListTail) {
        return NULL;
    }
    
    ListHead->Flink = NextEntry;
    if (NextEntry == ListHead) {
        ListTail = ListHead;
    } else {
        NextEntry->Blink = ListHead;
    }
    
    OldFirstEntry->Flink = NULL;
    OldFirstEntry->Blink = NULL;
    
    return OldFirstEntry;
}

FORCEINLINE
VOID
RtlInterlockedPushListEntry(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY ListTail,
    __inout PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY OldLastEntry;
    
    Entry->Flink = ListHead;
    Entry->Blink = ListTail;
    
    OldLastEntry = ListTail;
    ListTail = Entry;
    
    if (OldLastEntry != NULL) {
        OldLastEntry->Flink = Entry;
    } else {
        ListHead->Blink = Entry;
    }
}

#endif // _M_IX86

#endif // _NTRTL386_H_
