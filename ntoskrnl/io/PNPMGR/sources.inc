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

MAJORCOMP=ntos
MINORCOMP=io

TARGETNAME=pnpmgr
TARGETTYPE=LIBRARY
TARGETPATH=obj

MSC_WARNING_LEVEL=/W4 /WX
BUILD_PRODUCES=ntospnpmgr$(NT_UP)

INCLUDES=..;..\..;\
         ..\..\..\inc;\
         ..\..\..\pnp;\
         $(WINDOWS_INC_PATH);\
         $(DDK_INC_PATH);\
         $(NET_INC_PATH);\
         $(HALKIT_INC_PATH)

SOURCES=   \
        ..\assign.c   \
        ..\devices.c  \
        ..\devnode.c  \
        ..\devintrf.c \
        ..\dockhwp.c  \
        ..\iofileutil.c \
        ..\mapper.c   \
        ..\notify.c   \
        ..\pnpbusno.c \
        ..\pnpcvrt.c  \
        ..\pnpdata.c  \
        ..\pnpdd.c    \
        ..\pnpdel.c   \
        ..\pnpdma.c   \
        ..\pnpeisa.c  \
        ..\pnpenum.c  \
        ..\pnpevent.c  \
        ..\pnpinit.c  \
        ..\pnpioapi.c \
        ..\pnpirp.c   \
        ..\pnpirq.c   \
        ..\pnpmap.c   \
        ..\pnpmemio.c \
        ..\pnppower.c \
        ..\pnpres.c   \
        ..\pnprlist.c \
        ..\pnpstart.c  \
        ..\pnpsubs.c  \
        ..\ppcddb.c   \
        ..\ppcontrol.c  \
        ..\pphandle.c \
        ..\pphotswap.c \
        ..\pplastgood.c \
        ..\pppagepath.c \
        ..\ppdrvdb.c \
        ..\ppprofile.c  \
        ..\ppvutil.c \
        ..\remlock.c \
        ..\report.c

NTTEST=

PRECOMPILED_INCLUDE=..\pnpmgrp.h
PRECOMPILED_PCH=pnpmgrp.pch
PRECOMPILED_OBJ=pnpmgrp.obj

#  SOURCES_USED=..\sources.inc
