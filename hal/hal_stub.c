/*
 * Stub Hardware Abstraction Layer (HAL) for Windows Research Kernel
 * This provides stub implementations of HAL functions to allow compilation
 * without depending on precompiled HAL DLL
 *
 * Copyright (c) OpenXP Contributors
 Copyright (c) OpenXP Team 2026.
 * You may only use this code if you agree to the terms of the Windows Research 
 * Kernel Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 */

#include <ntdef.h>
#include <hal.h>

/*
 * Stub HAL Private Dispatch Table
 * This table contains function pointers to HAL implementation functions.
 * For the research kernel, we provide minimal stub implementations.
 */

/* Forward declarations of stub functions */
static NTSTATUS STUBAPI HalpRegisterBusHandler(
    INTERFACE_TYPE BusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG BusNumber,
    PBUS_HANDLER *BusHandler,
    PBUS_HANDLER ParentHandler
);

static PBUS_HANDLER STUBAPI HalpHandlerForBus(
    INTERFACE_TYPE BusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG Version,
    PVOID Context,
    PBUS_HANDLER ParentHandler
);

static PACCESS_BUS_HANDLER STUBAPI HalpHandlerForConfigSpace(
    ACCESS_BUS_TYPE AccessBusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG Version,
    PVOID Context
);

static VOID STUBAPI HalpLocateHiberRanges(
    PVOID HiberContext
);

static BOOLEAN STUBAPI HalpSetWakeEnable(
    BOOLEAN Enable
);

static NTSTATUS STUBAPI HalpSetWakeAlarm(
    ULONGLONG AlarmTime,
    BOOLEAN SystemTime
);

static VOID STUBAPI HalpHaltSystem(
    VOID
);

static VOID STUBAPI HalpResetDisplay(
    VOID
);

static NTSTATUS STUBAPI HalpAllocateMapRegisters(
    PADAPTER_OBJECT AdapterObject,
    ULONG BasePage,
    ULONG NumberOfMapRegisters,
    PMAP_REGISTER_ENTRY MapRegisters
);

static PKDPC STUBAPI HalpKdSetupPciDeviceForDebugging(
    PPCI_COMMON_CONFIG PciData,
    PKDPC Dpc
);

static VOID STUBAPI HalpKdReleasePciDeviceForDebugging(
    PKDPC Dpc
);

static PCHAR STUBAPI HalpKdGetAcpiTablePhase0(
    PKDDEBUGGER_DATA64 DebuggerDataBlock
);

static BOOLEAN STUBAPI HalpKdCheckPowerButton(
    VOID
);

static UCHAR STUBAPI HalpVectorToIDTEntry(
    KINTERRUPT_VECTOR Vector
);

static PVOID STUBAPI HalpKdMapPhysicalMemory64(
    PHYSICAL_ADDRESS PhysicalAddress,
    ULONG Length,
    BOOLEAN CacheEnabled
);

static VOID STUBAPI HalpKdUnmapVirtualAddress(
    PVOID VirtualAddress,
    ULONG Length
);

/*
 * Stub implementation of HalRegisterBusHandler
 */
static NTSTATUS STUBAPI HalpRegisterBusHandler(
    INTERFACE_TYPE BusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG BusNumber,
    PBUS_HANDLER *BusHandler,
    PBUS_HANDLER ParentHandler
)
{
    /* Stub - return success but don't actually register anything */
    if (BusHandler) {
        *BusHandler = NULL;
    }
    return STATUS_SUCCESS;
}

/*
 * Stub implementation of HalHandlerForBus
 */
static PBUS_HANDLER STUBAPI HalpHandlerForBus(
    INTERFACE_TYPE BusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG Version,
    PVOID Context,
    PBUS_HANDLER ParentHandler
)
{
    /* Stub - return NULL */
    return NULL;
}

/*
 * Stub implementation of HalHandlerForConfigSpace
 */
static PACCESS_BUS_HANDLER STUBAPI HalpHandlerForConfigSpace(
    ACCESS_BUS_TYPE AccessBusType,
    BUS_DATA_TYPE ConfigurationSpace,
    ULONG Version,
    PVOID Context
)
{
    /* Stub - return NULL */
    return NULL;
}

/*
 * Stub implementation of HalLocateHiberRanges
 */
static VOID STUBAPI HalpLocateHiberRanges(
    PVOID HiberContext
)
{
    /* Stub - do nothing */
    UNREFERENCED_PARAMETER(HiberContext);
}

