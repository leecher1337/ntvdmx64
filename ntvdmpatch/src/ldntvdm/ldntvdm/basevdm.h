#pragma once

#define VDM_PARTIALLY_CREATED	    1
#define VDM_FULLY_CREATED	    2
#define VDM_BEING_REUSED	    4
#define VDM_CREATION_SUCCESSFUL     8

#define BINARY_TYPE_DOS 	    0x10
#define BINARY_TYPE_WIN16           0x20
#define BINARY_TYPE_SEPWOW          0x40

#define BINARY_SUBTYPE_MASK         0xF
#define BINARY_TYPE_DOS_EXE	    01
#define BINARY_TYPE_DOS_COM	    02
#define BINARY_TYPE_DOS_PIF         03

#define VDM_NOT_PRESENT 	    1
#define VDM_PRESENT_NOT_READY	    2
#define VDM_PRESENT_AND_READY	    4 

#define VDM_STATE_MASK		    7 