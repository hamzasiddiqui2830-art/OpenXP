/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    Version.c

Abstract:

    This module implements a function to compare OS versions. Its the basis for
    VerifyVersionInfoW API. The Rtl version can be called from device drivers.

Author:

    Nar Ganapathy     [Narg]    19-Oct-1998

Environment:

    Pure utility routine

Revision History:

--*/

#include <stdio.h>
#include <ntrtlp.h>
#if !defined(NTOS_KERNEL_RUNTIME)
#include <winerror.h>
#endif

#if defined(ALLOC_PRAGMA) && defined(NTOS_KERNEL_RUNTIME)
#pragma alloc_text(PAGE, RtlGetVersion)
#endif

//
// The following comment explains the old and the new style layouts for the
// condition masks. The condition mask is passed as a parameter to the
// VerifyVersionInfo API. The condition mask encodes conditions like VER_AND,
// VER_OR, VER_EQUAL for various types like VER_PLATFORMID, VER_MINORVERSION
// etc., When the API was originally designed the application used a macro
// called VER_SET_CONDTION which was defined to be  _m_=(_m_|(_c_<<(1<<_t_))).
// where _c_ is the condition and _t_ is the type. This macro is buggy for
// types >= VER_PLATFORMID. Unfortunately a lot of application code already
// uses this buggy macro (notably this terminal server) and have been shipped.
// To fix this bug, a new API VerSetConditionMask is defined which has a new
// bit layout. To provide backwards compatibility, we need to know if a
// specific condition mask is a new style mask (has the new bit layout) or is an
// old style mask. In both bit layouts bit 64 can never be set.
// So the new API sets this bit to indicate that the condition mask is a new
// style condition mask. So the code in this function that extracts the
// condition uses the new bit layout if bit 63 is set and the old layout if
// bit 63 is not set. This should allow applications that was compiled with the
// old macro to work.
//

//
// Use bit 63 to indicate that the new style bit layout is followed.
//
#define NEW_STYLE_BIT_MASK              0x8000000000000000


//
// Condition extractor for the old style mask.
//
// The historical macro used (1 << type) as the bit offset. For the higher
// version types that offset is 64 or greater, so the original expression is
// undefined and MSVC correctly diagnoses it as C4293. Those old-style masks
// cannot encode such conditions in a 64-bit value; return no condition rather
// than performing an invalid shift. Lower types retain the original layout.
//
static ULONG
RtlpVerGetOldCondition(
    ULONGLONG ConditionMask,
    ULONG TypeMask
    )
{
    ULONG Shift;

    if (TypeMask >= 6) {
        return 0;
    }

    Shift = 1UL << TypeMask;
    return (ULONG)((ConditionMask >> Shift) & 0xFFULL);
}

#define OLD_CONDITION(_m_,_t_)  RtlpVerGetOldCondition((_m_),(_t_))

//
// Test to see  if the mask is an old style mask.
//
#define OLD_STYLE_CONDITION_MASK(_m_)  (((_m_) & NEW_STYLE_BIT_MASK)  == 0)

#define RTL_GET_CONDITION(_m_, _t_) \
        (OLD_STYLE_CONDITION_MASK(_m_) ? (OLD_CONDITION(_m_,_t_)) : \
                RtlpVerGetConditionMask((_m_), (_t_)))

#define LEXICAL_COMPARISON        1     /* Do string comparison. Used for minor numbers */
#define MAX_STRING_LENGTH         20    /* Maximum number of digits for sprintf */

ULONG
RtlpVerGetConditionMask(
        ULONGLONG       ConditionMask,
        ULONG   TypeMask
        );


