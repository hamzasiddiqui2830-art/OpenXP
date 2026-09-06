/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    logging.c

Abstract:

    This module contains routines for trace logging.

--*/

#include "perfp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGEWMI, PerfInfoReserveBytes)
#pragma alloc_text(PAGEWMI, PerfInfoLogBytes)
#endif //ALLOC_PRAGMA


NTSTATUS
PerfInfoReserveBytes(
    PPERFINFO_HOOK_HANDLE Hook,
    USHORT HookId,
    ULONG BytesToReserve
    )
/*++

Routine Description:

    Reserves memory for the hook via WMI and initializes the header.

Arguments:

    Hook - pointer to hook handle (used for reference decrement)
    HookId - Id for the hook
    BytesToLog - size of data in bytes

Return Value:

    STATUS_SUCCESS on success
    STATUS_UNSUCCESSFUL if the buffer memory couldn't be allocated.
--*/
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    PPERFINFO_TRACE_HEADER PPerfTraceHeader = NULL;
    PWMI_BUFFER_HEADER PWmiBufferHeader = NULL;

    PERF_ASSERT((BytesToReserve + FIELD_OFFSET(PERFINFO_TRACE_HEADER, Data)) <= MAXUSHORT);
    PERF_ASSERT(Hook != NULL);

    PPerfTraceHeader = WmiReserveWithPerfHeader(BytesToReserve, &PWmiBufferHeader);

    if (PPerfTraceHeader != NULL) {
        PPerfTraceHeader->Packet.HookId = HookId;
        Hook->PerfTraceHeader = PPerfTraceHeader;
        Hook->WmiBufferHeader = PWmiBufferHeader;

        Status = STATUS_SUCCESS;
    } else {
        *Hook = PERF_NULL_HOOK_HANDLE;
    }

    return Status;
}


NTSTATUS
PerfInfoLogBytes(
    USHORT HookId,
    PVOID Data,
    ULONG BytesToLog
    )
/*++

Routine Description:

    Reserves memory for the hook, copies the data, and unref's the hook entry.

Arguments:

    HookId - Id for the hook
    Data - pointer to the data to be logged
    BytesToLog - size of data in bytes

Return Value:

    STATUS_SUCCESS on success
--*/
{
    PERFINFO_HOOK_HANDLE Hook;
    NTSTATUS Status;

    Status = PerfInfoReserveBytes(&Hook, HookId, BytesToLog);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    RtlCopyMemory(PERFINFO_HOOK_HANDLE_TO_DATA(Hook, PPERF_BYTE), Data, BytesToLog);
    PERF_FINISH_HOOK(Hook);

    return STATUS_SUCCESS;
}

