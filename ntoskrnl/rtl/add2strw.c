//
// Copyright (c) OpenXP Team 2026.
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//

#include <nt.h>
#include <ntrtl.h>
#include <stdio.h>

#define RtlIpv4AddressToStringT RtlIpv4AddressToStringW
#define RtlIpv6AddressToStringT RtlIpv6AddressToStringW
#define RtlIpv4AddressToStringExT RtlIpv4AddressToStringExW
#define RtlIpv6AddressToStringExT RtlIpv6AddressToStringExW

/* nt.h is force-included, so use explicit wide types rather than LPTSTR. */
#define LPTSTR PWSTR
#define TCHAR WCHAR
#define _T(x) L##x
#define _stprintf _swprintf

#include "add2strt.h"
