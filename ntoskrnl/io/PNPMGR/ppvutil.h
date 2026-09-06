/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    ppvutil.h

Abstract:

    This header exposes various utilities required to do driver verification.

--*/

#ifndef _PPVUTIL_H_
#define _PPVUTIL_H_

typedef enum {

    PPVERROR_DUPLICATE_PDO_ENUMERATED           = 0,
    PPVERROR_MISHANDLED_TARGET_DEVICE_RELATIONS,
    PPVERROR_DDI_REQUIRES_PDO

} PPVFAILURE_TYPE;

typedef enum {

    PPVREMOVAL_SHOULD_DELETE           = 0,
    PPVREMOVAL_SHOULDNT_DELETE,
    PPVREMOVAL_MAY_DEFER_DELETION

} PPVREMOVAL_OPTION;

VOID
FASTCALL
PpvUtilInit(
    VOID
    );

NTSTATUS
FASTCALL
PpvUtilCallAddDevice(
    IN  PDEVICE_OBJECT      PhysicalDeviceObject,
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDRIVER_ADD_DEVICE  AddDeviceFunction,
    IN  VF_DEVOBJ_TYPE      DevObjType
    );

VOID
FASTCALL
PpvUtilTestStartedPdoStack(
    IN  PDEVICE_OBJECT  DeviceObject
    );

PPVREMOVAL_OPTION
FASTCALL
PpvUtilGetDevnodeRemovalOption(
    IN  PDEVICE_OBJECT  PhysicalDeviceObject
    );

VOID
FASTCALL
PpvUtilFailDriver(
    IN  PPVFAILURE_TYPE FailureType,
    IN  PVOID           CulpritAddress,
    IN  PDEVICE_OBJECT  DeviceObject    OPTIONAL,
    IN  PVOID           ExtraneousInfo  OPTIONAL
    );

BOOLEAN
FASTCALL
PpvUtilIsHardwareBeingVerified(
    IN  PDEVICE_OBJECT  PhysicalDeviceObject
    );

#endif // _PPVUTIL_H_

