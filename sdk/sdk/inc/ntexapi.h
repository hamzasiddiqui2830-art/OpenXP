/*++ BUILD Version: 0008    // Increment this if a change has global effects

Copyright (c) OpenXP Contributors
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    ntexapi.h

Abstract:

    This module is the header file for the all the system services that
    are contained in the "ex" directory.

--*/

#ifndef _NTEXAPI_
#define _NTEXAPI_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <winternl.h>

//
// Delay thread execution.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDelayExecution (
    __in BOOLEAN Alertable,
    __in PLARGE_INTEGER DelayInterval
    );

//
// Query and set system environment variables.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemEnvironmentValue (
    __in PUNICODE_STRING VariableName,
    __out_bcount(ValueLength) PWSTR VariableValue,
    __in USHORT ValueLength,
    __out_opt PUSHORT ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemEnvironmentValue (
    __in PUNICODE_STRING VariableName,
    __in PUNICODE_STRING VariableValue
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemEnvironmentValueEx (
    __in PUNICODE_STRING VariableName,
