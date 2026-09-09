/*++

Module Name:

    fat.h

Abstract:

    Minimal FAT on-disk definitions required by the XIP executive.
    This header is intentionally limited to the boot-sector/BPB helpers
    consumed by xipdisp.c; the full filesystem implementation is not part
    of the kernel executive.

--*/

#ifndef _OPENXP_FAT_H_
#define _OPENXP_FAT_H_

#include "../inc/cpyuchr.h"

typedef LONGLONG LBO;

typedef struct _PACKED_BIOS_PARAMETER_BLOCK {
    UCHAR BytesPerSector[2];
    UCHAR SectorsPerCluster[1];
    UCHAR ReservedSectors[2];
    UCHAR Fats[1];
    UCHAR RootEntries[2];
    UCHAR Sectors[2];
    UCHAR Media[1];
    UCHAR SectorsPerFat[2];
    UCHAR SectorsPerTrack[2];
    UCHAR Heads[2];
    UCHAR HiddenSectors[4];
    UCHAR LargeSectors[4];
} PACKED_BIOS_PARAMETER_BLOCK, *PPACKED_BIOS_PARAMETER_BLOCK;

typedef struct _BIOS_PARAMETER_BLOCK {
    USHORT BytesPerSector;
    UCHAR SectorsPerCluster;
    USHORT ReservedSectors;
    UCHAR Fats;
    USHORT RootEntries;
    USHORT Sectors;
    UCHAR Media;
    USHORT SectorsPerFat;
    USHORT SectorsPerTrack;
    USHORT Heads;
    ULONG HiddenSectors;
    ULONG LargeSectors;
} BIOS_PARAMETER_BLOCK, *PBIOS_PARAMETER_BLOCK;

#define FatUnpackBios(Bios, Pbios) { \
    CopyUchar2(&(Bios)->BytesPerSector, &(Pbios)->BytesPerSector[0]); \
    CopyUchar1(&(Bios)->SectorsPerCluster, &(Pbios)->SectorsPerCluster[0]); \
    CopyUchar2(&(Bios)->ReservedSectors, &(Pbios)->ReservedSectors[0]); \
    CopyUchar1(&(Bios)->Fats, &(Pbios)->Fats[0]); \
    CopyUchar2(&(Bios)->RootEntries, &(Pbios)->RootEntries[0]); \
    CopyUchar2(&(Bios)->Sectors, &(Pbios)->Sectors[0]); \
    CopyUchar1(&(Bios)->Media, &(Pbios)->Media[0]); \
    CopyUchar2(&(Bios)->SectorsPerFat, &(Pbios)->SectorsPerFat[0]); \
    CopyUchar2(&(Bios)->SectorsPerTrack, &(Pbios)->SectorsPerTrack[0]); \
    CopyUchar2(&(Bios)->Heads, &(Pbios)->Heads[0]); \
    CopyUchar4(&(Bios)->HiddenSectors, &(Pbios)->HiddenSectors[0]); \
    CopyUchar4(&(Bios)->LargeSectors, &(Pbios)->LargeSectors[0]); \
}

typedef struct _PACKED_BOOT_SECTOR {
    UCHAR Jump[3];
    UCHAR Oem[8];
    PACKED_BIOS_PARAMETER_BLOCK PackedBpb;
    UCHAR PhysicalDriveNumber;
    UCHAR CurrentHead;
    UCHAR Signature;
    UCHAR Id[4];
    UCHAR VolumeLabel[11];
    UCHAR SystemId[8];
} PACKED_BOOT_SECTOR, *PPACKED_BOOT_SECTOR;

#define FatBytesPerCluster(B) \
    ((ULONG)((B)->BytesPerSector * (B)->SectorsPerCluster))

#define FatBytesPerFat(B) \
    ((ULONG)((B)->BytesPerSector * (B)->SectorsPerFat))

#define FatReservedBytes(B) \
    ((ULONG)((B)->BytesPerSector * (B)->ReservedSectors))

#define FatRootDirectorySize(B) \
    ((ULONG)((B)->RootEntries * 32U))

#define FatRootDirectoryLbo(B) \
    (FatReservedBytes(B) + ((B)->Fats * FatBytesPerFat(B)))

#define FatFileAreaLbo(B) \
    (FatRootDirectoryLbo(B) + FatRootDirectorySize(B))

#endif /* _OPENXP_FAT_H_ */
