/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    prefetch.h

Abstract:

    This module contains the public data structures and procedure
    prototypes for the kernel mode prefetcher.

Author:

    Arthur Zwiegincew (arthurz) 13-May-1999
    Stuart Sechrest (stuartse)  15-Jul-1999
    Chuck Lenzmeier (chuckl)    15-Mar-2000
    Cenk Ergan (cenke)          15-Mar-2000

Revision History:

--*/

#ifndef _PREFETCH_H_
#define _PREFETCH_H_

//
// Prefetcher scenario types.
//

typedef enum _PF_SCENARIO_TYPE {
    PfApplicationLaunchScenarioType,
    PfSystemBootScenarioType,
    PfAppScenarioType,
    PfEnrollmentScenarioType,
    PfHardwareConfigScenarioType,
    PfActivityScenarioType,
    PfTraceScenarioType,
    PfQwordScenarioType,
    PfMaxScenarioType
} PF_SCENARIO_TYPE, *PPF_SCENARIO_TYPE;

//
// Prefetcher boot phase IDs.
//

typedef enum _PF_BOOT_PHASE_ID {
    PfBootPhaseNone,
    PfBootPhaseDriverInit,
    PfBootPhaseSystemExtends,
    PfBootPhaseBootDriver,
    PfBootPhaseFileSystem,
    PfBootPhaseUserMode,
    PfBootPhaseMax
} PF_BOOT_PHASE_ID, *PPF_BOOT_PHASE_ID;

//
// Prefetcher information classes.
//

typedef enum _PREFETCHER_INFORMATION_CLASS {
    PrefetcherInformation = 0,
    BootPrefetcherInformation,
    MaxPrefetcherInfoClass
} PREFETCHER_INFORMATION_CLASS;

//
// Prefetcher parameters structure.
//

typedef struct _PREFETCHER_PARAMETERS {
    ULONG EnablePrefetcher;
    ULONG BootPrefetchEnabled;
    ULONG PrefetcherVersion;
} PREFETCHER_PARAMETERS, *PPREFETCHER_PARAMETERS;

//
// Prefetcher tracing structures.
//

typedef struct _CCPF_TRACE_HEADER {
    ULONG Version;
    ULONG TraceSize;
    ULONG ScenarioType;
    ULONG Reserved;
} CCPF_TRACE_HEADER, *PCCPF_TRACE_HEADER;

//
// Page fault logging flags.
//

#define CCPF_TYPE_IMAGE             0x00000001
#define CCPF_TYPE_ROM               0x00000002

#endif // _PREFETCH_H_
