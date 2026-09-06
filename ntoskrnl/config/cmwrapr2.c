/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    cmwrapr2.c

Abstract:

    This module contains the source for wrapper routines called by the
    hive code, which in turn call the appropriate NT routines.  But not
    callable from user mode.

--*/

#include    "cmp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,CmpFileSetSize)
#endif

extern  KEVENT StartRegistryCommand;
extern  KEVENT EndRegistryCommand;

//
// Write-Control:
//  CmpNoWrite is initially true.  When set this way write and flush
//  do nothing, simply returning success.  When cleared to FALSE, I/O
//  is enabled.  This change is made after the I/O system is started
//  AND autocheck (chkdsk) has done its thing.
//

extern  BOOLEAN CmpNoWrite;


BOOLEAN
CmpFileSetSize(
    PHHIVE      Hive,
    ULONG       FileType,
    ULONG       FileSize,
    ULONG       OldFileSize
    )
/*++

Routine Description:

    This routine sets the size of a file.  It must not return until
    the size is guaranteed, therefore, it does a flush.

    It is environment specific.

    This routine will force execution to the correct thread context.

Arguments:

    Hive - Hive we are doing I/O for

    FileType - which supporting file to use

    FileSize - 32 bit value to set the file's size to

Return Value:

    FALSE if failure
    TRUE if success

--*/
{
    NTSTATUS    status;

    ASSERT(FIELD_OFFSET(CMHIVE, Hive) == 0);

    ASSERT_HIVE_WRITER_LOCK_OWNED((PCMHIVE)Hive);

    //
    // Call the worker to do real work for us.
    //
    status = CmpDoFileSetSize(Hive,FileType,FileSize,OldFileSize);
    
    if (!NT_SUCCESS(status)) {
        CmKdPrintEx((DPFLTR_CONFIG_ID,CML_BUGCHECK,"CmpFileSetSize:\n\t"));
        CmKdPrintEx((DPFLTR_CONFIG_ID,CML_BUGCHECK,"Failure: status = %08lx ", status));
        return FALSE;
    }

    return TRUE;
}
