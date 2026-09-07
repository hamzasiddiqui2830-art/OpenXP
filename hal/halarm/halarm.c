/*
 * PROJECT:         OpenXP HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         ARM Hardware Abstraction Layer Main Entry
 */

#include <hal.h>
#include <armddk.h>

/* Global variables */
PHYSICAL_ADDRESS MmHighestPhysicalPage = {0};
ULONG KeNumberProcessors = 1;

/* Forward declarations */
BOOLEAN
NTAPI
HalpInitializeInterrupts(VOID);

BOOLEAN
NTAPI
HalpInitializeTimer(VOID);

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalInitSystem(
    ULONG BootPhase,
    PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    /* Only initialize during phase 1 */
    if (BootPhase != 1)
        return TRUE;

    /* Initialize interrupt controller */
    if (!HalpInitializeInterrupts())
        return FALSE;

    /* Initialize system timer */
    if (!HalpInitializeTimer())
        return FALSE;

    return TRUE;
}

/*
 * @implemented
 */
VOID
NTAPI
HalBeginSystemInterrupt(
    KIRQL Irql,
    ULONG Vector,
    PKIRQL OldIrql)
{
    /* Raise IRQL */
    *OldIrql = KeGetCurrentIrql();
    KeRaiseIrql(Irql, &KeGetCurrentIrql());
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalEndSystemInterrupt(
    KIRQL Irql,
    ULONG Vector)
{
    /* Lower IRQL */
    KeLowerIrql(Irql);
    return FALSE;
}

/*
 * @implemented
 */
VOID
NTAPI
HalDisableSystemInterrupt(
    ULONG Vector,
    KIRQL Irql)
{
    /* TODO: Implement interrupt masking */
}

/*
 * @implemented
 */
VOID
NTAPI
HalEnableSystemInterrupt(
    ULONG Vector,
    KIRQL Irql,
    KINTERRUPT_MODE InterruptMode)
{
    /* TODO: Implement interrupt unmasking */
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
HalQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength)
{
    /* Not implemented for now */
    return STATUS_NOT_IMPLEMENTED;
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
HalSetSystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength)
{
    /* Not implemented for now */
    return STATUS_NOT_IMPLEMENTED;
}
