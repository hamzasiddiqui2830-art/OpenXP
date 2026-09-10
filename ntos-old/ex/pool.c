/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    pool.c

Abstract:

    This module implements the NT executive pool allocator.

Author:

    Mark Lucovsky     16-Feb-1989
    Lou Perazzoli     31-Aug-1991 (change from binary buddy)
    David N. Cutler (davec) 27-May-1994
    Landy Wang        17-Oct-1997

Environment:

    Kernel mode only

Revision History:

--*/

#include "exp.h"

#pragma hdrstop

#undef ExAllocatePoolWithTag
#undef ExAllocatePool
#undef ExAllocatePoolWithQuota
#undef ExAllocatePoolWithQuotaTag
#undef ExFreePool
#undef ExFreePoolWithTag

//
// These bitfield definitions are based on EX_POOL_PRIORITY in inc\ex.h.
//

#define POOL_SPECIAL_POOL_BIT               0x8
#define POOL_SPECIAL_POOL_UNDERRUN_BIT      0x1

// InterlockedExchangeAddSizeT is supplied by the architecture MM header.



//
// Define forward referenced function prototypes.
//

#ifdef ALLOC_PRAGMA
PVOID
ExpAllocateStringRoutine (
    IN SIZE_T NumberOfBytes
    );

VOID
ExDeferredFreePool (
     IN PPOOL_DESCRIPTOR PoolDesc
     );

VOID
ExpSeedHotTags (
    VOID
    );

NTSTATUS
ExGetSessionPoolTagInfo (
    IN PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    IN OUT PULONG ReturnedEntries,
    IN OUT PULONG ActualEntries
    );

NTSTATUS
ExGetPoolTagInfo (
    IN PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    IN OUT PULONG ReturnedEntries,
    IN OUT PULONG ActualEntries
    );

#endif
