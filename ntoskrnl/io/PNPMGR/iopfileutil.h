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

    LIST_ENTRY Link;
    UNICODE_STRING Directory;
    WCHAR Name[1];

} DIRWALK_ENTRY, *PDIRWALK_ENTRY;

NTSTATUS
IopFileUtilWalkDirectoryTreeHelper(
    IN      PUNICODE_STRING  Directory,
    IN      ULONG            Flags,
    IN      DIRWALK_CALLBACK CallbackFunction,
    IN      PVOID            Context,
    IN      PUCHAR           Buffer,
    IN      ULONG            BufferSize,
    IN OUT  PLIST_ENTRY      DirList
    );

