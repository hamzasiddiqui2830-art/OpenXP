/*++

Copyright (c) Microsoft Corporation. All rights reserved.
Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    ntnls.h

Abstract:

    NLS file formats and data types

--*/

#ifndef _NTNLS_
#define _NTNLS_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAXIMUM_LEADBYTES   12

typedef struct _CPTABLEINFO {
    USHORT CodePage;                    // code page number
    USHORT MaximumCharacterSize;        // max length (bytes) of a char
    USHORT DefaultChar;                 // default character (MB)
    USHORT UniDefaultChar;              // default character (Unicode)
    USHORT TransDefaultChar;            // translation of default char (Unicode)
    USHORT TransUniDefaultChar;         // translation of Unic default char (MB)
    USHORT DBCSCodePage;                // Non 0 for DBCS code pages
    UCHAR  LeadByte[MAXIMUM_LEADBYTES]; // lead byte ranges
    PUSHORT MultiByteTable;             // pointer to MB translation table
    PVOID   WideCharTable;              // pointer to WC translation table
    PUSHORT DBCSRanges;                 // pointer to DBCS ranges
    PUSHORT DBCSOffsets;                // pointer to DBCS offsets
} CPTABLEINFO, *PCPTABLEINFO;

typedef struct _NLSTABLEINFO {
    CPTABLEINFO OemTableInfo;
    CPTABLEINFO AnsiTableInfo;
    PUSHORT UpperCaseTable;             // 844 format upcase table
    PUSHORT LowerCaseTable;             // 844 format lower case table
} NLSTABLEINFO, *PNLSTABLEINFO;

#ifdef __cplusplus
}
#endif

#endif // _NTNLS_

