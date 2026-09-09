/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    intrloc2.c

Abstract:

   This module implements the *portable*  (i.e. SLOW) versions of
   the executive's simple atomic increment/decrement procedures.
   Real implementation should be in assembler.

Author:

    Bryan Willman  (bryanwi)  2-Aug-90

Environment:

    Kernel mode only.

Revision History:

--*/

#include "exp.h"

INTERLOCKED_RESULT
Exfi386InterlockedIncrementLong (
    IN PLONG Addend
    )
{
    LONG OldValue;

    OldValue = InterlockedIncrement(Addend);

    if (OldValue < 0)
        return ResultNegative;
    if (OldValue == 0)
        return ResultZero;
    return ResultPositive;
}

INTERLOCKED_RESULT
Exfi386InterlockedDecrementLong (
    IN PLONG Addend
    )
{
    LONG OldValue;

    OldValue = InterlockedDecrement(Addend);

    if (OldValue < 0)
        return ResultNegative;
    if (OldValue == 0)
        return ResultZero;
    return ResultPositive;
}

INTERLOCKED_RESULT
ExInterlockedIncrementLong (
    IN PLONG Addend,
    IN PKSPIN_LOCK Lock
    )
{
    LONG OldValue;

    OldValue = (LONG)ExInterlockedAddUlong((PULONG)Addend, 1, Lock);

    if (OldValue < -1)
        return ResultNegative;
    if (OldValue == -1)
        return ResultZero;
    return ResultPositive;
}

INTERLOCKED_RESULT
ExInterlockedDecrementLong (
    IN PLONG Addend,
    IN PKSPIN_LOCK Lock
    )
{
    LONG OldValue;

    OldValue = (LONG)ExInterlockedAddUlong((PULONG)Addend, (ULONG)-1, Lock);

    if (OldValue > 1)
        return ResultPositive;
    if (OldValue == 1)
        return ResultZero;
    return ResultNegative;
}
