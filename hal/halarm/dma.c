/*
 * PROJECT:         OpenXP HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         ARM DMA Support
 */

#include <hal.h>
#include <armddk.h>

/*
 * @implemented
 */
PVOID
NTAPI
HalAllocateCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    OUT PPHYSICAL_ADDRESS LogicalAddress,
    OUT PBOOLEAN CachedWrite
)
{
    /* Simple allocation for now - platform specific */
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LogicalAddress);
    UNREFERENCED_PARAMETER(CachedWrite);
    
    return NULL;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalFreeCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    IN PHYSICAL_ADDRESS LogicalAddress,
    IN PVOID VirtualAddress,
    IN BOOLEAN CachedWrite
)
{
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LogicalAddress);
    UNREFERENCED_PARAMETER(VirtualAddress);
    UNREFERENCED_PARAMETER(CachedWrite);
    
    return FALSE;
}

/*
 * @implemented
 */
ULONG
NTAPI
HalGetDmaAdapter(
    IN PVOID DeviceObject,
    IN PDEVICE_DESCRIPTION DeviceDescription,
    OUT PULONG NumberOfMapRegisters
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(DeviceDescription);
    UNREFERENCED_PARAMETER(NumberOfMapRegisters);
    
    return 0;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalPutDmaAdapter(
    IN PDMADMA_ADAPTER DmaAdapter
)
{
    UNREFERENCED_PARAMETER(DmaAdapter);
    
    return FALSE;
}

/*
 * @implemented
 */
ULONG
NTAPI
HalMapTransfer(
    IN PDMADMA_ADAPTER DmaAdapter,
    IN PMDL Mdl,
    IN PHYSICAL_ADDRESS MapRegisterBase,
    IN ULONG Length,
    IN KPROCESSOR_MODE ProcessorMode,
    IN BOOLEAN WriteToDma
)
{
    UNREFERENCED_PARAMETER(DmaAdapter);
    UNREFERENCED_PARAMETER(Mdl);
    UNREFERENCED_PARAMETER(MapRegisterBase);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(ProcessorMode);
    UNREFERENCED_PARAMETER(WriteToDma);
    
    return 0;
}
