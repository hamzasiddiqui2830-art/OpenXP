/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfdebug.h

Abstract:

    This header contains debugging macros used by the driver verifier code.

--*/

extern ULONG VfSpewLevel;

#if DBG
#define VERIFIER_DBGPRINT(txt,level) \
{ \
    if (VfSpewLevel>(level)) { \
        DbgPrint##txt; \
    }\
}
#else
#define VERIFIER_DBGPRINT(txt,level)
#endif

