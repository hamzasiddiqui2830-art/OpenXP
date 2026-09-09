/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    heappriv.h

Abstract:

    Private include file used by heap allocator (heap.c, heapdll.c and
    heapdbg.c)

Author:

    Steve Wood (stevewo) 25-Oct-1994

Revision History:

--*/

#ifndef _RTL_HEAP_PRIVATE_
#define _RTL_HEAP_PRIVATE_

#include "heap.h"
#include "heappage.h"

//
//  In private builds (PRERELEASE = 1) we allow using the new low fragmentation heap
//  for processes that set the DisableLookaside registry key. The main purpose is to
//  allow testing the new heap API.
//

#ifndef PRERELEASE

#define DISABLE_REGISTRY_TEST_HOOKS

#endif

//
//  Disable FPO optimization so even retail builds get somewhat reasonable
//  stack backtraces
//

#if i386
// #pragma optimize("y",off)
#endif

#if DBG
#define HEAPASSERT(exp) if (!(exp)) RtlAssert( #exp, __FILE__, __LINE__, NULL )
#else
#define HEAPASSERT(exp)
#endif

//
// Define Minimum lookaside list depth.
//

#define MINIMUM_LOOKASIDE_DEPTH 4

//
//  This variable contains the fill pattern used for heap tail checking
//

extern const UCHAR CheckHeapFillPattern[ CHECK_HEAP_TAIL_SIZE ];

