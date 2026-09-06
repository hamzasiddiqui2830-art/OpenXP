/*
 * HAL ACPI Multiprocessor (ACPI-MP) HAL Stub Target
 * This is a stub implementation for the halacpim target
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 Copyright (c) OpenXP Team 2026.
 * You may only use this code if you agree to the terms of the Windows Research 
 * Kernel Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 */

/* Define _NTHAL_ to ensure NTHALAPI is defined correctly when building HAL */
#define _NTHAL_

#include <ntdef.h>
#include <hal.h>

/* Target-specific identifier */
const char* HalGetTargetName(VOID)
{
    return "halacpim";
}
