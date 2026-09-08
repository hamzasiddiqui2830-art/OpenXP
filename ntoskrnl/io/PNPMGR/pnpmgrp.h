/*
 * ReactOS Kernel
 * Copyright (C) 2024 ReactOS Team
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _PNPMGRP_
#define _PNPMGRP_

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4324)   // alignment sensitive to declspec
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4706)   // assignment within conditional expression

#include "ntos.h"
#include "zwapi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windef.h"
#include "winerror.h"

#include "strsafe.h"

#include "iopcmn.h"

#include "ppmacro.h"
#include "ppdebug.h"
#include "pnpi.h"
#include "arbiter.h"
#include "dockintf.h"
#include "pnprlist.h"

#include "ioverifier.h"
#include "iofileutil.h"
#include "pnpiop.h"
#include "pphotswap.h"
#include "ppprofile.h"
#include "pphandle.h"
#include "ppvutil.h"
#include "ppdrvdb.h"
#include "ppcddb.h"

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'  pP')
#undef ExAllocatePoolWithQuota
#define ExAllocatePoolWithQuota(a,b) ExAllocatePoolWithQuotaTag(a,b,'  pP')
#endif


#endif // _PNPMGRP_
