/*++

Copyright (c) Microsoft Corporation. All rights reserved.
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    atom.h

Abstract:

    This is the header file that describes the constants and data
    structures used by the atom manager, exported by ntdll.dll and
    ntrtl.lib

    Procedure prototypes are defined in ntrtl.h

--*/

typedef struct _RTL_ATOM_TABLE_ENTRY {
    struct _RTL_ATOM_TABLE_ENTRY *HashLink;
    USHORT HandleIndex;
    RTL_ATOM Atom;
    USHORT ReferenceCount;
    UCHAR Flags;
    UCHAR NameLength;
    WCHAR Name[ 1 ];
} RTL_ATOM_TABLE_ENTRY, *PRTL_ATOM_TABLE_ENTRY;

typedef struct _RTL_ATOM_TABLE {
    ULONG Signature;

#if defined(NTOS_KERNEL_RUNTIME)
    EX_PUSH_LOCK PushLock;
    PHANDLE_TABLE ExHandleTable;
#else
    RTL_CRITICAL_SECTION CriticalSection;
    RTL_HANDLE_TABLE RtlHandleTable;
#endif
    ULONG NumberOfBuckets;
    PRTL_ATOM_TABLE_ENTRY Buckets[ 1 ];
} RTL_ATOM_TABLE, *PRTL_ATOM_TABLE;

#define RTL_ATOM_TABLE_SIGNATURE (ULONG)'motA'

typedef struct _RTL_ATOM_HANDLE_TABLE_ENTRY {
    USHORT Flags;
    USHORT LockCount;
    PRTL_ATOM_TABLE_ENTRY Atom;
} RTL_ATOM_HANDLE_TABLE_ENTRY, *PRTL_ATOM_HANDLE_TABLE_ENTRY;

