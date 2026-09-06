/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfdevobj.h

Abstract:

    This header exposes function hooks that verify drivers properly manage
    device objects.

--*/

typedef enum {

    VF_DEVOBJ_PDO = 0,
    VF_DEVOBJ_BUS_FILTER,
    VF_DEVOBJ_LOWER_DEVICE_FILTER,
    VF_DEVOBJ_LOWER_CLASS_FILTER,
    VF_DEVOBJ_FDO,
    VF_DEVOBJ_UPPER_DEVICE_FILTER,
    VF_DEVOBJ_UPPER_CLASS_FILTER

} VF_DEVOBJ_TYPE, *PVF_DEVOBJ_TYPE;

VOID
VfDevObjPreAddDevice(
    IN  PDEVICE_OBJECT      PhysicalDeviceObject,
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDRIVER_ADD_DEVICE  AddDeviceFunction,
    IN  VF_DEVOBJ_TYPE      DevObjType
    );

VOID
VfDevObjPostAddDevice(
    IN  PDEVICE_OBJECT      PhysicalDeviceObject,
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDRIVER_ADD_DEVICE  AddDeviceFunction,
    IN  VF_DEVOBJ_TYPE      DevObjType,
    IN  NTSTATUS            Result
    );

VOID
VfDevObjAdjustFdoForVerifierFilters(
    IN OUT  PDEVICE_OBJECT *FunctionalDeviceObject
    );

VOID
VerifierIoAttachDeviceToDeviceStack(
    IN PDEVICE_OBJECT NewDevice,
    IN PDEVICE_OBJECT ExistingDevice
    );

VOID
VerifierIoDetachDevice(
    IN PDEVICE_OBJECT LowerDevice
    );

VOID
VerifierIoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