/*
 * Stub implementation of HalSetWakeEnable
 */
static BOOLEAN STUBAPI HalpSetWakeEnable(
    BOOLEAN Enable
)
{
    /* Stub - always return FALSE */
    UNREFERENCED_PARAMETER(Enable);
    return FALSE;
}

/*
 * Stub implementation of HalSetWakeAlarm
 */
static NTSTATUS STUBAPI HalpSetWakeAlarm(
    ULONGLONG AlarmTime,
    BOOLEAN SystemTime
)
{
    /* Stub - return unsupported */
    UNREFERENCED_PARAMETER(AlarmTime);
    UNREFERENCED_PARAMETER(SystemTime);
    return STATUS_NOT_SUPPORTED;
}

/*
 * Stub implementation of HalHaltSystem
 */
static VOID STUBAPI HalpHaltSystem(
    VOID
)
{
    /* Stub - halt in infinite loop */
    for (;;) {
        __halt();
    }
}

/*
 * Stub implementation of HalResetDisplay
 */
static VOID STUBAPI HalpResetDisplay(
    VOID
)
{
    /* Stub - do nothing */
}

/*
 * Stub implementation of HalAllocateMapRegisters
 */
static NTSTATUS STUBAPI HalpAllocateMapRegisters(
    PADAPTER_OBJECT AdapterObject,
    ULONG BasePage,
    ULONG NumberOfMapRegisters,
    PMAP_REGISTER_ENTRY MapRegisters
)
{
    /* Stub - return success but don't allocate anything */
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(BasePage);
    UNREFERENCED_PARAMETER(NumberOfMapRegisters);
    UNREFERENCED_PARAMETER(MapRegisters);
    return STATUS_SUCCESS;
}

/*
 * Stub implementation of KdSetupPciDeviceForDebugging
 */
static PKDPC STUBAPI HalpKdSetupPciDeviceForDebugging(
    PPCI_COMMON_CONFIG PciData,
    PKDPC Dpc
)
{
    /* Stub - return NULL */
    UNREFERENCED_PARAMETER(PciData);
    UNREFERENCED_PARAMETER(Dpc);
    return NULL;
}

/*
 * Stub implementation of KdReleasePciDeviceForDebugging
 */
static VOID STUBAPI HalpKdReleasePciDeviceForDebugging(
    PKDPC Dpc
)
{
    /* Stub - do nothing */
    UNREFERENCED_PARAMETER(Dpc);
}

/*
 * Stub implementation of KdGetAcpiTablePhase0
 */
static PCHAR STUBAPI HalpKdGetAcpiTablePhase0(
    PKDDEBUGGER_DATA64 DebuggerDataBlock
)
{
    /* Stub - return NULL */
    UNREFERENCED_PARAMETER(DebuggerDataBlock);
    return NULL;
}

/*
 * Stub implementation of KdCheckPowerButton
 */
static BOOLEAN STUBAPI HalpKdCheckPowerButton(
    VOID
)
{
    /* Stub - always return FALSE */
    return FALSE;
}

/*
 * Stub implementation of HalVectorToIDTEntry
 */
static UCHAR STUBAPI HalpVectorToIDTEntry(
    KINTERRUPT_VECTOR Vector
)
{
    /* Stub - return vector as-is */
    return (UCHAR)Vector;
}

/*
 * Stub implementation of KdMapPhysicalMemory64
 */
static PVOID STUBAPI HalpKdMapPhysicalMemory64(
    PHYSICAL_ADDRESS PhysicalAddress,
    ULONG Length,
    BOOLEAN CacheEnabled
)
{
    /* Stub - return NULL */
    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(CacheEnabled);
    return NULL;
}

/*
 * Stub implementation of KdUnmapVirtualAddress
 */
static VOID STUBAPI HalpKdUnmapVirtualAddress(
    PVOID VirtualAddress,
    ULONG Length
)
{
    /* Stub - do nothing */
    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Length);
}

/*
 * HAL Private Dispatch Table instance
 * This is the main export that replaces the HAL DLL dependency
 */
