/*++

Copyright (c) OpenXP Team 2026.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.


Module Name:

    obvutil.h

Abstract:

    This header exposes various utilities required to do driver verification.

--*/

LONG_PTR
ObvUtilStartObRefMonitoring(
    IN PDEVICE_OBJECT DeviceObject
    );

LONG_PTR
ObvUtilStopObRefMonitoring(
    IN PDEVICE_OBJECT DeviceObject,
    IN LONG StartSkew
    );

