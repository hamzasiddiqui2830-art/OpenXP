/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    PnP.c

Abstract:

    The PnP package provides a method for file systems to 
    notify applications and services that a volume is being
    locked or unlocked, so handles to it can be closed and
    reopened.
    
    This module exports routines which help file systems
    do this notification.

--*/

#include "FsRtlP.h"

#ifndef FAR
#define FAR
#endif
#include <IoEvent.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, FsRtlNotifyVolumeEvent)
#endif


NTKERNELAPI
NTSTATUS
FsRtlNotifyVolumeEvent (
    __in PFILE_OBJECT FileObject,
    __in ULONG EventCode
    )

/*++

Routine Description:

    This routine notifies any registered applications that a 
    volume is being locked, unlocked, etc.  

Arguments:

    FileeObject - Supplies a file object for the volume being
        locked.

    EventCode - Which event is occurring -- e.g. FSRTL_VOLUME_LOCK
        
Return Value:

    Status of the notification.

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;

    TARGET_DEVICE_CUSTOM_NOTIFICATION Event;
    PDEVICE_OBJECT Pdo;

    //
    //  Retrieve the device object associated with this file object.
    //

    Status = IoGetRelatedTargetDevice( FileObject, &Pdo );

    if (NT_SUCCESS( Status )) {

        ASSERT(Pdo != NULL);

        Event.Version = 1;
        Event.FileObject = NULL;
        Event.NameBufferOffset = -1;
        Event.Size = (USHORT)FIELD_OFFSET( TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer );

        switch (EventCode) {

        case FSRTL_VOLUME_DISMOUNT:
            
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_DISMOUNT, sizeof( GUID ));
            break;
            
        case FSRTL_VOLUME_DISMOUNT_FAILED:
            
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_DISMOUNT_FAILED, sizeof( GUID ));
            break;            

        case FSRTL_VOLUME_LOCK:
        
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_LOCK, sizeof( GUID ));
            break;

        case FSRTL_VOLUME_LOCK_FAILED:
        
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_LOCK_FAILED, sizeof( GUID ));
            break;
        
        case FSRTL_VOLUME_MOUNT:
            
            //
            //  Mount notification is asynchronous to avoid deadlocks when someone 
            //  unwittingly causes a mount in the course of handling some other
            //  PnP notification, e.g. MountMgr's device arrival code.
            //
            
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_MOUNT, sizeof( GUID ));
            IoReportTargetDeviceChangeAsynchronous( Pdo, &Event, NULL, NULL );
            ObDereferenceObject( Pdo );
            return STATUS_SUCCESS;
            break;

        case FSRTL_VOLUME_UNLOCK:
        
            RtlCopyMemory( &Event.Event, &GUID_IO_VOLUME_UNLOCK, sizeof( GUID ));
            break;
            
        default:

            ObDereferenceObject( Pdo );
            return STATUS_INVALID_PARAMETER;
        }
        
        IoReportTargetDeviceChange( Pdo, &Event );
        ObDereferenceObject( Pdo );
    }

    return Status;
}