/*++

Routine Description:

    This function retrieves the OS version information. Its the kernel equivalent of
    the GetVersionExW win 32 API.

Arguments:

    lpVersionInformation - Supplies a pointer to the version info structure.
        In the kernel always assume that the structure is of type
        PRTL_OSVERSIONINFOEXW as its not exported to drivers. The signature
        is kept the same as for the user level RtlGetVersion.

Return Value:

    Always succeeds and returns STATUS_SUCCESS.
--*/
#if defined(NTOS_KERNEL_RUNTIME)
NTSTATUS
RtlGetVersion (
    OUT  PRTL_OSVERSIONINFOW lpVersionInformation
    )
{
        NT_PRODUCT_TYPE NtProductType;
    RTL_PAGED_CODE();

    lpVersionInformation->dwMajorVersion = NtMajorVersion;
    lpVersionInformation->dwMinorVersion = NtMinorVersion;
    lpVersionInformation->dwBuildNumber = (USHORT)(NtBuildNumber & 0x3FFF);
    lpVersionInformation->dwPlatformId  = 2; // VER_PLATFORM_WIN32_NT from winbase.h
    if (lpVersionInformation->dwOSVersionInfoSize == sizeof( RTL_OSVERSIONINFOEXW )) {
        ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor = ((USHORT)CmNtCSDVersion >> 8) & (0xFF);
        ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wServicePackMinor = (USHORT)CmNtCSDVersion & 0xFF;
        ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask = (USHORT)(USER_SHARED_DATA->SuiteMask&0xffff);
        ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wProductType = (RtlGetNtProductType(&NtProductType) ? NtProductType :0);

        /* Not set as its not needed by VerifyVersionInfoW */
        ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wReserved = (UCHAR)0;
    }

    return STATUS_SUCCESS;
}
#else
NTSTATUS
RtlGetVersion(
    OUT  PRTL_OSVERSIONINFOW lpVersionInformation
    )
{
    PPEB Peb;
    NT_PRODUCT_TYPE NtProductType;

    Peb = NtCurrentPeb();
    lpVersionInformation->dwMajorVersion = Peb->OSMajorVersion;
    lpVersionInformation->dwMinorVersion = Peb->OSMinorVersion;
    lpVersionInformation->dwBuildNumber  = Peb->OSBuildNumber;
    lpVersionInformation->dwPlatformId   = Peb->OSPlatformId;
    if (Peb->CSDVersion.Buffer) {
        wcscpy( lpVersionInformation->szCSDVersion, Peb->CSDVersion.Buffer );
    } else {
        lpVersionInformation->szCSDVersion[0] = 0;
    }

    if (lpVersionInformation->dwOSVersionInfoSize == sizeof( OSVERSIONINFOEXW ))
    {
        ((POSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor = (Peb->OSCSDVersion >> 8) & 0xFF;
        ((POSVERSIONINFOEXW)lpVersionInformation)->wServicePackMinor = Peb->OSCSDVersion & 0xFF;
        ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask = (USHORT)(USER_SHARED_DATA->SuiteMask&0xffff);
        ((POSVERSIONINFOEXW)lpVersionInformation)->wProductType = 0;
        if (RtlGetNtProductType( &NtProductType )) {
            ((POSVERSIONINFOEXW)lpVersionInformation)->wProductType = (UCHAR)NtProductType;
            if (NtProductType == VER_NT_WORKSTATION) {
               //
               // For workstation product never return VER_SUITE_TERMINAL
               //
                ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask = ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask & 0xffef;
            }

        }
    }

    return STATUS_SUCCESS;
}
#endif


BOOLEAN
RtlpVerCompare(
    LONG Condition,
    LONG Value1,
    LONG Value2,
    BOOLEAN *Equal,
    int   Flags
    )
{
    char    String1[MAX_STRING_LENGTH];
    char    String2[MAX_STRING_LENGTH];
    LONG    Comparison;

    if (Flags & LEXICAL_COMPARISON) {
        sprintf(String1, "%d", Value1); 
        sprintf(String2, "%d", Value2);
        Comparison = strcmp(String2, String1);
        Value1 = 0;
        Value2 = Comparison;
    }
    *Equal = (Value1 == Value2);
    switch (Condition) {
        case VER_EQUAL:
            return (Value2 == Value1);

        case VER_GREATER:
            return (Value2 > Value1);

        case VER_LESS:
            return (Value2 < Value1);

        case VER_GREATER_EQUAL:
            return (Value2 >= Value1);

        case VER_LESS_EQUAL:
            return (Value2 <= Value1);

        default:
            break;
    }

    return FALSE;
}



NTSTATUS
RtlVerifyVersionInfo(
    IN PRTL_OSVERSIONINFOEXW VersionInfo,
    IN ULONG TypeMask,
    IN ULONGLONG  ConditionMask
    )

/*+++
    This function verifies a version condition.  Basically, this
    function lets an app query the system to see if the app is
    running on a specific version combination.
