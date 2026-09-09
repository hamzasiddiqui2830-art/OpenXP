//
// Copyright (c) OpenXP Team 2026.
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//

#include <nt.h>
#include <ntrtl.h>
#include <stdio.h>
#include <stdarg.h>

#define RtlIpv4AddressToStringT RtlIpv4AddressToStringW
#define RtlIpv6AddressToStringT RtlIpv6AddressToStringW
#define RtlIpv4AddressToStringExT RtlIpv4AddressToStringExW
#define RtlIpv6AddressToStringExT RtlIpv6AddressToStringExW

#define LPTSTR PWSTR
#define TCHAR WCHAR
#define _T(x) L##x

static int
OpenXpStprintfW(
    PWSTR Buffer,
    PCWSTR Format,
    ...
    )
{
    va_list Args;
    int Result;

    va_start(Args, Format);
    Result = _vsnwprintf(Buffer, INET6_ADDRSTRLEN, Format, Args);
    va_end(Args);
    return Result;
}

#define _stprintf OpenXpStprintfW

#include "add2strt.h"
