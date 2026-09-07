/*
 * PROJECT:         OpenXP system libraries
 * LICENSE:         GNU GPL - See COPYING in the top level directory
 * PURPOSE:         Support library for PSEH3 - Assembly definitions
 */

#ifndef _PSEH3_ASMDEF_H_
#define _PSEH3_ASMDEF_H_

/* Field offsets for SEH3$_REGISTRATION_FRAME structure */
#define SEH3_REGISTRATION_FRAME_Next 0
#define SEH3_REGISTRATION_FRAME_Handler 4
#define SEH3_REGISTRATION_FRAME_ScopeTable 8
#define SEH3_REGISTRATION_FRAME_TryLevel 12
#define SEH3_REGISTRATION_FRAME_EndOfChain 16
#define SEH3_REGISTRATION_FRAME_ExceptionPointers 20
#define SEH3_REGISTRATION_FRAME_Esp 24
#define SEH3_REGISTRATION_FRAME_Ebp 28
#define SEH3_REGISTRATION_FRAME_ExceptionCode 32
#define SEH3_REGISTRATION_FRAME_AllocaFrame 36
#ifdef _SEH3$_FRAME_ALL_NONVOLATILES
#define SEH3_REGISTRATION_FRAME_Ebx 40
#define SEH3_REGISTRATION_FRAME_Esi 44
#define SEH3_REGISTRATION_FRAME_Edi 48
#ifdef __clang__
#define SEH3_REGISTRATION_FRAME_ReturnAddress 52
#endif
#else
#ifdef __clang__
#define SEH3_REGISTRATION_FRAME_ReturnAddress 40
#endif
#endif

/* Field offsets for SEH3$_SCOPE_TABLE structure */
#define SEH3_SCOPE_TABLE_EnclosingLevel 0
#define SEH3_SCOPE_TABLE_Filter 4
#define SEH3_SCOPE_TABLE_Target 8

#endif /* _PSEH3_ASMDEF_H_ */
