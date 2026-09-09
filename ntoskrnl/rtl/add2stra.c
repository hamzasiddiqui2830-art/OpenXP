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

#define RtlIpv6AddressToStringT RtlIpv6AddressToStringA
#define RtlIpv4AddressToStringT RtlIpv4AddressToStringA
#define RtlIpv6AddressToStringExT RtlIpv6AddressToStringExA
#define RtlIpv4AddressToStringExT RtlIpv4AddressToStringExA

#define LPTSTR PSTR
#define TCHAR CHAR
#define _T(x) x
#define INET6_ADDRSTRLEN 65

static int
OpenXpStprintfA(
    PSTR Buffer,
    PCSTR Format,
    ...
    )
{
    va_list Args;
    int Result;

    va_start(Args, Format);
    Result = _vsnprintf(Buffer, INET6_ADDRSTRLEN, Format, Args);
    va_end(Args);
    return Result;
}

#define _stprintf OpenXpStprintfA

#include "add2strt.h"
