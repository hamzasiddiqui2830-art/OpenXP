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

    PDEVICE_OBJECT                  DeviceObject;
    PEPROCESS                       Process;
    PHANDLE_ENUMERATION_CALLBACK    CallBack;
    PVOID                           Context;

} HANDLE_ENUM_CONTEXT, *PHANDLE_ENUM_CONTEXT;

LOGICAL
PiHandleEnumerateHandlesAgainstDeviceObject(
    IN  PDEVICE_OBJECT                  DeviceObject,
    IN  PHANDLE_ENUMERATION_CALLBACK    HandleEnumCallBack,
    IN  PVOID                           Context
    );

BOOLEAN
PiHandleProcessWalkWorker(
    IN  PHANDLE_TABLE_ENTRY     ObjectTableEntry,
    IN  HANDLE                  HandleId,
    IN  PHANDLE_ENUM_CONTEXT    EnumContext
    );

//
// This macro uses private information from the ntos\ex module. It should be
// replaced with an inter-module define or function
//
#define OBJECT_FROM_EX_TABLE_ENTRY(x) \
    (POBJECT_HEADER)((ULONG_PTR)(x)->Object & ~7)


