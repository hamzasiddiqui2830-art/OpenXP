
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
    __in LPGUID VendorGuid,
    __out_bcount_opt(*ValueLength) PVOID Value,
    __inout PULONG ValueLength,
    __out_opt PULONG Attributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemEnvironmentValueEx (
    __in PUNICODE_STRING VariableName,
    __in LPGUID VendorGuid,
    __in_bcount_opt(ValueLength) PVOID Value,
    __in ULONG ValueLength,
    __in ULONG Attributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateSystemEnvironmentValuesEx (
    __in ULONG InformationClass,
    __out PVOID Buffer,
    __inout PULONG BufferLength
    );

// begin_nthal

#define VARIABLE_ATTRIBUTE_NON_VOLATILE 0x00000001

#define VARIABLE_INFORMATION_NAMES  1
#define VARIABLE_INFORMATION_VALUES 2

typedef struct _VARIABLE_NAME {
    ULONG NextEntryOffset;
    GUID VendorGuid;
    WCHAR Name[ANYSIZE_ARRAY];
} VARIABLE_NAME, *PVARIABLE_NAME;

typedef struct _VARIABLE_NAME_AND_VALUE {
    ULONG NextEntryOffset;
    ULONG ValueOffset;
    ULONG ValueLength;
    ULONG Attributes;
    GUID VendorGuid;
    WCHAR Name[ANYSIZE_ARRAY];
    //UCHAR Value[ANYSIZE_ARRAY];
} VARIABLE_NAME_AND_VALUE, *PVARIABLE_NAME_AND_VALUE;

// end_nthal

//
// Boot entry management APIs.
//

typedef struct _FILE_PATH {
    ULONG Version;
    ULONG Length;
    ULONG Type;
    UCHAR FilePath[ANYSIZE_ARRAY];
} FILE_PATH, *PFILE_PATH;

#define FILE_PATH_VERSION 1

#define FILE_PATH_TYPE_ARC           1
#define FILE_PATH_TYPE_ARC_SIGNATURE 2
#define FILE_PATH_TYPE_NT            3
#define FILE_PATH_TYPE_EFI           4

#define FILE_PATH_TYPE_MIN FILE_PATH_TYPE_ARC
#define FILE_PATH_TYPE_MAX FILE_PATH_TYPE_EFI

typedef struct _WINDOWS_OS_OPTIONS {
    UCHAR Signature[8];
    ULONG Version;
    ULONG Length;
    ULONG OsLoadPathOffset;
    WCHAR OsLoadOptions[ANYSIZE_ARRAY];
    //FILE_PATH OsLoadPath;
} WINDOWS_OS_OPTIONS, *PWINDOWS_OS_OPTIONS;

#define WINDOWS_OS_OPTIONS_SIGNATURE "WINDOWS"

#define WINDOWS_OS_OPTIONS_VERSION 1

typedef struct _BOOT_ENTRY {
    ULONG Version;
    ULONG Length;
    ULONG Id;
    ULONG Attributes;
    ULONG FriendlyNameOffset;
    ULONG BootFilePathOffset;
    ULONG OsOptionsLength;
    UCHAR OsOptions[ANYSIZE_ARRAY];
    //WCHAR FriendlyName[ANYSIZE_ARRAY];
    //FILE_PATH BootFilePath;
} BOOT_ENTRY, *PBOOT_ENTRY;

#define BOOT_ENTRY_VERSION 1

#define BOOT_ENTRY_ATTRIBUTE_ACTIVE             0x00000001
#define BOOT_ENTRY_ATTRIBUTE_DEFAULT            0x00000002
#define BOOT_ENTRY_ATTRIBUTE_WINDOWS            0x00000004
#define BOOT_ENTRY_ATTRIBUTE_REMOVABLE_MEDIA    0x00000008

#define BOOT_ENTRY_ATTRIBUTE_VALID_BITS (  \
            BOOT_ENTRY_ATTRIBUTE_ACTIVE  | \
            BOOT_ENTRY_ATTRIBUTE_DEFAULT   \
            )

typedef struct _BOOT_OPTIONS {
    ULONG Version;
    ULONG Length;
    ULONG Timeout;
    ULONG CurrentBootEntryId;
    ULONG NextBootEntryId;
    WCHAR HeadlessRedirection[ANYSIZE_ARRAY];
} BOOT_OPTIONS, *PBOOT_OPTIONS;

#define BOOT_OPTIONS_VERSION 1

