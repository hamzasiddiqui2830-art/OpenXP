/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    Sep.c

Abstract:

    This Module implements the private security routine that are defined
    in sep.h

--*/

#include "pch.h"

#pragma hdrstop


#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,SepCheckAcl)
#endif



BOOLEAN
SepCheckAcl (
    IN PACL Acl,
    IN ULONG Length
    )

/*++

Routine Description:

    This is a private routine that checks that an acl is well formed.

Arguments:

    Acl - Supplies the acl to check

    Length - Supplies the real size of the acl.  The internal acl size
        must agree.

Return Value:

    BOOLEAN - TRUE if the acl is well formed and FALSE otherwise

--*/
{
    if ((Length < sizeof(ACL)) || (Length != Acl->AclSize)) {
        return FALSE;
    }
    return RtlValidAcl( Acl );
}

