/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    cmdat2.c

Abstract:

    This module contains data strings that describes the registry space
    and that are exported to the rest of the system.

--*/

#include "cmp.h"

//
// ***** PAGE *****
//

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("PAGEDATA")
#endif

//
// control values/overrides read from registry
//
ULONG CmRegistrySizeLimit = { 0 };
ULONG CmRegistrySizeLimitLength = 4;
ULONG CmRegistrySizeLimitType = { 0 };

//
// Maximum number of bytes of Global Quota the registry may use.
// Set to largest positive number for use in boot.  Will be set down
// based on pool and explicit registry values.
//
SIZE_T  CmpGlobalQuotaAllowed = CM_WRAP_LIMIT;
SIZE_T  CmpGlobalQuota = CM_WRAP_LIMIT;
SIZE_T  CmpGlobalQuotaWarning = CM_WRAP_LIMIT;
BOOLEAN CmpQuotaWarningPopupDisplayed = FALSE;
BOOLEAN CmpSystemQuotaWarningPopupDisplayed = FALSE;

//
// the "disk full" popup has already been displayed
//
BOOLEAN CmpDiskFullWorkerPopupDisplayed = FALSE;
BOOLEAN CmpCannotWriteConfiguration = FALSE;

//
// GQ actually in use
//
SIZE_T  CmpGlobalQuotaUsed = 0;

//
// State flag to remember when to turn it on
//
BOOLEAN CmpProfileLoaded = FALSE;

PUCHAR CmpStashBuffer = NULL;
ULONG  CmpStashBufferSize = 0;
EX_PUSH_LOCK CmpStashBufferLock;

//
// Shutdown control
//
BOOLEAN HvShutdownComplete = FALSE;     // Set to true after shutdown
                                        // to disable any further I/O

PCM_KEY_CONTROL_BLOCK CmpKeyControlBlockRoot = NULL;

HANDLE CmpRegistryRootHandle = NULL;

struct {
    PHHIVE      Hive;
    ULONG       Status;
} CmCheckRegistryDebug = { 0 };

//
// The last I/O error status code
//
struct {
    ULONG       Action;
    HANDLE      Handle;
    NTSTATUS    Status;
} CmRegistryIODebug = { 0 };

//
// globals private to check code
//

struct {
    PHHIVE      Hive;
    ULONG       Status;
} CmpCheckRegistry2Debug = { 0 };

struct {
    PHHIVE      Hive;
    ULONG       Status;
    HCELL_INDEX Cell;
    PCELL_DATA  CellPoint;
    PVOID       RootPoint;
    ULONG       Index;
} CmpCheckKeyDebug = { 0 };

struct {
    PHHIVE      Hive;
    ULONG       Status;
    PCELL_DATA  List;
    ULONG       Index;
    HCELL_INDEX Cell;
    PCELL_DATA  CellPoint;
} CmpCheckValueListDebug = { 0 };

ULONG CmpUsedStorage = { 0 };

// hivechek.c
struct {
    PHHIVE      Hive;
    ULONG       Status;
    ULONG       Space;
    HCELL_INDEX MapPoint;
    PHBIN       BinPoint;
} HvCheckHiveDebug = { 0 };

struct {
    PHBIN       Bin;
    ULONG       Status;
    PHCELL      CellPoint;
} HvCheckBinDebug = { 0 };

struct {
    PHHIVE      Hive;
    ULONG       FileOffset;
    ULONG       FailPoint; // look in HvpRecoverData for exact point of failure
} HvRecoverDataDebug = { 0 };

//
// when a local hive cannot be loaded, set this to it's index
// and the load hive worker thread responsible for it will be held of 
// until all the others finish; We can then debug the offending hive
//
ULONG   CmpCheckHiveIndex = CM_NUMBER_OF_MACHINE_HIVES;

#ifdef ALLOC_DATA_PRAGMA
#pragma  data_seg()
#endif