#define BOOT_OPTIONS_FIELD_TIMEOUT              0x00000001
#define BOOT_OPTIONS_FIELD_NEXT_BOOT_ENTRY_ID   0x00000002
#define BOOT_OPTIONS_FIELD_HEADLESS_REDIRECTION 0x00000004

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddBootEntry (
    __in PBOOT_ENTRY BootEntry,
    __out_opt PULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteBootEntry (
    __in ULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtModifyBootEntry (
    __in PBOOT_ENTRY BootEntry
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateBootEntries (
    __out_bcount_opt(*BufferLength) PVOID Buffer,
    __inout PULONG BufferLength
    );

typedef struct _BOOT_ENTRY_LIST {
    ULONG NextEntryOffset;
    BOOT_ENTRY BootEntry;
} BOOT_ENTRY_LIST, *PBOOT_ENTRY_LIST;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryBootEntryOrder (
    __out_ecount_opt(*Count) PULONG Ids,
    __inout PULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetBootEntryOrder (
    __in_ecount(Count) PULONG Ids,
    __in ULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryBootOptions (
    __out_bcount_opt(*BootOptionsLength) PBOOT_OPTIONS BootOptions,
    __inout PULONG BootOptionsLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetBootOptions (
    __in PBOOT_OPTIONS BootOptions,
    __in ULONG FieldsToChange
    );

#define BOOT_OPTIONS_FIELD_COUNTDOWN            0x00000001
#define BOOT_OPTIONS_FIELD_NEXT_BOOT_ENTRY_ID   0x00000002
#define BOOT_OPTIONS_FIELD_HEADLESS_REDIRECTION 0x00000004

NTSYSCALLAPI
NTSTATUS
NTAPI
NtTranslateFilePath (
    __in PFILE_PATH InputFilePath,
    __in ULONG OutputType,
    __out_bcount_opt(*OutputFilePathLength) PFILE_PATH OutputFilePath,
    __inout_opt PULONG OutputFilePathLength
    );

//
// Driver entry management APIs.
//

typedef struct _EFI_DRIVER_ENTRY {
    ULONG Version;
    ULONG Length;
    ULONG Id;
    ULONG FriendlyNameOffset;
    ULONG DriverFilePathOffset;
    //WCHAR FriendlyName[ANYSIZE_ARRAY];
    //FILE_PATH DriverFilePath;
} EFI_DRIVER_ENTRY, *PEFI_DRIVER_ENTRY;

typedef struct _EFI_DRIVER_ENTRY_LIST {
    ULONG NextEntryOffset;
    EFI_DRIVER_ENTRY DriverEntry;
} EFI_DRIVER_ENTRY_LIST, *PEFI_DRIVER_ENTRY_LIST;

#define EFI_DRIVER_ENTRY_VERSION 1

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddDriverEntry (
    __in PEFI_DRIVER_ENTRY DriverEntry,
    __out_opt PULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteDriverEntry (
    __in ULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtModifyDriverEntry (
    __in PEFI_DRIVER_ENTRY DriverEntry
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateDriverEntries (
    __out_bcount(*BufferLength) PVOID Buffer,
    __inout PULONG BufferLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDriverEntryOrder (
    __out_ecount(*Count) PULONG Ids,
    __inout PULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDriverEntryOrder (
    __in_ecount(Count) PULONG Ids,
    __in ULONG Count
    );

// begin_ntddk 

//
// Firmware Table provider definitions 
//

typedef enum _SYSTEM_FIRMWARE_TABLE_ACTION {
    SystemFirmwareTable_Enumerate,
    SystemFirmwareTable_Get
} SYSTEM_FIRMWARE_TABLE_ACTION;

typedef struct _SYSTEM_FIRMWARE_TABLE_INFORMATION {
    ULONG                           ProviderSignature;
    SYSTEM_FIRMWARE_TABLE_ACTION    Action;
    ULONG                           TableID;
    ULONG                           TableBufferLength;
    UCHAR                           TableBuffer[ANYSIZE_ARRAY];
} SYSTEM_FIRMWARE_TABLE_INFORMATION, *PSYSTEM_FIRMWARE_TABLE_INFORMATION;

typedef NTSTATUS (__cdecl *PFNFTH)(PSYSTEM_FIRMWARE_TABLE_INFORMATION);

typedef struct _SYSTEM_FIRMWARE_TABLE_HANDLER {
    ULONG       ProviderSignature;
    BOOLEAN     Register;
    PFNFTH      FirmwareTableHandler;
    PVOID       DriverObject;
} SYSTEM_FIRMWARE_TABLE_HANDLER, *PSYSTEM_FIRMWARE_TABLE_HANDLER;

// end_ntddk

// begin_ntifs begin_wdm begin_ntddk
//
// Event Specific Access Rights.
//

#define EVENT_QUERY_STATE       0x0001
#define EVENT_MODIFY_STATE      0x0002  // winnt
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt

// end_ntifs end_wdm end_ntddk

//
// Event Information Classes.
//

typedef enum _EVENT_INFORMATION_CLASS {
    EventBasicInformation
} EVENT_INFORMATION_CLASS;

//
// Event Information Structures.
//

typedef struct _EVENT_BASIC_INFORMATION {
    EVENT_TYPE EventType;
    LONG EventState;
} EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

//
// Event object function definitions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtClearEvent (
    __in HANDLE EventHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateEvent (
    __out PHANDLE EventHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in EVENT_TYPE EventType,
    __in BOOLEAN InitialState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenEvent (
    __out PHANDLE EventHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtPulseEvent (
    __in HANDLE EventHandle,
    __out_opt PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryEvent (
    __in HANDLE EventHandle,
    __in EVENT_INFORMATION_CLASS EventInformationClass,
    __out_bcount(EventInformationLength) PVOID EventInformation,
    __in ULONG EventInformationLength,
    __out_opt PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtResetEvent (
    __in HANDLE EventHandle,
    __out_opt PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEvent (
    __in HANDLE EventHandle,
    __out_opt PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEventBoostPriority (
    __in HANDLE EventHandle
    );

//
// Event Specific Access Rights.
//

#define EVENT_PAIR_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE)

//
// Event pair object function definitions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateEventPair (
    __out PHANDLE EventPairHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenEventPair (
    __out PHANDLE EventPairHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitLowEventPair ( 
    __in HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitHighEventPair (
    __in HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLowWaitHighEventPair (
    __in HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetHighWaitLowEventPair (
    __in HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLowEventPair (
    __in HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetHighEventPair (
    __in HANDLE EventPairHandle
    );

//
// Mutant Specific Access Rights.
//
// begin_winnt

#define MUTANT_QUERY_STATE      0x0001

#define MUTANT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|\
                          MUTANT_QUERY_STATE)
// end_winnt

//
// Mutant Information Classes.
//

typedef enum _MUTANT_INFORMATION_CLASS {
    MutantBasicInformation
} MUTANT_INFORMATION_CLASS;

//
// Mutant Information Structures.
//

typedef struct _MUTANT_BASIC_INFORMATION {
    LONG CurrentCount;
    BOOLEAN OwnedByCaller;
    BOOLEAN AbandonedState;
} MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

//
// Mutant object function definitions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateMutant (
    __out PHANDLE MutantHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in BOOLEAN InitialOwner
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenMutant (
    __out PHANDLE MutantHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryMutant (
    __in HANDLE MutantHandle,
    __in MUTANT_INFORMATION_CLASS MutantInformationClass,
    __out_bcount(MutantInformationLength) PVOID MutantInformation,
    __in ULONG MutantInformationLength,
    __out_opt PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseMutant (
    __in HANDLE MutantHandle,
    __out_opt PLONG PreviousCount
    );

// begin_ntifs begin_wdm begin_ntddk
//
// Semaphore Specific Access Rights.
//

#define SEMAPHORE_QUERY_STATE       0x0001
#define SEMAPHORE_MODIFY_STATE      0x0002  // winnt

#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt

// end_ntifs end_wdm end_ntddk

//
// Semaphore Information Classes.
//

typedef enum _SEMAPHORE_INFORMATION_CLASS {
    SemaphoreBasicInformation
} SEMAPHORE_INFORMATION_CLASS;

//
// Semaphore Information Structures.
//

typedef struct _SEMAPHORE_BASIC_INFORMATION {
    LONG CurrentCount;
    LONG MaximumCount;
} SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

//
// Semaphore object function definitions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSemaphore (
    __out PHANDLE SemaphoreHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in LONG InitialCount,
    __in LONG MaximumCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSemaphore(
    __out PHANDLE SemaphoreHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySemaphore (
    __in HANDLE SemaphoreHandle,
    __in SEMAPHORE_INFORMATION_CLASS SemaphoreInformationClass,
    __out_bcount(SemaphoreInformationLength) PVOID SemaphoreInformation,
    __in ULONG SemaphoreInformationLength,
    __out_opt PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseSemaphore(
    __in HANDLE SemaphoreHandle,
    __in LONG ReleaseCount,
    __out_opt PLONG PreviousCount
    );

// begin_winnt
//
// Timer Specific Access Rights.
//

#define TIMER_QUERY_STATE       0x0001
#define TIMER_MODIFY_STATE      0x0002

#define TIMER_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|\
                          TIMER_QUERY_STATE|TIMER_MODIFY_STATE)

// end_winnt
//
// Timer Information Classes.
//

typedef enum _TIMER_INFORMATION_CLASS {
    TimerBasicInformation
} TIMER_INFORMATION_CLASS;

//
// Timer Information Structures.
//

typedef struct _TIMER_BASIC_INFORMATION {
    LARGE_INTEGER RemainingTime;
    BOOLEAN TimerState;
} TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;

// begin_ntddk
//
// Timer APC routine definition.
//

typedef
VOID
(*PTIMER_APC_ROUTINE) (
    __in PVOID TimerContext,
    __in ULONG TimerLowValue,
    __in LONG TimerHighValue
    );

// end_ntddk

//
// Timer object function definitions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateTimer (
    __out PHANDLE TimerHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in TIMER_TYPE TimerType
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenTimer (
    __out PHANDLE TimerHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelTimer (
    __in HANDLE TimerHandle,
    __out_opt PBOOLEAN CurrentState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryTimer (
    __in HANDLE TimerHandle,
    __in TIMER_INFORMATION_CLASS TimerInformationClass,
    __out_bcount(TimerInformationLength) PVOID TimerInformation,
    __in ULONG TimerInformationLength,
    __out_opt PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimer (
    __in HANDLE TimerHandle,
    __in PLARGE_INTEGER DueTime,
    __in_opt PTIMER_APC_ROUTINE TimerApcRoutine,
    __in_opt PVOID TimerContext,
    __in BOOLEAN ResumeTimer,
    __in_opt LONG Period,
    __out_opt PBOOLEAN PreviousState
    );

//
// System Time and Timer function definitions
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemTime (
    __out PLARGE_INTEGER SystemTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemTime (
    __in_opt PLARGE_INTEGER SystemTime,
    __out_opt PLARGE_INTEGER PreviousTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryTimerResolution (
    __out PULONG MaximumTime,
    __out PULONG MinimumTime,
    __out PULONG CurrentTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimerResolution (
    __in ULONG DesiredTime,
    __in BOOLEAN SetResolution,
    __out PULONG ActualTime
    );

//
//  Locally Unique Identifier (LUID) allocation
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateLocallyUniqueId (
    __out PLUID Luid
    );

//
//  Universally Unique Identifier (UUID) time allocation
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetUuidSeed (
    __in PCHAR Seed
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateUuids (
    __out PULARGE_INTEGER Time,
    __out PULONG Range,
    __out PULONG Sequence,
    __out PCHAR Seed
    );

//
// Profile Object Definitions
//

#define PROFILE_CONTROL           0x0001
#define PROFILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | PROFILE_CONTROL)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProfile (
    __out PHANDLE ProfileHandle,
    __in HANDLE Process OPTIONAL,
    __in PVOID ProfileBase,
    __in SIZE_T ProfileSize,
    __in ULONG BucketSize,
    __in PULONG Buffer,
    __in ULONG BufferSize,
    __in KPROFILE_SOURCE ProfileSource,
    __in KAFFINITY Affinity
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtStartProfile (
    __in HANDLE ProfileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtStopProfile (
    __in HANDLE ProfileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetIntervalProfile (
    __in ULONG Interval,
    __in KPROFILE_SOURCE Source
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryIntervalProfile (
    __in KPROFILE_SOURCE ProfileSource,
    __out PULONG Interval
    );

//
// Performance Counter Definitions
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter (
    __out PLARGE_INTEGER PerformanceCounter,
    __out_opt PLARGE_INTEGER PerformanceFrequency
    );

#define KEYEDEVENT_WAIT 0x0001
#define KEYEDEVENT_WAKE 0x0002
#define KEYEDEVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | KEYEDEVENT_WAIT | KEYEDEVENT_WAKE)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateKeyedEvent (
    __out PHANDLE KeyedEventHandle,
    __in ACCESS_MASK DesiredAccess,
    __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
    __in ULONG Flags
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKeyedEvent (
    __out PHANDLE KeyedEventHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent (
    __in HANDLE KeyedEventHandle,
    __in PVOID KeyValue,
    __in BOOLEAN Alertable,
    __in_opt PLARGE_INTEGER Timeout
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent (
    __in HANDLE KeyedEventHandle,
    __in PVOID KeyValue,
    __in BOOLEAN Alertable,
    __in_opt PLARGE_INTEGER Timeout
    );

//
// Nt Api Profile Definitions
//
// Nt Api Profiling data structure
//

typedef struct _NAPDATA {
    ULONG NapLock;
    ULONG Calls;
    ULONG TimingErrors;
    LARGE_INTEGER TotalTime;
    LARGE_INTEGER FirstTime;
    LARGE_INTEGER MaxTime;
    LARGE_INTEGER MinTime;
} NAPDATA, *PNAPDATA;

NTSTATUS
NapClearData (
    VOID
    );

NTSTATUS
NapRetrieveData (
    OUT NAPDATA *NapApiData,
    OUT PCHAR **NapApiNames,
    OUT PLARGE_INTEGER *NapCounterFrequency
    );

NTSTATUS
NapGetApiCount (
    OUT PULONG NapApiCount
    );

NTSTATUS
NapPause (
    VOID
    );

NTSTATUS
NapResume (
    VOID
    );

// begin_ntifs begin_ntddk

//
//  Driver Verifier Definitions
//

typedef ULONG_PTR (*PDRIVER_VERIFIER_THUNK_ROUTINE) (
    IN PVOID Context
    );

//
//  This structure is passed in by drivers that want to thunk callers of
//  their exports.
//

typedef struct _DRIVER_VERIFIER_THUNK_PAIRS {
    PDRIVER_VERIFIER_THUNK_ROUTINE  PristineRoutine;
    PDRIVER_VERIFIER_THUNK_ROUTINE  NewRoutine;
} DRIVER_VERIFIER_THUNK_PAIRS, *PDRIVER_VERIFIER_THUNK_PAIRS;

//
//  Driver Verifier flags.
//

#define DRIVER_VERIFIER_SPECIAL_POOLING             0x0001
#define DRIVER_VERIFIER_FORCE_IRQL_CHECKING         0x0002
#define DRIVER_VERIFIER_INJECT_ALLOCATION_FAILURES  0x0004
#define DRIVER_VERIFIER_TRACK_POOL_ALLOCATIONS      0x0008
#define DRIVER_VERIFIER_IO_CHECKING                 0x0010

// end_ntifs end_ntddk

#define DRIVER_VERIFIER_DEADLOCK_DETECTION          0x0020
#define DRIVER_VERIFIER_ENHANCED_IO_CHECKING        0x0040
#define DRIVER_VERIFIER_DMA_VERIFIER                0x0080
#define DRIVER_VERIFIER_HARDWARE_VERIFICATION       0x0100
#define DRIVER_VERIFIER_SYSTEM_BIOS_VERIFICATION    0x0200
#define DRIVER_VERIFIER_EXPOSE_IRP_HISTORY          0x0400

//
// System Information Classes.
//

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation,
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemVdmInstemulInformation,
    SystemVdmBopInformation,
    SystemFileCacheInformation,
    SystemPoolTagInformation,
    SystemInterruptInformation,
    SystemDpcBehaviorInformation,
    SystemFullMemoryInformation,
    SystemLoadGdiDriverInformation,
    SystemUnloadGdiDriverInformation,
    SystemTimeAdjustmentInformation,
    SystemSummaryMemoryInformation,
    SystemMirrorMemoryInformation,
    SystemPerformanceTraceInformation,
    SystemObsolete0,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemExtendServiceTableInformation,
    SystemPrioritySeperation,
    SystemVerifierAddDriverInformation,
    SystemVerifierRemoveDriverInformation,
    SystemProcessorIdleInformation,
    SystemLegacyDriverInformation,
    SystemCurrentTimeZoneInformation,
    SystemLookasideInformation,
    SystemTimeSlipNotification,
    SystemSessionCreate,
    SystemSessionDetach,
    SystemSessionInformation,
    SystemRangeStartInformation,
    SystemVerifierInformation,
    SystemVerifierThunkExtend,
    SystemSessionProcessInformation,
    SystemLoadGdiDriverInSystemSpace,
    SystemNumaProcessorMap,
    SystemPrefetcherInformation,
    SystemExtendedProcessInformation,
    SystemRecommendedSharedDataAlignment,
    SystemComPlusPackage,
    SystemNumaAvailableMemory,
    SystemProcessorPowerInformation,
    SystemEmulationBasicInformation,
    SystemEmulationProcessorInformation,
    SystemExtendedHandleInformation,
    SystemLostDelayedWriteInformation,
    SystemBigPoolInformation,
    SystemSessionPoolTagInformation,
    SystemSessionMappedViewInformation,
    SystemHotpatchInformation,
    SystemObjectSecurityMode,
    SystemWatchdogTimerHandler,
    SystemWatchdogTimerInformation,
    SystemLogicalProcessorInformation,
    SystemWow64SharedInformation,
    SystemRegisterFirmwareTableInformationHandler,
    SystemFirmwareTableInformation,
    SystemModuleInformationEx,
    SystemVerifierTriageInformation,
    SystemSuperfetchInformation,
    SystemMemoryListInformation,
    SystemFileCacheInformationEx,
    MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;

//
// System Information Structures.
//

// begin_winnt
#define TIME_ZONE_ID_UNKNOWN  0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2
// end_winnt

typedef struct _SYSTEM_VDM_INSTEMUL_INFO {
    ULONG SegmentNotPresent ;
    ULONG VdmOpcode0F       ;
    ULONG OpcodeESPrefix    ;
    ULONG OpcodeCSPrefix    ;
    ULONG OpcodeSSPrefix    ;
    ULONG OpcodeDSPrefix    ;
    ULONG OpcodeFSPrefix    ;
    ULONG OpcodeGSPrefix    ;
    ULONG OpcodeOPER32Prefix;
    ULONG OpcodeADDR32Prefix;
    ULONG OpcodeINSB        ;
    ULONG OpcodeINSW        ;
    ULONG OpcodeOUTSB       ;
    ULONG OpcodeOUTSW       ;
    ULONG OpcodePUSHF       ;
    ULONG OpcodePOPF        ;
    ULONG OpcodeINTnn       ;
    ULONG OpcodeINTO        ;
    ULONG OpcodeIRET        ;
    ULONG OpcodeINBimm      ;
    ULONG OpcodeINWimm      ;
    ULONG OpcodeOUTBimm     ;
    ULONG OpcodeOUTWimm     ;
    ULONG OpcodeINB         ;
    ULONG OpcodeINW         ;
    ULONG OpcodeOUTB        ;
    ULONG OpcodeOUTW        ;
    ULONG OpcodeLOCKPrefix  ;
    ULONG OpcodeREPNEPrefix ;
    ULONG OpcodeREPPrefix   ;
    ULONG OpcodeHLT         ;
    ULONG OpcodeCLI         ;
    ULONG OpcodeSTI         ;
    ULONG BopCount          ;
} SYSTEM_VDM_INSTEMUL_INFO, *PSYSTEM_VDM_INSTEMUL_INFO;

//
// System Time of Day Information.
//

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION {
    LARGE_INTEGER BootTime;
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER TimeZoneBias;
    ULONG TimeZoneId;
    ULONG Reserved;
    ULONGLONG BootTimeBias;
    ULONGLONG SleepTimeBias;
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

#if defined(_WIN64)
typedef ULONG SYSINF_PAGE_COUNT;
#else
typedef SIZE_T SYSINF_PAGE_COUNT;
#endif

typedef struct _SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    SYSINF_PAGE_COUNT NumberOfPhysicalPages;
    SYSINF_PAGE_COUNT LowestPhysicalPageNumber;
    SYSINF_PAGE_COUNT HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG_PTR MinimumUserModeAddress;
    ULONG_PTR MaximumUserModeAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

// Remaining declarations are intentionally preserved from the original SDK file.

//
// Routines for manipulating global atoms stored in kernel space
//

typedef USHORT RTL_ATOM, *PRTL_ATOM;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddAtom (
    __in_bcount_opt(Length) PWSTR AtomName,
    __in ULONG Length,
    __out_opt PRTL_ATOM Atom
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFindAtom (
    __in_bcount_opt(Length) PWSTR AtomName,
    __in ULONG Length,
    __out_opt PRTL_ATOM Atom
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteAtom (
    __in RTL_ATOM Atom
    );

#ifdef __cplusplus
}
#endif

#endif // _NTEXAPI_
