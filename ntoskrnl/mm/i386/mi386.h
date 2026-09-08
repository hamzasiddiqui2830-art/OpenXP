/*++

Copyright (c) OpenXP Contributors
Project OpenXP Internal

Module Name:

    mi386.h

Abstract:

    This module contains the private data structures and procedure
    prototypes for the hardware dependent portion of the
    memory management system.

    This module is specifically tailored for the X86.

Author:

    Lou Perazzoli (loup) 6-Jan-1990

Revision History:

--*/

#ifndef _MI386_
#define _MI386_

//
// Virtual Memory Layout on X86
//

#define MM_HIGHEST_USER_ADDRESS         ((PVOID)0x7FFEFFFF)
#define MM_USER_PROBE_ADDRESS           ((PVOID)0x7FFF0000)

//
// PTE base addresses
//

#if !defined (_X86PAE_)

#define PTE_BASE ((PMMPTE)0xC0000000)
#define PDE_BASE ((PMMPTE)0xC0000000)

#define PTE_KBASE 0xC0000000              // Kernel address space PTE base
#define PTE_UTOP  0xBFFFFFFF              // User address space PTE top

#else
