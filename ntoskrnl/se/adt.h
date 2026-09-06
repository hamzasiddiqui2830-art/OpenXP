/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    adt.h

Abstract:

    Auditing - Defines, Function Prototypes and Macro Functions.
               These are public to the Security Component only.

--*/

#include <ntlsa.h>


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Auditing Routines visible to rest of Security Component outside Auditing //
// subcomponent.                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


BOOLEAN
SepAdtInitializePhase0();

BOOLEAN
SepAdtInitializePhase1();

VOID
SepAdtLogAuditRecord(
    IN PSE_ADT_PARAMETER_ARRAY AuditParameters
    );

NTSTATUS
SepAdtCopyToLsaSharedMemory(
    IN HANDLE LsaProcessHandle,
    IN PVOID Buffer,
    IN ULONG BufferLength,
    OUT PVOID *LsaBufferAddress
    );

