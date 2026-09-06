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

typedef LOGICAL (*PHANDLE_ENUMERATION_CALLBACK)(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PEPROCESS       Process,
    IN  PFILE_OBJECT    FileObject,
    IN  HANDLE          HandleId,
    IN  PVOID           Context
    );

LOGICAL
PpHandleEnumerateHandlesAgainstPdoStack(
    IN  PDEVICE_OBJECT                  PhysicalDeviceObject,
    IN  PHANDLE_ENUMERATION_CALLBACK    HandleEnumCallBack,
    IN  PVOID                           Context
    );

