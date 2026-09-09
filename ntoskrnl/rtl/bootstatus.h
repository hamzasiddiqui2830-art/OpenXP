/*++

Module Name:

    bootstatus.h

Abstract:

    Private boot status data definitions used by the WRK RTL boot-status
    implementation.  The public RTL_BSD_ITEM_TYPE definition is provided by
    the local WRK ntrtl.h header.

--*/

#ifndef _OPENXP_BOOTSTATUS_H_
#define _OPENXP_BOOTSTATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BSD_FILE_NAME L"\\bootstat.dat"

typedef struct _BSD_BOOT_STATUS_DATA {
    ULONG Version;
    NT_PRODUCT_TYPE ProductType;
    BOOLEAN AutoAdvancedBoot;
    ULONG AdvancedBootMenuTimeout;
    BOOLEAN LastBootSucceeded;
    BOOLEAN LastBootShutdown;
} BSD_BOOT_STATUS_DATA, *PBSD_BOOT_STATUS_DATA;

#ifdef __cplusplus
}
#endif

#endif /* _OPENXP_BOOTSTATUS_H_ */
