/*++

Module Name:

    ntrtlp.h

Abstract:

    Private RTL declarations shared by the WRK RTL implementation and RTL
    test programs.

--*/

#ifndef _NTRTLP_
#define _NTRTLP_

#include <ntos.h>
#include <nturtl.h>
#include <zwapi.h>

#if defined(_AMD64_)
#include "amd64/ntrtlamd64.h"
#elif defined(_X86_)
#include "i386/ntrtl386.h"
#else
#error "no target architecture"
#endif

#include <string.h>
#include <wchar.h>

#define MAX_USTRING (sizeof(WCHAR) * (MAXUSHORT / sizeof(WCHAR)))
#define ASSERT_WELL_FORMED_UNICODE_STRING(Str) \
    ASSERT((!(((Str)->Length & 1) || ((Str)->MaximumLength & 1))) && \
           ((Str)->Length <= (Str)->MaximumLength))
#define ASSERT_WELL_FORMED_UNICODE_STRING_IN(Str) ASSERT(!((Str)->Length & 1))
#define ASSERT_WELL_FORMED_UNICODE_STRING_OUT(Str) \
    ASSERT((!((Str)->MaximumLength & 1)) && ((Str)->Length <= (Str)->MaximumLength))

#if defined(_WIN64)
extern PVOID RtlpFunctionAddressTable[];
extern UNWIND_HISTORY_TABLE RtlpUnwindHistoryTable;
#endif

VOID RtlCaptureImageExceptionValues(IN PVOID Base, OUT PVOID *FunctionTable, OUT PULONG TableSize);
LONG LdrpCompareResourceNames(IN ULONG ResourceName, IN const IMAGE_RESOURCE_DIRECTORY *ResourceDirectory, IN const IMAGE_RESOURCE_DIRECTORY_ENTRY *ResourceDirectoryEntry);
NTSTATUS LdrpSearchResourceSection(IN PVOID DllHandle, IN const ULONG_PTR *ResourceIdPath, IN ULONG ResourceIdPathLength, IN BOOLEAN FindDirectoryEntry, OUT PVOID *ResourceDirectoryOrData);
PVOID LdrpGetAlternateResourceModuleHandle(IN PVOID Module, IN LANGID LangId);
LONG LdrpCompareResourceNames_U(IN ULONG_PTR ResourceName, IN const IMAGE_RESOURCE_DIRECTORY *ResourceDirectory, IN const IMAGE_RESOURCE_DIRECTORY_ENTRY *ResourceDirectoryEntry);
NTSTATUS LdrpSearchResourceSection_U(IN PVOID DllHandle, IN const ULONG_PTR *ResourceIdPath, IN ULONG ResourceIdPathLength, IN ULONG Flags, OUT PVOID *ResourceDirectoryOrData);
NTSTATUS LdrpAccessResourceData(IN PVOID DllHandle, IN const IMAGE_RESOURCE_DATA_ENTRY *ResourceDataEntry, OUT PVOID *Address OPTIONAL, OUT PULONG Size OPTIONAL);
NTSTATUS LdrpAccessResourceDataNoMultipleLanguage(IN PVOID DllHandle, IN const IMAGE_RESOURCE_DATA_ENTRY *ResourceDataEntry, OUT PVOID *Address OPTIONAL, OUT PULONG Size OPTIONAL);
VOID RtlpAnsiPszToUnicodePsz(IN PCHAR AnsiString, IN WCHAR *UnicodeString, IN USHORT AnsiStringLength);
BOOLEAN RtlpDidUnicodeToOemWork(IN PCOEM_STRING OemString, IN PCUNICODE_STRING UnicodeString);

extern CONST CCHAR RtlpBitsClearAnywhere[256];
extern CONST CCHAR RtlpBitsClearLow[256];
extern CONST CCHAR RtlpBitsClearHigh[256];
extern CONST CCHAR RtlpBitsClearTotal[256];
#define RtlpBitSetAnywhere(Byte) RtlpBitsClearAnywhere[(~(Byte) & 0xFF)]
#define RtlpBitsSetLow(Byte) RtlpBitsClearLow[(~(Byte) & 0xFF)]
#define RtlpBitsSetHigh(Byte) RtlpBitsClearHigh[(~(Byte) & 0xFF)]
#define RtlpBitsSetTotal(Byte) RtlpBitsClearTotal[(~(Byte) & 0xFF)]

extern PUSHORT Nls844UnicodeUpcaseTable;
extern PUSHORT Nls844UnicodeLowercaseTable;
#define LOBYTE(w) ((UCHAR)((w)))
#define HIBYTE(w) ((UCHAR)(((USHORT)(w) >> 8) & 0xFF))
#define GET8(w) ((ULONG)(((w) >> 8) & 0xff))
#define GETHI4(w) ((ULONG)(((w) >> 4) & 0xf))
#define GETLO4(w) ((ULONG)((w) & 0xf))
#define TRAVERSE844W(pTable,wch) ((pTable)[(pTable)[(pTable)[GET8((wch))] + GETHI4((wch))] + GETLO4((wch))])
#define NLS_UPCASE(wch) (((wch) < 'a') ? (wch) : ((wch) <= 'z' ? (wch)-('a'-'A') : ((WCHAR)((wch)+TRAVERSE844W(Nls844UnicodeUpcaseTable,(wch))))))
#define NLS_DOWNCASE(wch) (((wch) < 'A') ? (wch) : ((wch) <= 'Z' ? (wch)+('a'-'A') : ((WCHAR)((wch)+TRAVERSE844W(Nls844UnicodeLowercaseTable,(wch))))))

#if DBG
#define RTL_PAGED_CODE() PAGED_CODE()
#else
#define RTL_PAGED_CODE()
#endif

typedef NTSTATUS (*PRTL_COMPRESS_WORKSPACE_SIZE)(IN USHORT, OUT PULONG, OUT PULONG);
typedef NTSTATUS (*PRTL_COMPRESS_BUFFER)(IN USHORT, IN PUCHAR, IN ULONG, OUT PUCHAR, IN ULONG, IN ULONG, OUT PULONG, IN PVOID);
typedef NTSTATUS (*PRTL_DECOMPRESS_BUFFER)(OUT PUCHAR, IN ULONG, IN PUCHAR, IN ULONG, OUT PULONG);
typedef NTSTATUS (*PRTL_DECOMPRESS_FRAGMENT)(OUT PUCHAR, IN ULONG, IN PUCHAR, IN ULONG, IN ULONG, OUT PULONG, IN PVOID);
typedef NTSTATUS (*PRTL_DESCRIBE_CHUNK)(IN OUT PUCHAR *, IN PUCHAR, OUT PUCHAR *, OUT PULONG);
typedef NTSTATUS (*PRTL_RESERVE_CHUNK)(IN OUT PUCHAR *, IN PUCHAR, OUT PUCHAR *, IN ULONG);

NTSTATUS RtlCompressWorkSpaceSizeLZNT1(IN USHORT, OUT PULONG, OUT PULONG);
NTSTATUS RtlCompressBufferLZNT1(IN USHORT, IN PUCHAR, IN ULONG, OUT PUCHAR, IN ULONG, IN ULONG, OUT PULONG, IN PVOID);
NTSTATUS RtlDecompressBufferLZNT1(OUT PUCHAR, IN ULONG, IN PUCHAR, IN ULONG, OUT PULONG);