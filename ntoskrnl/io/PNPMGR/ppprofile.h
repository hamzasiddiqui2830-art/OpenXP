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

typedef enum _HARDWARE_PROFILE_BUS_TYPE {

    HardwareProfileBusTypeACPI

} HARDWARE_PROFILE_BUS_TYPE, *PHARDWARE_PROFILE_BUS_TYPE;

VOID
PpProfileInit(
    VOID
    );

VOID
PpProfileBeginHardwareProfileTransition(
    IN BOOLEAN SubsumeExistingDeparture
    );

VOID
PpProfileIncludeInHardwareProfileTransition(
    IN  PDEVICE_NODE    DeviceNode,
    IN  PROFILE_STATUS  ChangeInPresence
    );

NTSTATUS
PpProfileQueryHardwareProfileChange(
    IN  BOOLEAN                     SubsumeExistingDeparture,
    IN  PROFILE_NOTIFICATION_TIME   NotificationTime,
    OUT PPNP_VETO_TYPE              VetoType,
    OUT PUNICODE_STRING             VetoName OPTIONAL
    );

VOID
PpProfileCommitTransitioningDock(
    IN PDEVICE_NODE     DeviceNode,
    IN PROFILE_STATUS   ChangeInPresence
    );

VOID
PpProfileCancelTransitioningDock(
    IN PDEVICE_NODE     DeviceNode,
    IN PROFILE_STATUS   ChangeInPresence
    );

VOID
PpProfileCancelHardwareProfileTransition(
    VOID
    );

VOID
PpProfileMarkAllTransitioningDocksEjected(
    VOID
    );

PDEVICE_OBJECT
PpProfileRetrievePreferredDockToEject(
    VOID
    );

VOID
PpProfileProcessDockDeviceCapability(
    IN PDEVICE_NODE DeviceNode,
    IN PDEVICE_CAPABILITIES Capabilities
    );
