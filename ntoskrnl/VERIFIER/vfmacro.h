/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    vfmacro.h

Abstract:

    This header contains a collection of macros used by the verifier.

--*/


//
// This macro takes an array and returns the number of elements in it.
//
#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

//
// This macro takes a value and an alignment and rounds the entry up
// appropriately. The alignment MUST be a power of two!
//
#define ALIGN_UP_ULONG(value, alignment) (((value)+(alignment)-1)&(~(alignment-1)))

//
// This macro compares two guids in their binary form for equivalence.
//
#define IS_EQUAL_GUID(a,b) (RtlCompareMemory(a, b, sizeof(GUID)) == sizeof(GUID))

