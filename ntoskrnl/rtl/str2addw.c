//
// Copyright (c) OpenXP Team 2026.
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//

// Without this define, link errors can occur due to missing _pctype and
// __mb_cur_max
//
#define _CTYPE_DISABLE_MACROS

#define UNICODE
#define _UNICODE

#include <nt.h>
#include <ntrtl.h>
#include <stdlib.h>
#include <tchar.h>

// NT headers historically define WCHAR/TCHAR as unsigned-short based types.
// Modern MSVC keeps native wchar_t distinct unless /Zc:wchar_t- is used,
// while the W RTL prototypes use native wchar_t.  Keep this translation unit
// type-correct without changing the ABI-facing NT header definitions.
#undef TCHAR
#define TCHAR wchar_t
#undef LPCTSTR
#define LPCTSTR const wchar_t *
#undef LPTSTR
#define LPTSTR wchar_t *

#define RtlIpv4StringToAddressT RtlIpv4StringToAddressW
#define RtlIpv6StringToAddressT RtlIpv6StringToAddressW
#define RtlIpv4StringToAddressExT RtlIpv4StringToAddressExW
#define RtlIpv6StringToAddressExT RtlIpv6StringToAddressExW

#include "str2addt.h"

