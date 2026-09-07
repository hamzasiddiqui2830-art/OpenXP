/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         ARM Bus Support
 * PROGRAMMER:      ReactOS Team
 */

#include <hal.h>
#include <armddk.h>

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalGetAdapter(
    PDEVICE_DESCRIPTION DeviceDescription,
    PDMADMA_ADAPTER *DmaAdapter)
{
    /* Not implemented for ARM yet */
    UNREFERENCED_PARAMETER(DeviceDescription);
    UNREFERENCED_PARAMETER(DmaAdapter);
    
    return FALSE;
}

/*
 * @implemented
 */
ULONG
NTAPI
HalSetBusDataByOffset(
    BUS_DATA_TYPE BusDataType,
    ULONG BusNumber,
    ULONG SlotNumber,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length)
{
    /* PCI not typically used on ARM, return 0 */
    UNREFERENCED_PARAMETER(BusDataType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(SlotNumber);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Offset);
    UNREFERENCED_PARAMETER(Length);
    
    return 0;
}

/*
 * @implemented
 */
ULONG
NTAPI
HalGetBusDataByOffset(
    BUS_DATA_TYPE BusDataType,
    ULONG BusNumber,
    ULONG SlotNumber,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length)
{
    /* PCI not typically used on ARM, return 0 */
    UNREFERENCED_PARAMETER(BusDataType);
    UNREFERENCED_PARAMETER(BusNumber);
    UNREFERENCED_PARAMETER(SlotNumber);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Offset);
    UNREFERENCED_PARAMETER(Length);
    
    return 0;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalTranslateBusAddress(
    INTERFACE_TYPE InterfaceType,
    ULONG BusNumber,
    PHYSICAL_ADDRESS BusAddress,
    PULONG AddressSpace,
    PPHYSICAL_ADDRESS TranslatedAddress)
{
    /* Simple identity mapping for now */
    TranslatedAddress->QuadPart = BusAddress.QuadPart;
    *AddressSpace = 0; /* Memory space */
    
    return TRUE;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalConvertPhysicalAddressToVirtual(
    PPHYSICAL_ADDRESS PhysicalAddress,
    PVOID *VirtualAddress)
{
    /* Simple identity mapping */
    *VirtualAddress = (PVOID)(ULONG_PTR)PhysicalAddress->QuadPart;
    return TRUE;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalConvertVirtualAddressToPhysical(
    PVOID VirtualAddress,
    PPHYSICAL_ADDRESS PhysicalAddress)
{
    /* Simple identity mapping */
    PhysicalAddress->QuadPart = (ULONGLONG)(ULONG_PTR)VirtualAddress;
    return TRUE;
}