HAL_PRIVATE_DISPATCH HalPrivateDispatchTable = {
    HAL_PRIVATE_DISPATCH_VERSION,
    HalpRegisterBusHandler,           /* HalRegisterBusHandler */
    HalpHandlerForBus,                /* HalHandlerForBus */
    HalpHandlerForConfigSpace,        /* HalHandlerForConfigSpace */
    HalpLocateHiberRanges,            /* HalLocateHiberRanges */
    HalpSetWakeEnable,                /* HalSetWakeEnable */
    HalpSetWakeAlarm,                 /* HalSetWakeAlarm */
    HalpHaltSystem,                   /* HalHaltSystem */
    HalpResetDisplay,                 /* HalResetDisplay */
    HalpAllocateMapRegisters,         /* HalAllocateMapRegisters */
    HalpKdSetupPciDeviceForDebugging, /* KdSetupPciDeviceForDebugging */
    HalpKdReleasePciDeviceForDebugging, /* KdReleasePciDeviceForDebugging */
    HalpKdGetAcpiTablePhase0,         /* KdGetAcpiTablePhase0 */
    HalpKdCheckPowerButton,           /* KdCheckPowerButton */
    HalpVectorToIDTEntry,             /* HalVectorToIDTEntry */
    HalpKdMapPhysicalMemory64,        /* KdMapPhysicalMemory64 */
    HalpKdUnmapVirtualAddress         /* KdUnmapVirtualAddress */
};

/*
 * Additional common HAL stub functions
 */

/* Stub for HalQuerySystemInformation */
NTSTATUS
HalQuerySystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    IN ULONG SystemInformationLength,
    OUT PVOID SystemInformation,
    OUT PULONG ReturnLength OPTIONAL
)
{
    UNREFERENCED_PARAMETER(SystemInformationClass);
    UNREFERENCED_PARAMETER(SystemInformationLength);
    UNREFERENCED_PARAMETER(SystemInformation);
    UNREFERENCED_PARAMETER(ReturnLength);
    return STATUS_NOT_IMPLEMENTED;
}

/* Stub for HalSetSystemInformation */
NTSTATUS
HalSetSystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    IN ULONG SystemInformationLength,
    IN PVOID SystemInformation
)
{
    UNREFERENCED_PARAMETER(SystemInformationClass);
    UNREFERENCED_PARAMETER(SystemInformationLength);
    UNREFERENCED_PARAMETER(SystemInformation);
    return STATUS_NOT_IMPLEMENTED;
}

/* Stub for HalInitSystem */
BOOLEAN
HalInitSystem(
    IN ULONG DriverObject,
    IN PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
    return TRUE;
}

/* Stub for HalHalt */
VOID
HalHalt(
    VOID
)
{
    for (;;) {
        __halt();
    }
}

/* Stub for HalTranslateBusAddress */
BOOLEAN
HalTranslateBusAddress(
    IN INTERFACE_TYPE InterfaceType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS BusAddress,
    IN OUT PULONG AddressSpace,
    OUT PPHYSICAL_ADDRESS TranslatedAddress
)
{
    UNREFERENCED_PARAMETER(InterfaceType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(BusAddress);
    UNREFERENCED_PARAMETER(AddressSpace);
    UNREFERENCED_PARAMETER(TranslatedAddress);
    return FALSE;
}

/* Stub for HalGetBusDataByOffset */
ULONG
HalGetBusDataByOffset(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
)
{
    UNREFERENCED_PARAMETER(BusDataType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(SlotNumber);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Offset);
    UNREFERENCED_PARAMETER(Length);
    return 0;
}

/* Stub for HalSetBusDataByOffset */
ULONG
HalSetBusDataByOffset(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
)
{
    UNREFERENCED_PARAMETER(BusDataType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(SlotNumber);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Offset);
    UNREFERENCED_PARAMETER(Length);
    return 0;
}

/* Stub for HalGetAdapter */
PADAPTER_OBJECT
HalGetAdapter(
    IN PDEVICE_OBJECT DeviceObject,
    IN PMAST_ADAPTER MasterAdapter
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(MasterAdapter);
    return NULL;
}

/* Stub for HalPutAdapter */
VOID
HalPutAdapter(
    IN PADAPTER_OBJECT AdapterObject
)
{
    UNREFERENCED_PARAMETER(AdapterObject);
}

/* Stub for HalAllocateCommonBuffer */
PVOID
HalAllocateCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    OUT PPHYSICAL_ADDRESS LogicalAddress,
    IN BOOLEAN CacheEnabled
)
{
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LogicalAddress);
    UNREFERENCED_PARAMETER(CacheEnabled);
    return NULL;
}

