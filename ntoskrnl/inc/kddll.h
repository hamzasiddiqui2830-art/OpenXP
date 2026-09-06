/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    
Module Name:

    kddll.h

Abstract:
    
    Kernel Debugger HW Extension DLL definitions

--*/

#ifndef __KDDLL_H__
#define __KDDLL_H__


//
// This Kernel Debugger Context structure is used to share
// information between the Kernel Debugger and the Kernel
// Debugger HW extension DLL
//
typedef struct _KD_CONTEXT {
    ULONG KdpDefaultRetries;
    BOOLEAN KdpControlCPending;
} KD_CONTEXT, *PKD_CONTEXT;

//
// Kernel Debugger HW Extension DLL exported functions
//
NTSTATUS
KdD0Transition(
    VOID
    );

NTSTATUS
KdD3Transition(
    VOID
    );

NTSTATUS
KdDebuggerInitialize0(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
KdDebuggerInitialize1(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock
    );

ULONG
KdReceivePacket(
    IN ULONG PacketType,
    OUT PSTRING MessageHeader,
    OUT PSTRING MessageData,
    OUT PULONG DataLength,
    IN OUT PKD_CONTEXT KdContext
    );

NTSTATUS
KdRestore(
    IN BOOLEAN KdSleepTransition
    );

NTSTATUS
KdSave(
    IN BOOLEAN KdSleepTransition
    );

//
// status Constants for Packet waiting
//

#define KDP_PACKET_RECEIVED 0
#define KDP_PACKET_TIMEOUT 1
#define KDP_PACKET_RESEND 2

VOID
KdSendPacket(
    IN ULONG PacketType,
    IN PSTRING MessageHeader,
    IN PSTRING MessageData OPTIONAL,
    IN OUT PKD_CONTEXT KdContext
    );

#endif // __KDDLL_H__
