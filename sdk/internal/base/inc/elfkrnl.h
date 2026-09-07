/*++

Copyright (c) OpenXP Contributors
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    elfkrnl.h

Abstract:

    This file contains defines for kernel->elf data structures

--*/

#ifndef _ELFKRNL_
#define _ELFKRNL_

#define     ELF_PORT_NAME	    "\\ErrorLogPort"
#define     ELF_PORT_NAME_U	    L"\\ErrorLogPort"

//
//  Type discriminator
//

typedef enum {
    IO_ERROR_LOG = 0,
    SM_ERROR_LOG,
} ELF_MESSAGE_TYPE;


//
//  SM event structure
//

typedef struct {
    LARGE_INTEGER TimeStamp;
    NTSTATUS Status;
    ULONG StringOffset;
    ULONG StringLength;
} SM_ERROR_LOG_MESSAGE, *PSM_ERROR_LOG_MESSAGE;


//
// Max size of data sent to the eventlogging service through the LPC port.
//

#define     ELF_PORT_MAX_MESSAGE_LENGTH PORT_MAXIMUM_MESSAGE_LENGTH


//
// Structure that is passed in from the system thread to the LPC port
//

typedef struct  {
   PORT_MESSAGE PortMessage;
   ULONG MessageType;
   union {
       IO_ERROR_LOG_MESSAGE IoErrorLogMessage;
       SM_ERROR_LOG_MESSAGE SmErrorLogMessage;
   } u;
} ELF_PORT_MSG, *PELF_PORT_MSG;


//
// Structure for the message as a reply from the eventlogging service to
// the LPC client.
//

typedef struct _ELF_REPLY_MESSAGE {
    PORT_MESSAGE PortMessage;
    NTSTATUS Status;
} ELF_REPLY_MESSAGE, *PELF_REPLY_MESSAGE;

#endif // ifndef _ELFLPC_