/* Stub for HalFreeCommonBuffer */
VOID
HalFreeCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    IN PHYSICAL_ADDRESS LogicalAddress,
    IN PVOID VirtualAddress,
    IN BOOLEAN CacheEnabled
)
{
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LogicalAddress);
    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(CacheEnabled);
}

/* Stub for HalFlushIoBuffers */
VOID
HalFlushIoBuffers(
    IN PADAPTER_OBJECT AdapterObject,
    IN PVOID VirtualAddress,
    IN ULONG Length,
    IN PHYSICAL_ADDRESS LogicalAddress,
    IN BOOLEAN WriteToDevice
)
{
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LogicalAddress);
    UNREFERENCED_PARAMETER(WriteToDevice);
}

/* Stub for HalGetInterruptVector */
ULONG
HalGetInterruptVector(
    IN INTERFACE_TYPE InterfaceType,
    IN ULONG BusNumber,
    IN ULONG BusInterruptLevel,
    IN ULONG BusInterruptVector,
    OUT PKIRQL Irql,
    OUT PKAFFINITY Affinity
)
{
    UNREFERENCED_PARAMETER(InterfaceType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(BusInterruptLevel);
    UNREFERENCED_PARAMETER(BusInterruptVector);
    UNREFERENCED_PARAMETER(Irql);
    UNREFERENCED_PARAMETER(Affinity);
    return 0;
}

/* Stub for HalEnableSystemInterrupt */
BOOLEAN
HalEnableSystemInterrupt(
    IN ULONG BusInterruptLevel,
    IN KINTERRUPT_MODE InterruptMode,
    IN KAFFINITY TargetProcessor
)
{
    UNREFERENCED_PARAMETER(BusInterruptLevel);
    UNREFERENCED_PARAMETER(InterruptMode);
    UNREFERENCED_PARAMETER(TargetProcessor);
    return FALSE;
}

/* Stub for HalDisableSystemInterrupt */
VOID
HalDisableSystemInterrupt(
    IN ULONG BusInterruptLevel,
    IN KAFFINITY TargetProcessor
)
{
    UNREFERENCED_PARAMETER(BusInterruptLevel);
    UNREFERENCED_PARAMETER(TargetProcessor);
}

/* Stub for HalRequestSoftwareInterrupt */
VOID
HalRequestSoftwareInterrupt(
    IN KIRQL SoftwareInterruptRequested
)
{
    UNREFERENCED_PARAMETER(SoftwareInterruptRequested);
}

/* Stub for HalClearSoftwareInterrupt */
VOID
HalClearSoftwareInterrupt(
    IN KIRQL SoftwareInterruptToClear
)
{
    UNREFERENCED_PARAMETER(SoftwareInterruptToClear);
}

/* Stub for HalRaiseIrql */
KIRQL
HalRaiseIrql(
    IN KIRQL NewIrql
)
{
    UNREFERENCED_PARAMETER(NewIrql);
    return 0;
}

/* Stub for HalLowerIrql */
VOID
HalLowerIrql(
    IN KIRQL NewIrql
)
{
    UNREFERENCED_PARAMETER(NewIrql);
}

/* Stub for HalStallExecutionProcessor */
VOID
HalStallExecutionProcessor(
    IN ULONG Processor,
    IN ULONG MicroSeconds
)
{
    UNREFERENCED_PARAMETER(Processor);
    UNREFERENCED_PARAMETER(MicroSeconds);
}

/* Stub for HalQueryRealTimeClock */
NTSTATUS
HalQueryRealTimeClock(
    IN PRTCTIME Time
)
{
    UNREFERENCED_PARAMETER(Time);
    return STATUS_NOT_IMPLEMENTED;
}

/* Stub for HalSetRealTimeClock */
NTSTATUS
HalSetRealTimeClock(
    IN PRTCTIME Time
)
{
    UNREFERENCED_PARAMETER(Time);
    return STATUS_NOT_IMPLEMENTED;
}

/* Stub for HalQueryBusInformation */
NTSTATUS
HalQueryBusInformation(
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    OUT PVOID BusInformationBuffer,
    IN OUT PULONG BusInformationBufferLength,
    OUT PULONG RequiredLength
)
{
    UNREFERENCED_PARAMETER(BusType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(BusInformationBuffer);
    UNREFERENCED_PARAMETER(BusInformationBufferLength);
    UNREFERENCED_PARAMETER(RequiredLength);
    return STATUS_NOT_IMPLEMENTED;
}
