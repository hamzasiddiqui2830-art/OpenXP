/*
 * ReactOS Kernel
 * Copyright (C) 2024 ReactOS Team
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

typedef struct {

    ULONG           Depth;
    PDEVICE_OBJECT  PhysicalDeviceObject;

} BEST_DOCK_TO_EJECT, *PBEST_DOCK_TO_EJECT;

VOID
PiProfileSendHardwareProfileCommit(
    VOID
    );

VOID
PiProfileSendHardwareProfileCancel(
    VOID
    );

NTSTATUS
PiProfileUpdateHardwareProfile(
    OUT BOOLEAN     *ProfileChanged
    );

NTSTATUS
PiProfileRetrievePreferredCallback(
    IN PDEVICE_NODE         DeviceNode,
    IN PVOID                Context
    );

PDEVICE_NODE
PiProfileConvertFakeDockToRealDock(
    IN  PDEVICE_NODE    FakeDockDevnode
    );

NTSTATUS
PiProfileUpdateDeviceTree(
    VOID
    );

VOID
PiProfileUpdateDeviceTreeWorker(
    IN PVOID Context
    );

NTSTATUS
PiProfileUpdateDeviceTreeCallback(
    IN PDEVICE_NODE DeviceNode,
    IN PVOID Context
    );

//
// Functions not yet ported from dockhwp.c
//

NTSTATUS
IopExecuteHardwareProfileChange(
    IN  HARDWARE_PROFILE_BUS_TYPE   Bus,
    IN  PWCHAR                    * ProfileSerialNumbers,
    IN  ULONG                       SerialNumbersCount,
    OUT PHANDLE                     NewProfile,
    OUT PBOOLEAN                    ProfileChanged
    );

NTSTATUS
IopExecuteHwpDefaultSelect (
    IN  PCM_HARDWARE_PROFILE_LIST ProfileList,
    OUT PULONG ProfileIndexToUse,
    IN  PVOID Context
    );


