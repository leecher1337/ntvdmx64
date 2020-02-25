!IF 0

Copyright (c) 1989-1991 Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

TARGETNAME=ntvdmd
TARGETPATH=obj
TARGETTYPE=DYNLINK

DLLENTRY=DbgDllInitialize
DLLDEF=$(O)\ntvdmd.def

INCLUDES=..\..\inc;..\..\softpc.new\host\inc

SOURCES=..\dbg.c	\
        ..\ntvdmd.rc

TARGETLIBS= \
    $(SDK_LIB_PATH)\kernel32.lib \
    $(SDK_LIB_PATH)\user32.lib \
    $(SDK_LIB_PATH)\ntvdm.lib

C_DEFINES=-DWIN_32 -DDEBUG

UMTYPE=windows

BUFFER_OVERFLOW_CHECKS=0
NO_SAFESEH=1