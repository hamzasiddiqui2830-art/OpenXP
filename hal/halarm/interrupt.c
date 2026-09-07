/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         ARM Interrupt Controller Support
 * PROGRAMMER:      ReactOS Team
 */

#include <hal.h>
#include <armddk.h>

/* GIC Base Address (configurable per platform) */
#define GIC_BASE_ADDRESS    0x10140000UL

/* GIC Register Offsets */
#define GICD_CTLR           0x000
#define GICD_TYPER          0x004
#define GICD_IGROUPR        0x080
#define GICD_ISENABLER      0x100
#define GICD_ICENABLER      0x180
#define GICD_IPRIORITYR     0x400
#define GICD_ITARGETSR      0x800
#define GICD_ICFGR          0xC00
#define GICD_SGIR           0xF00

typedef struct _GIC_REGS {
    volatile ULONG CTLR;
    volatile ULONG TYPER;
    /* ... more registers */
} GIC_REGS, *PGIC_REGS;

static PGIC_REGS GicDistributor = (PGIC_REGS)GIC_BASE_ADDRESS;

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalpInitializeInterrupts(VOID)
{
    ULONG i;
    ULONG ItLines;

    /* Read interrupt controller type */
    ItLines = (GicDistributor->TYPER & 0x1F) + 1;

    /* Disable all interrupts initially */
    for (i = 0; i < (ItLines / 32); i++) {
        *((volatile ULONG*)((ULONG_PTR)&GicDistributor->ICENABLER + (i * 4))) = 0xFFFFFFFF;
    }

    /* Set all interrupts to Group 0 (Secure) */
    for (i = 0; i < (ItLines / 32); i++) {
        *((volatile ULONG*)((ULONG_PTR)&GicDistributor->IGROUPR + (i * 4))) = 0x00000000;
    }

    /* Set priority for all interrupts to lowest */
    for (i = 0; i < (ItLines / 4); i++) {
        *((volatile ULONG*)((ULONG_PTR)&GicDistributor->IPRIORITYR + (i * 4))) = 0xFFFFFFFF;
    }

    /* Enable the distributor */
    GicDistributor->CTLR |= 0x01;

    return TRUE;
}

/*
 * @implemented
 */
VOID
NTAPI
HalpAcknowledgeInterrupt(
    ULONG Vector,
    KIRQL Irql)
{
    /* ARM GIC doesn't require explicit ACK for edge-triggered interrupts */
    /* For level-triggered, we need to deassert at the source device */
    UNREFERENCED_PARAMETER(Vector);
    UNREFERENCED_PARAMETER(Irql);
}

/*
 * @implemented
 */
ULONG
NTAPI
HalGetInterruptVector(
    ULONG BusNumber,
    ULONG BusInterruptLevel)
{
    /* On ARM, the vector is typically the same as the IRQ number */
    return BusInterruptLevel;
}

/*
 * @implemented
 */
KIRQL
NTAPI
HalGetInterruptRequestLevel(
    ULONG Vector)
{
    /* Default IRQL mapping for ARM interrupts */
    if (Vector >= 16 && Vector <= 31)
        return DISPATCH_LEVEL;
    else if (Vector > 31)
        return DEVICE_LEVEL;
    
    return PASSIVE_LEVEL;
}
