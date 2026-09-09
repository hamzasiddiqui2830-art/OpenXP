/*++

Module Name:

    add2strt.h

Abstract:

    Shared implementation for the WRK IPv4 and IPv6 address-to-string
    RTL routines. This file supplies its own address structures so kernel
    builds do not depend on Winsock or Windows SDK headers.

--*/

#ifndef OPENXP_ADD2STRT_H_INCLUDED
#define OPENXP_ADD2STRT_H_INCLUDED

struct in6_addr {
    union {
        UCHAR Byte[16];
        USHORT Word[8];
    } u;
};
#define s6_bytes u.Byte
#define s6_words u.Word

struct in_addr {
    union {
        struct { UCHAR s_b1, s_b2, s_b3, s_b4; } S_un_b;
        struct { USHORT s_w1, s_w2; } S_un_w;
        ULONG S_addr;
    } S_un;
};
#define s_addr S_un.S_addr

#define AF_INET 2
#define AF_INET6 23
#define INET_ADDRSTRLEN 22
#define INET6_ADDRSTRLEN 65

LPTSTR
NTAPI
RtlIpv6AddressToStringT(
    __in const struct in6_addr *Addr,
    __out_ecount(INET6_ADDRSTRLEN) LPTSTR S
    )
{
    int maxFirst, maxLast;
    int curFirst, curLast;
    int i;
    int endHex = 8;

    if ((Addr->s6_words[0] == 0) && (Addr->s6_words[1] == 0) &&
        (Addr->s6_words[2] == 0) && (Addr->s6_words[3] == 0) &&
        (Addr->s6_words[6] != 0)) {
        if ((Addr->s6_words[4] == 0) &&
            ((Addr->s6_words[5] == 0) || (Addr->s6_words[5] == 0xffff))) {
            S += _stprintf(S, _T("::%hs%u.%u.%u.%u"),
                           Addr->s6_words[5] == 0 ? "" : "ffff:",
                           Addr->s6_bytes[12], Addr->s6_bytes[13],
                           Addr->s6_bytes[14], Addr->s6_bytes[15]);
            return S;
        }
        else if ((Addr->s6_words[4] == 0xffff) && (Addr->s6_words[5] == 0)) {
            S += _stprintf(S, _T("::ffff:0:%u.%u.%u.%u"),
                           Addr->s6_bytes[12], Addr->s6_bytes[13],
                           Addr->s6_bytes[14], Addr->s6_bytes[15]);
            return S;
        }
    }

    maxFirst = maxLast = 0;
    curFirst = curLast = 0;

    if (((Addr->s6_words[4] & 0xfffd) == 0) &&
        (Addr->s6_words[5] == 0xfe5e)) {
        endHex = 6;
    }

    for (i = 0; i < endHex; i++) {
        if (Addr->s6_words[i] == 0) {
            curLast = i + 1;
            if (curLast - curFirst > maxLast - maxFirst) {
                maxFirst = curFirst;
                maxLast = curLast;
            }
        }
        else {
            curFirst = curLast = i + 1;
        }
    }

    if (maxLast - maxFirst <= 1)
        maxFirst = maxLast = 0;

    for (i = 0; i < endHex; i++) {
        if ((maxFirst <= i) && (i < maxLast)) {
            S += _stprintf(S, _T("::"));
            i = maxLast - 1;
            continue;
        }

        if ((i != 0) && (i != maxLast))
            S += _stprintf(S, _T(":"));

        S += _stprintf(S, _T("%x"), RtlUshortByteSwap(Addr->s6_words[i]));
    }

    if (endHex < 8) {
        S += _stprintf(S, _T(":%u.%u.%u.%u"),
                       Addr->s6_bytes[12], Addr->s6_bytes[13],
                       Addr->s6_bytes[14], Addr->s6_bytes[15]);
    }

    return S;
}

NTSTATUS
NTAPI
RtlIpv6AddressToStringExT(
    __in const struct in6_addr *Address,
    __in ULONG ScopeId,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) LPTSTR AddressString,
    __inout PULONG AddressStringLength
    )
{
    TCHAR String[INET6_ADDRSTRLEN];
    LPTSTR S;
    ULONG Length;

    if ((Address == NULL) || (AddressString == NULL) ||
        (AddressStringLength == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    S = String;
    if (Port)
        S += _stprintf(S, _T("["));

    S = RtlIpv6AddressToStringT(Address, S);

    if (ScopeId != 0)
        S += _stprintf(S, _T("%%%u"), ScopeId);

    if (Port != 0)
        S += _stprintf(S, _T("]:%u"), RtlUshortByteSwap(Port));

    Length = (ULONG)(S - String + 1);
    if (*AddressStringLength < Length) {
        *AddressStringLength = Length;
        return STATUS_INVALID_PARAMETER;
    }

    *AddressStringLength = Length;
    RtlCopyMemory(AddressString, String, Length * sizeof(TCHAR));
    return STATUS_SUCCESS;
}

LPTSTR
NTAPI
RtlIpv4AddressToStringT(
    __in const struct in_addr *Addr,
    __out_ecount(16) LPTSTR S
    )
{
    S += _stprintf(S, _T("%u.%u.%u.%u"),
                   (Addr->s_addr >> 0) & 0xFF,
                   (Addr->s_addr >> 8) & 0xFF,
                   (Addr->s_addr >> 16) & 0xFF,
                   (Addr->s_addr >> 24) & 0xFF);
    return S;
}

NTSTATUS
NTAPI
RtlIpv4AddressToStringExT(
    __in const struct in_addr *Address,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) LPTSTR AddressString,
    __inout PULONG AddressStringLength
    )
{
    TCHAR String[INET_ADDRSTRLEN];
    LPTSTR S;
    ULONG Length;

    if ((Address == NULL) || (AddressString == NULL) ||
        (AddressStringLength == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    S = String;
    S = RtlIpv4AddressToStringT(Address, S);

    if (Port != 0)
        S += _stprintf(S, _T(":%u"), RtlUshortByteSwap(Port));

    Length = (ULONG)(S - String + 1);
    if (*AddressStringLength < Length) {
        *AddressStringLength = Length;
        return STATUS_INVALID_PARAMETER;
    }

    RtlCopyMemory(AddressString, String, Length * sizeof(TCHAR));
    *AddressStringLength = Length;
    return STATUS_SUCCESS;
}

#endif /* OPENXP_ADD2STRT_H_INCLUDED */
