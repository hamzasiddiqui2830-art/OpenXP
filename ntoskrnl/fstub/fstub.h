/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    fstub.h

Abstract:

    Fstub private header file.

--*/

#pragma once

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4706)   // assignment within conditional expression


typedef struct _INTERNAL_DISK_GEOMETRY {
    DISK_GEOMETRY Geometry;
    LARGE_INTEGER DiskSize;
} INTERNAL_DISK_GEOMETRY, *PINTERNAL_DISK_GEOMETRY;

//
// Verify that the INTERNAL_DISK_GEOMETRY structure matches the DISK_GEOMETRY
// structure.
//

C_ASSERT (FIELD_OFFSET (DISK_GEOMETRY_EX, Geometry) ==
            FIELD_OFFSET (INTERNAL_DISK_GEOMETRY, Geometry) &&
          FIELD_OFFSET (DISK_GEOMETRY_EX, DiskSize) ==
            FIELD_OFFSET (INTERNAL_DISK_GEOMETRY, DiskSize));

//
// Debugging macros and flags
//

#define FSTUB_VERBOSE_LEVEL 4

#if DBG

VOID
FstubDbgPrintPartition(
    IN PPARTITION_INFORMATION Partition,
    IN ULONG PartitionCount
    );

VOID
FstubDbgPrintDriveLayout(
    IN PDRIVE_LAYOUT_INFORMATION  Layout
    );

VOID
FstubDbgPrintPartitionEx(
    IN PPARTITION_INFORMATION_EX PartitionEx,
    IN ULONG PartitionCount
    );

VOID
FstubDbgPrintDriveLayoutEx(
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutEx
    );

VOID
FstubDbgPrintSetPartitionEx(
    IN PSET_PARTITION_INFORMATION_EX SetPartition,
    IN ULONG PartitionNumber
    );

#else

#define FstubDbgPrintPartition(Partition, PartitionCount)
#define FstubDbgPrintDriveLayout(Layout)
#define FstubDbgPrintPartitionEx(PartitionEx, PartitionCount)
#define FstubDbgPrintDriveLayoutEx(LayoutEx)
#define FstubDbgPrintSetPartitionEx(SetPartition, PartitionNumber)

#endif // !DBG

