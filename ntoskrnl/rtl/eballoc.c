/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    eballoc.c

Abstract:

    Process/Thread Environment Block allocation functions

--*/

#include "ntrtlp.h"
#include <nturtl.h>

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(INIT,RtlAcquirePebLock)
#pragma alloc_text(INIT,RtlReleasePebLock)
#endif


#undef RtlAcquirePebLock

VOID
RtlAcquirePebLock( VOID )
{
}


#undef RtlReleasePebLock

VOID
RtlReleasePebLock( VOID )
{
}

#if DBG
VOID
RtlAssertPebLockOwned( VOID )
{
}
#endif

