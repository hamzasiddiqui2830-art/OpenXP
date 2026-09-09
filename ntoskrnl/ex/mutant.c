/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    mutant.c

Abstract:

   This module implements the executive mutant object. Functions are
   provided to create, open, release, and query mutant objects.

--*/

#include "exp.h"

// The kernel build keeps the WRK debugger-dump layout calculation local.
// This value is consumed only as the byte count for the initial process
// directory-table-area clearing below.
#ifndef KdDumpEnableOffset
extern ULONG KdDumpEnableOffset;
#endif

//
// Address of mutant object type descriptor.
//

POBJECT_TYPE ExMutantObjectType;

//
// Structure that describes the mapping of generic access rights to object
// specific access rights for mutant objects.
//

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("INITCONST")
#endif

const GENERIC_MAPPING ExpMutantMapping = {
    STANDARD_RIGHTS_READ | MUTANT_QUERY_STATE,
    STANDARD_RIGHTS_WRITE,
    STANDARD_RIGHTS_EXECUTE | SYNCHRONIZE,
    MUTANT_ALL_ACCESS
};

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg()
#endif

#pragma alloc_text(INIT, ExpMutantInitialization)
#pragma alloc_text(PAGE, NtCreateMutant)
#pragma alloc_text(PAGE, NtOpenMutant)
#pragma alloc_text(PAGE, NtQueryMutant)
#pragma alloc_text(PAGE, NtReleaseMutant)

VOID
ExpDeleteMutant (
    IN PVOID Mutant
    )

/*++

Routine Description:

    This function is called when an executive mutant object is about to
    be deleted. The mutant object is released with an abandoned status to
    ensure that it is removed from the owner thread's mutant list if the
    mutant object is currently owned by a thread.

Arguments:

    Mutant - Supplies a pointer to an executive mutant object.

Return Value:

    None.

--*/

{
    //
    // Release the mutant object with an abandoned status to ensure that it
    // is removed from the owner thread's mutant list if the mutant is
    // currently owned by a thread.
    //

    KeReleaseMutant((PKMUTANT)Mutant, MUTANT_INCREMENT, TRUE, FALSE);
    return;
}

BOOLEAN
ExpMutantInitialization (
    VOID
    )

/*++

Routine Description:

    This function creates the mutant object type descriptor at system
    initialization and stores the address of the object type descriptor
    in local static storage.

Arguments:

    None.

Return Value:

    A value of TRUE is returned if the mutant object type descriptor is
    successfully created. Otherwise a value of FALSE is returned.

--*/

{

    OBJECT_TYPE_INITIALIZER ObjectTypeInitializer;
    NTSTATUS Status;
    UNICODE_STRING TypeName;

    //
    // Initialize string descriptor.
    //

    RtlInitUnicodeString(&TypeName, L"Mutant");

    //
    // Create mutant object type descriptor.
    //

    RtlZeroMemory(&ObjectTypeInitializer, sizeof(ObjectTypeInitializer));
    RtlZeroMemory(&PsGetCurrentProcess()->Pcb.DirectoryTableBase[0], KdDumpEnableOffset);
    ObjectTypeInitializer.Length = sizeof(ObjectTypeInitializer);
    ObjectTypeInitializer.InvalidAttributes = OBJ_OPENLINK;
    ObjectTypeInitializer.GenericMapping = ExpMutantMapping;
    ObjectTypeInitializer.PoolType = NonPagedPool;
    ObjectTypeInitializer.DefaultNonPagedPoolCharge = sizeof(KMUTANT);
    ObjectTypeInitializer.ValidAccessMask = MUTANT_ALL_ACCESS;
    ObjectTypeInitializer.DeleteProcedure = ExpDeleteMutant;
    Status = ObCreateObjectType(&TypeName,
                                &ObjectTypeInitializer,
                                (PSECURITY_DESCRIPTOR)NULL,
                                &ExMutantObjectType);

    //
    // If the mutant object type descriptor was successfully created, then
    // return a value of TRUE. Otherwise return a value of FALSE.
    //

    return NT_SUCCESS(Status);
}

NTSTATUS
NtCreateMutant (
    OUT PHANDLE MutantHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN BOOLEAN InitialOwner
    )
{
    return NtCreateMutantEx(MutantHandle, DesiredAccess, ObjectAttributes, InitialOwner, NULL);
}

NTSTATUS
NtOpenMutant (
    OUT PHANDLE MutantHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
    return ObOpenObjectByName(ObjectAttributes, ExMutantObjectType, NULL,
                              UserMode, DesiredAccess, NULL, MutantHandle);
}

NTSTATUS
NtQueryMutant (
    IN HANDLE MutantHandle,
    IN MUTANT_INFORMATION_CLASS MutantInformationClass,
    OUT PVOID MutantInformation,
    IN ULONG MutantInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    )
{
    return KeQueryMutant(MutantHandle, MutantInformationClass,
                         MutantInformation, MutantInformationLength,
                         ReturnLength);
}

NTSTATUS
NtReleaseMutant (
    IN HANDLE MutantHandle,
    OUT PLONG PreviousState OPTIONAL
    )
{
    return NtReleaseMutantEx(MutantHandle, PreviousState, NULL);
}
