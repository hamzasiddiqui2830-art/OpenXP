/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfprint.h

Abstract:

    This header exposes prototypes required when outputting various data types
    to the debugger.

--*/

VOID
VfPrintDumpIrpStack(
    IN PIO_STACK_LOCATION IrpSp
    );

VOID
VfPrintDumpIrp(
    IN PIRP IrpToFlag
    );

