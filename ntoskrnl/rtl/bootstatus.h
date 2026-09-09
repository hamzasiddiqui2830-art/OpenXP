/*++

Module Name:

    bootstatus.h

Abstract:

    Private boot status data definitions used by the WRK RTL boot-status
    implementation.  Kept local so the build does not depend on a Windows SDK
    bootstatus header.

--*/

#ifndef _OPENXP_BOOTSTATUS_H_
#define _OPENXP_BOOTSTATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BSD_FILE_NAME L"\\bootstat.dat"

typedef enum _RTL_BSD_ITEM_TYPE {
    RtlBsdItemVersionNumber = 0,
    RtlBsdItemProductType,
    RtlBsdItemAabEnabled,
    RtlBsdItemAabTimeout,
    RtlBsdItemLastBootSucceeded,
    RtlBsdItemLastBootShutdown,
    RtlBsdItemMax
} RTL_BSD_ITEM_TYPE;

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
