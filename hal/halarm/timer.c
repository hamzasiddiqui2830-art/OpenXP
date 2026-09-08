/*
 * PROJECT:         OpenXP HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         ARM Timer Support
 */

#include <hal.h>
#include <armddk.h>

/* ARM Generic Timer Register Offsets */
#define CNTFRQ          0x000   /* Counter Frequency */
#define CNTP_TVAL       0x020   /* Physical Timer Value */
#define CNTP_CTL        0x024   /* Physical Timer Control */
#define CNTP_CVAL       0x028   /* Physical Timer Compare Value */

#define TIMER_CTRL_ENABLE     (1 << 0)
#define TIMER_CTRL_IMASK      (1 << 1)
#define TIMER_CTRL_ISTATUS    (1 << 2)

typedef struct _ARM_TIMER_REGS {
    volatile ULONG FRQ;
    volatile ULONG Reserved[7];
    volatile ULONG TVAL;
    volatile ULONG CTL;
    volatile ULONG CVAL;
} ARM_TIMER_REGS, *PARM_TIMER_REGS;

static PARM_TIMER_REGS ArmTimer = NULL;
static ULONG TimerFrequency = 0;

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalpInitializeTimer(VOID)
{
    /* Map timer registers (platform-specific) */
    /* For now, assume mapped at fixed address */
    ArmTimer = (PARM_TIMER_REGS)0x101C0000UL;

    /* Read timer frequency */
    TimerFrequency = ArmTimer->FRQ;

    if (TimerFrequency == 0)
        return FALSE;

    /* Disable timer initially */
    ArmTimer->CTL &= ~TIMER_CTRL_ENABLE;

    return TRUE;
}

/*
 * @implemented
 */
ULONG
NTAPI
HalQueryCounterFrequency(VOID)
{
    return TimerFrequency;
}

/*
 * @implemented
 */
ULONGLONG
NTAPI
HalQueryPerformanceCounter(VOID)
{
    ULONGLONG count;

    /* Read 64-bit system counter */
    __asm__ volatile ("mrrc p15, 0, %Q0, %R0, c14" : "=r" (count));

    return count;
}

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalSetProfileInterval(
    ULONG ProfileInterval)
{
    /* Set timer reload value */
    if (ArmTimer && TimerFrequency > 0) {
        ULONG ticks = (TimerFrequency * ProfileInterval) / 10000000UL;
        ArmTimer->TVAL = ticks;
        return TRUE;
    }

    return FALSE;
}

/*
 * @implemented
 */
VOID
NTAPI
HalStartProfileInterrupt(
    KPROFILE_SOURCE ProfileSource)
{
    UNREFERENCED_PARAMETER(ProfileSource);

    /* Enable the timer interrupt */
    if (ArmTimer) {
        ArmTimer->CTL |= TIMER_CTRL_ENABLE;
    }
}

/*
 * @implemented
 */
VOID
NTAPI
HalStopProfileInterrupt(VOID)
{
    /* Disable the timer interrupt */
    if (ArmTimer) {
        ArmTimer->CTL &= ~TIMER_CTRL_ENABLE;
    }
}
