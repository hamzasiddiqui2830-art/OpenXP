/*
 * ReactOS Kernel
 * Copyright (C) 2024 ReactOS Team
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//
// This is to make all the TEXT(...) macros come out right. As of 07/27/2000,
// UNICODE isn't defined in kernel space by default.
//
#define UNICODE

//
// This macro is used to convert HKLM relative paths from user-mode accessable
// headers into a form usable by kernel mode. Eventually this macro should be
// moved to somewhere like cm.h so the entire kernel can use it.
//
#define CM_REGISTRY_MACHINE(x) L"\\Registry\\Machine\\"##x

