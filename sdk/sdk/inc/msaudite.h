//
// Copyright (c) OpenXP Contributors
// Copyright (c) OpenXP Team 2026.
//
// You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
// If you do not agree to the terms, do not use the code.
//

#ifndef _MSAUDITE_
#define _MSAUDITE_

//
// Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
// where
//
//     Sev - is the severity code
//
//         00 - Success
//         01 - Informational
//         10 - Warning
//         11 - Error
//
//     C - is the Customer code flag
//
//     R - is a reserved bit
//
//     Facility - is the facility code
//
//     Code - is the facility's status code
//
//
// Define the facility codes
//

//
// Define the severity codes
//

//
// MessageId: 0x00000000L (No symbolic name defined)
//
// MessageText:
//
// Unused message ID
//

#define SE_ADT_MIN_CATEGORY_ID   1
#define SE_ADT_MAX_CATEGORY_ID   9

#define SE_ADT_MIN_AUDIT_ID      0x200
#define SE_ADT_MAX_AUDIT_ID      0x5ff

#define SE_AUDITID_AUDITS_DISCARDED      ((ULONG)0x00000204L)

#endif /* _MSAUDITE_ */