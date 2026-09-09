/*++ BUILD Version: 0003

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    ntosdef.h

Abstract:

    Compatibility wrapper for the complete NTOS private definitions.

--*/

#ifndef _OPENXP_NTOSDEF_WRAPPER_
#define _OPENXP_NTOSDEF_WRAPPER_

/*
 * Keep the active kernel include tree backed by the complete local WRK
 * definitions. The legacy copy is repository-local and is not a host SDK
 * dependency.
 *
 * The WRK dispatcher header stores the timer-table hand in the same byte as
 * Size. Older WRK private code refers to that byte as Hand, so provide the
 * compatibility spelling while the header is consumed.
 */
#define Hand Size
#include "../../ntos-old/inc/ntosdef.h"

#endif /* _OPENXP_NTOSDEF_WRAPPER_ */
