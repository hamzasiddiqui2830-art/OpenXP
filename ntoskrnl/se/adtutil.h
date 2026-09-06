/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    adtutil.h - Security Auditing - Utility Routines

Abstract:

    This Module contains miscellaneous utility routines private to the
    Security Auditing Component.

--*/

NTSTATUS
SepRegQueryDwordValue(
    IN  PCWSTR KeyName,
    IN  PCWSTR ValueName,
    OUT PULONG Value
    );

NTSTATUS
SepRegQueryHelper(
    IN  PCWSTR KeyName,
    IN  PCWSTR ValueName,
    IN  ULONG  ValueType,
    IN  ULONG  ValueLength,
    OUT PVOID  ValueBuffer,
    OUT PULONG LengthRequired
    );

