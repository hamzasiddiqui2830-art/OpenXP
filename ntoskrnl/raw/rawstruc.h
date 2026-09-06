/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    RawStruc.h

Abstract:

    This module defines the data structures that make up the major internal
    part of the Raw file system.

--*/

#ifndef _RAWSTRUC_
#define _RAWSTRUC_


//
//  The Vcb (Volume control Block) record corresponds to every volume mounted
//  by the file system.  This structure must be allocated from non-paged pool.
//

typedef struct _VCB {

    //
    //  The type and size of this record (must be RAW_NTC_VCB)
    //

    NODE_TYPE_CODE NodeTypeCode;
    NODE_BYTE_SIZE NodeByteSize;

    //
    //  A pointer the device object passed in by the I/O system on a mount
    //  This is the target device object that the file system talks to when it
    //  needs to do any I/O (e.g., the disk stripper device object).
    //
    //

    PDEVICE_OBJECT TargetDeviceObject;

    //
    //  A pointer to the VPB for the volume passed in by the I/O system on
    //  a mount.
    //

    PVPB Vpb;

    //
    //  A pointer to a spare Vpb used for explicit dismount
    // 

    PVPB SpareVpb;


    //
    //  The internal state of the device.
    //

    USHORT VcbState;

    //
    //  A mutex to control access to VcbState, OpenCount and ShareAccess
    //

    KMUTEX Mutex;

    //
    //  A count of the number of file objects that have opened the volume
    //  and their share access state.
    //

    CLONG OpenCount;

    SHARE_ACCESS ShareAccess;

    //
    //  Information about the disk geometry
    //

    ULONG BytesPerSector;

    LARGE_INTEGER SectorsOnDisk;

} VCB;
typedef VCB *PVCB;

#define VCB_STATE_FLAG_LOCKED            (0x0001)
#define VCB_STATE_FLAG_DISMOUNTED        (0x0002)

//
//  The Volume Device Object is an I/O system device object with a
//  VCB record appended to the end.  There are multiple of these
//  records, one for every mounted volume, and are created during
//  a volume mount operation.
//

typedef struct _VOLUME_DEVICE_OBJECT {

    DEVICE_OBJECT DeviceObject;

    //
    //  This is the file system specific volume control block.
    //

    VCB Vcb;

} VOLUME_DEVICE_OBJECT;
typedef VOLUME_DEVICE_OBJECT *PVOLUME_DEVICE_OBJECT;

#endif // _RAWSTRUC_

