/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    perfdata.c

Abstract:

    This module contains the global read/write data for the perf subsystem

--*/

#include "perfp.h"

PERFINFO_GROUPMASK PerfGlobalGroupMask;
PERFINFO_GROUPMASK *PPerfGlobalGroupMask;
const PERFINFO_HOOK_HANDLE PerfNullHookHandle = { NULL, NULL };

//
// Profiling
//

KPROFILE PerfInfoProfileObject;
KPROFILE_SOURCE PerfInfoProfileSourceActive = ProfileMaximum;   // Set to invalid source
KPROFILE_SOURCE PerfInfoProfileSourceRequested = ProfileTime;
KPROFILE_SOURCE PerfInfoProfileInterval = 10000;    // 1ms in 100ns ticks
BOOLEAN PerfInfoSampledProfileCaching;
LONG PerfInfoSampledProfileFlushInProgress;
PERFINFO_SAMPLED_PROFILE_CACHE PerfProfileCache;

