/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    open.c

Abstract:

    This module contains the code to implement the NtOpenFile system
    service.

--*/

#include "iomgr.h"

#pragma alloc_text(PAGE, NtOpenFile)

NTSTATUS
NtOpenFile(
    __out PHANDLE FileHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __out PIO_STATUS_BLOCK IoStatusBlock,
    __in ULONG ShareAccess,
    __in ULONG OpenOptions
    )

/*++

Routine Description:

    This service opens a file or a device.  It is used to establish a file
    handle to the open device/file that can then be used in subsequent
    operations to perform I/O operations on.

Arguments:

    FileHandle - A pointer to a variable to receive the handle to the open file.

    DesiredAccess - Supplies the types of access that the caller would like to
        the file.

    ObjectAttributes - Supplies the attributes to be used for file object (name,
        SECURITY_DESCRIPTOR, etc.)

    IoStatusBlock - Specifies the address of the caller's I/O status block.

    ShareAccess - Supplies the types of share access that the caller would like
        to the file.

    OpenOptions - Caller options for how to perform the open.

Return Value:

    The function value is the final completion status of the open/create
    operation.

--*/

{
    //
    // Simply invoke the common I/O file creation routine to perform the work.
    //

    PAGED_CODE();

    return IoCreateFile( FileHandle,
                         DesiredAccess,
                         ObjectAttributes,
                         IoStatusBlock,
                         (PLARGE_INTEGER) NULL,
                         0L,
                         ShareAccess,
                         FILE_OPEN,
                         OpenOptions,
                         (PVOID) NULL,
                         0L,
                         CreateFileTypeNone,
                         (PVOID) NULL,
                         0 );
}

