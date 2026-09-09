/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    adtlog.c

Abstract:

    Auditing - Audit Record Queuing and Logging Routines

    This file contains functions that construct Audit Records in self-
    relative form from supplied information, enqueue/dequeue them and
    write them to the log.

--*/

#include "pch.h"

#pragma hdrstop

#include <msaudite.h>

#ifndef SE_AUDITID_AUDITS_DISCARDED
#define SE_AUDITID_AUDITS_DISCARDED ((ULONG)0x00000204L)
#endif


#ifdef ALLOC_PRAGMA
