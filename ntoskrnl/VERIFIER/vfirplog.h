/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfirplog.h

Abstract:

    This header exposes functions for logging IRP events.

--*/

//
// Log-snapshots are retrievable by user mode for profiling and targetted
// probing of stacks. Content-wise they are heavier.
//
typedef struct _IRPLOG_SNAPSHOT {

    ULONG       Count;
    UCHAR       MajorFunction;
    UCHAR       MinorFunction;
    UCHAR       Flags;
    UCHAR       Control;
    ULONGLONG   ArgArray[4];

} IRPLOG_SNAPSHOT, *PIRPLOG_SNAPSHOT;

VOID
VfIrpLogInit(
    VOID
    );

VOID
VfIrpLogRecordEvent(
    IN  PVERIFIER_SETTINGS_SNAPSHOT VerifierSettingsSnapshot,
    IN  PDEVICE_OBJECT              DeviceObject,
    IN  PIRP                        Irp
    );

ULONG
VfIrpLogGetIrpDatabaseSiloCount(
    VOID
    );

NTSTATUS
VfIrpLogLockDatabase(
    IN  ULONG   SiloNumber
    );

NTSTATUS
VfIrpLogRetrieveWmiData(
    IN  ULONG   SiloNumber,
    OUT PUCHAR  OutputBuffer                OPTIONAL,
    OUT ULONG  *OffsetInstanceNameOffsets,
    OUT ULONG  *InstanceCount,
    OUT ULONG  *DataBlockOffset,
    OUT ULONG  *TotalRequiredSize
    );

VOID
VfIrpLogUnlockDatabase(
    IN  ULONG   SiloNumber
    );

VOID
VfIrpLogDeleteDeviceLogs(
    IN PDEVICE_OBJECT DeviceObject
    );

