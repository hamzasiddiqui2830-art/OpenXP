/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Module Name:

   miglobal.c

Abstract:

    This module contains the private global storage for the memory
    management subsystem.

--*/
#include "mi.h"

#if !defined(_WIN64)

ULONG_PTR MmVirtualBias;

#if defined(_X86_)
MMPTE MmPteGlobal = {0x0};
PVOID MmHyperSpaceEnd;
#endif

#endif

//
// Virtual size of system cache in pages.
//
ULONG_PTR MmSizeOfSystemCacheInPages;

//
// System cache working set.
//
PMMWSL MmSystemCacheWorkingSetList = (PMMWSL)MM_SYSTEM_CACHE_WORKING_SET;
MMSUPPORT MmSystemCacheWs;

//
// System cache working set list entries.
//
PMMWSLE MmSystemCacheWsle;

//
// System cache size.
//
SIZE_T MmSystemCacheWorkingSetSize;

//
// Global MM information.
//
MMINFO_COUNTERS MmInfoCounters;
