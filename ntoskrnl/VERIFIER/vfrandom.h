/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfrandom.h

Abstract:

    This header exposes support for random number generation as needed by the
    verifier.

--*/

VOID
VfRandomInit(
    VOID
    );

ULONG
FASTCALL
VfRandomGetNumber(
    IN  ULONG   Minimum,
    IN  ULONG   Maximum
    );

