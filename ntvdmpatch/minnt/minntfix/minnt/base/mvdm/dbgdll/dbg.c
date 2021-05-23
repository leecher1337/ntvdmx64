/*
 *  dbg.c - Main Module of DBG DLL.
 *
 *  BobDay 13-Jan-1992 Created
 *
 * WARNING: THIS MODULE APPEARS TO HAVE MANY FUNCTIONS THAT APPEAR IDENTICAL
 *          THEY ARE NOT IDENTICAL.  THEY ARE NEARLY IDENTICAL.  IF YOU CHOOSE
 *          TO COMBINE SIMILAR FUNCTIONALITY INTO SEVERAL SMALLER ROUTINES,
 *          BE VERY CAREFUL TO LOOK FOR THE SMALL SUBTLE DIFFERENCES.
 */

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <wdbgexts.h>
#include <stdio.h>
#include <string.h>
#include <mvdm.h>
#include <bop.h>
#include <softpc.h>
#include <dbgexp.h>
#include <dbgsvc.h>
#include <vdmdbg.h>
#include <dbginfo.h>

#define MAX_MODULE  64

#define MAX_DBG_FRAME   10

ULONG   ulTHHOOK = 0L;          // Address registered from 16-bit world
LPVOID  lpRemoteAddress = NULL; // Address registered from WOW32
DWORD   lpRemoteBlock   = 0;    // Address registered from WOW32
BOOL    f386;
#ifdef MONITOR
PX86CONTEXT     px86;
#endif
VDMCONTEXT        vcContext;
VDMINTERNALINFO   viInfo;
DWORD IntelMemoryBase, VdmDbgTraceFlags, VdmDbgEFLAGS, EventFlags;
LPVOID lpNtCpuInfo, lpVdmState;
VDM_BREAKPOINT VdmBreakPoints[16]={0};
DWORD   EventParams[4];
BOOL    InVdmDebugger = FALSE, bWantsNtsdPrompt = FALSE, DbgTimerInitialized = FALSE, bWantsTraceInteractive = FALSE;
WORD   DbgWowhExeHead = 0, DbgWowhGlobalHeap = 0;
BYTE    DbgTimerMode = VDMTI_TIMER_TICK;

// From ntvdm.exe
extern DECLSPEC_IMPORT LDT_ENTRY *ExpLdt;

/* include inc/vint.h from Win2k for FIXED_NTVDMSTATE_LINEAR */
#undef pNtVDMState
#define pNtVDMState   ((ULONG *)(IntelMemoryBase+FIXED_NTVDMSTATE_LINEAR))

extern VOID VdmDbgAttach(VOID);
extern PVOID VdmMapFlat(USHORT selector, ULONG offset, ULONG mode);
BOOL SendVDMEvent(DWORD Param0);

typedef struct _trapframe {
    WORD    wCode;          /* Noise from DbgDispatchBop */
    WORD    wAX;            /* AX at time of fault */
    WORD    wDS;            /* DS at time of fault */
    WORD    wRetIP;         /* Noise from DPMI */
    WORD    wRetCS;         /* Noise from DPMI */
    WORD    wErrCode;       /* Noise from 16-bit kernel */
    WORD    wIP;            /* IP at time of fault */
    WORD    wCS;            /* CS at time of fault */
    WORD    wFlags;         /* Flags at time of fault */
    WORD    wSP;            /* SS at time of fault */
    WORD    wSS;            /* SP at time of fault */
} TFRAME16;
typedef TFRAME16 UNALIGNED *PTFRAME16;

typedef struct _faultframe {
    WORD    wES;            /* ES at time of fault */
    WORD    wDS;            /* DS at time of fault */
    WORD    wDI;            /* DI at time of fault */
    WORD    wSI;            /* SI at time of fault */
    WORD    wTempBP;        /* Noise from 16-bit kernel stack frame */
    WORD    wTempSP;        /* Noise from 16-bit kernel stack frame */
    WORD    wBX;            /* BX at time of fault */
    WORD    wDX;            /* DX at time of fault */
    WORD    wCX;            /* CX at time of fault */
    WORD    wAX;            /* AX at time of fault */
    WORD    wBP;            /* BP at time of fault */
    WORD    npszMsg;        /* Noise from 16-bit kernel */
    WORD    wPrevIP;        /* Noise from DPMI */
    WORD    wPrevCS;        /* Noise from DPMI */
    WORD    wRetIP;         /* Noise from DPMI */
    WORD    wRetCS;         /* Noise from DPMI */
    WORD    wErrCode;       /* Noise from 16-bit kernel */
    WORD    wIP;            /* IP at time of fault */
    WORD    wCS;            /* CS at time of fault */
    WORD    wFlags;         /* Flags at time of fault */
    WORD    wSP;            /* SS at time of fault */
    WORD    wSS;            /* SP at time of fault */
} FFRAME16;
typedef FFRAME16 UNALIGNED *PFFRAME16;

typedef struct _newtaskframe {
    DWORD   dwNoise;            /* Noise from InitTask         */
    DWORD   dwModulePath;       /* Module path address         */
    DWORD   dwModuleName;       /* Module name address         */
    WORD    hModule;            /* 16-bit Module handle        */
    WORD    hTask;              /* 16-bit Task handle          */
    WORD    wFlags;             /* Flags at time to task start */
    WORD    wDX;                /* DX at time of task start    */
    WORD    wBX;                /* BX at time of task start    */
    WORD    wES;                /* ES at time of task start    */
    WORD    wCX;                /* CX at time of task start    */
    WORD    wAX;                /* AX at time of task start    */
    WORD    wDI;                /* DI at time of task start    */
    WORD    wSI;                /* SI at time of task start    */
    WORD    wDS;                /* DS at time of task start    */
    WORD    wBP;                /* BP at time of task start    */
    WORD    wIP;                /* IP for task start           */
    WORD    wCS;                /* CS for task start           */
} NTFRAME16;
typedef NTFRAME16 UNALIGNED *PNTFRAME16;

#pragma pack(2)

typedef struct _stoptaskframe {
    WORD    wCode;              /* Noise from BOP Dispatcher  */
    DWORD   dwModulePath;       /* Module path address        */
    DWORD   dwModuleName;       /* Module name address        */
    WORD    hModule;            /* 16-bit Module handle       */
    WORD    hTask;              /* 16-bit Task handle         */
} STFRAME16;
typedef STFRAME16 UNALIGNED *PSTFRAME16;

typedef struct _newdllframe {
    WORD    wCode;              /* Noise from DbgDispatchBop  */
    DWORD   dwModulePath;       /* Module path address        */
    DWORD   dwModuleName;       /* Module name address        */
    WORD    hModule;            /* 16-bit Module handle       */
    WORD    hTask;              /* 16-bit Task handle         */
    WORD    wDS;                /* DS at time of dll start    */
    WORD    wAX;                /* AX at time of dll start    */
    WORD    wIP;                /* IP at time of dll start    */
    WORD    wCS;                /* CS at time of dll start    */
    WORD    wFlags;             /* Flags at time of dll start */
} NDFRAME16;
typedef NDFRAME16 UNALIGNED *PNDFRAME16;

#pragma pack()

#ifndef LODWORD
#define LODWORD(i)          ((DWORD)(i))
#endif

#ifndef HIDWORD
#define HIDWORD(i)          ((DWORD)(((__int64)(i) >> 32) & 0xFFFFFFFF))
#endif

BOOL  fDebugged = FALSE;

VOID DbgAttach(void);
void DbgGetContext();
void FlushVdmBreakPoints();


BOOL DbgDllInitialize(
    HINSTANCE hinstDLL,
    DWORD     fdwReason, 
    LPVOID    lpReserved
) {
    HANDLE      hProcess;
    HANDLE      MyDebugPort;
    DWORD       st;
    BOOL        fWasDebugged;

    hProcess = NtCurrentProcess();

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:

#ifdef MONITOR
            // X86 Only, get pointer to Register Context Block
            px86 = getIntelRegistersPointer();
#endif
            fWasDebugged = FALSE;
            break;
    case DLL_THREAD_ATTACH:
            fWasDebugged = fDebugged;
            break;
        default:
            return( TRUE );
    }

    st = NtQueryInformationProcess(
                hProcess,
                ProcessDebugPort,
                (PVOID)&MyDebugPort,
                sizeof(MyDebugPort),
                NULL );
    if ( NT_SUCCESS(st) ) {
        fDebugged = (MyDebugPort != NULL);
    } else {
        fDebugged = FALSE;
    }
    if ( fDebugged && !fWasDebugged ) {
         DbgAttach();
    }
    return( TRUE );
}

VOID DbgAttach(void)
{
    DbgGetContext();
    SendVDMEvent(DBG_ATTACH);
    VdmDbgAttach();    // Import
}

BOOL SendVDMEvent(DWORD Param0) 
{
    BOOL    fResult;

    EventParams[0] = Param0 | (EventFlags << 16);
    EventParams[3] = (DWORD)&viInfo;

    InVdmDebugger = TRUE;
    do
    {
        // Slimyness to determine whether the exception was handled or not.
        try {
            bWantsNtsdPrompt = FALSE;
            RaiseException( STATUS_VDM_EVENT,
                            0,
                            4,
                            (ULONG_PTR*)&EventParams);
			fResult = TRUE;
        } except(EXCEPTION_EXECUTE_HANDLER) {
            fResult = FALSE;
        }
        if (!bWantsNtsdPrompt) break;
        DbgBreakPoint();
    } while ( bWantsNtsdPrompt );
    InVdmDebugger = FALSE;
    FlushVdmBreakPoints();
    return( fResult );
}


/* GetNormalContext() - Get the VDM's current context
 *
 * Most of the routines that send VDMEvents need to have a context record
 * associated with them.  This routine is a quick way to get most of the
 * general registers.  Redundant work is being done because AX for example
 * is often on the stack and as such must really be pulled from the frame.
 * Hopefully this is OK because it is fast.
 */
#ifdef MONITOR
#define PX86    px86
#else
#define PX86    0
#endif

void DbgGetContext()
{
    //
    // Everything defaults to 0.
    //
    RtlFillMemory( &vcContext, sizeof(VDMCONTEXT), (UCHAR)0 );
    RtlFillMemory( &viInfo, sizeof(VDMINTERNALINFO), (UCHAR)0 );

    //
    // Fill in the internal info structure
    //
    viInfo.dwLdtBase       = (DWORD)ExpLdt;    // Import from NTVDM
    viInfo.dwIntelBase     = IntelMemoryBase;
    viInfo.wKernelSeg      = HIWORD(ulTHHOOK);
    viInfo.dwOffsetTHHOOK  = (DWORD)(LOWORD(ulTHHOOK));
    viInfo.vdmContext      = &vcContext;
    viInfo.lpRemoteAddress = lpRemoteAddress;
    viInfo.lpRemoteBlock   = lpRemoteBlock;
    viInfo.f386            = f386;
    viInfo.lpNtvdmState    = lpVdmState;
    viInfo.vdmContext      = &vcContext;
    viInfo.lpVdmDbgFlags   = &VdmDbgTraceFlags;
    viInfo.lpNtCpuInfo     = lpNtCpuInfo;
    viInfo.lpVdmBreakPoints= &VdmBreakPoints;


    //
    // Fill in the context structure
    //
    vcContext.SegEs = (ULONG)getES();
    vcContext.SegDs = (ULONG)getDS();
    vcContext.SegCs = (ULONG)getCS();
    vcContext.SegSs = (ULONG)getSS();
    vcContext.SegFs = (ULONG)getFS();
    vcContext.SegGs = (ULONG)getGS();

    vcContext.EFlags = getEFLAGS();
    VdmDbgEFLAGS = vcContext.EFlags;
#ifdef xxMONITORxx	/* Not used anymore */
    //
    // On x86 systems, we really might have some data in the FS and GS
    // registers.  Hopefully the code path through DOSX and KERNEL don't
    // modify them (I traced it and it didn't seem to).  Here is where
    // we attempt to recover them.
    //
    vcContext.SegGs = px86->SegGs;
    vcContext.SegFs = px86->SegFs;

    // On x86 systems, we really might have some data in the high words
    // of the registers.  Hopefully the code path through DOSX and KERNEL
    // don't modify them (I trace it and it didn't seem to).  Here is where
    // we attempt to recover them.

    vcContext.Edi    = MAKELONG(getDI(),    HIWORD(px86->Edi   ));
    vcContext.Esi    = MAKELONG(getSI(),    HIWORD(px86->Esi   ));
    vcContext.Ebx    = MAKELONG(getBX(),    HIWORD(px86->Ebx   ));
    vcContext.Edx    = MAKELONG(getDX(),    HIWORD(px86->Edx   ));
    vcContext.Ecx    = MAKELONG(getCX(),    HIWORD(px86->Ecx   ));
    vcContext.Eax    = MAKELONG(getAX(),    HIWORD(px86->Eax   ));

    vcContext.Ebp    = MAKELONG(getBP(),    HIWORD(px86->Ebp   ));
    vcContext.Eip    = MAKELONG(getIP(),    HIWORD(px86->Eip   ));
    vcContext.Esp    = MAKELONG(getSP(),    HIWORD(px86->Esp   ));

#else
    vcContext.Edi = (ULONG)getEDI();
    vcContext.Esi = (ULONG)getESI();
    vcContext.Ebx = (ULONG)getEBX();
    vcContext.Edx = (ULONG)getEDX();
    vcContext.Ecx = (ULONG)getECX();
    vcContext.Eax = (ULONG)getEAX();

    vcContext.Ebp = (ULONG)getEBP();
    vcContext.Eip = (ULONG)getEIP();
    vcContext.Esp = (ULONG)getESP();

    EventFlags = 0;

    if ( (getMSW() & MSW_PE) == 0 || (getEFLAGS() & V86FLAGS_V86) ) {
        EventFlags |= VDMEVENT_V86;
    } else {
        EventFlags |= VDMEVENT_PE;
        if ( getMSW() & MSW_PE ) {
            if ( !(ExpLdt[vcContext.SegCs >> 3].HighWord.Bits.Default_Big) )
            if (! (*((BYTE*)ExpLdt + vcContext.SegCs + 6) & 0x40))
                EventFlags |= VDMEVENT_PM16;
        }
    }

#endif
}

void FlushVdmBreakPoints()
{
  int i;

  for (i=0; i<sizeof(VdmBreakPoints)/sizeof(VdmBreakPoints[0]); i++)
  {
      if (VdmBreakPoints[i].Flags & VDMBP_FLUSH)
          VdmBreakPoints[i].Flags &= ~VDMBP_FLUSH;
  }
}

void SegmentLoad(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    Selector,
    WORD    Segment,
    BOOL    fData

) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;
        UINT            length;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1 = Selector;
        se.Segment   = Segment;
        se.Type      = (WORD)fData;

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_SEGLOAD );
    }
}

void SegmentMove(
    WORD    OldSelector,
    WORD    NewSelector
) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1   = OldSelector;
        se.Selector2   = NewSelector;

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_SEGMOVE );
    }
}

void SegmentFree(
    WORD    Selector,
    BOOL    fBPRelease
) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1   = Selector;
        se.Type        = (WORD)fBPRelease;

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_SEGFREE );
    }
}

void ModuleLoad(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    Segment,
    DWORD   Length
) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;
        UINT            length;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Segment = Segment;
        se.Length  = Length;

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_MODLOAD );
    }
}

void ModuleSegmentMove(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    ModuleSegment,
    WORD    OldSelector,
    WORD    NewSelector,
    DWORD   Length
) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;
        UINT            length;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Segment     = ModuleSegment;
        se.Selector1   = OldSelector;
        se.Selector2   = NewSelector;
        se.Length      = Length;
        se.Type        = 2;
        length = strlen(lpPathName);

        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_SEGMOVE );
    }
}

void ModuleFree(
    LPSTR   lpModuleName,
    LPSTR   lpPathName
) {

    if ( fDebugged ) {
        SEGMENT_NOTE    se;
        UINT            length;

        DbgGetContext();

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( DBG_MODFREE );
    }
}

void DbgSetTemporaryBP(
    WORD wSeg, 
    WORD wOffset, 
    BOOL fProtectedMode
) {
  PBYTE pInstr;

  if ( VdmBreakPoints[0].Flags & VDMBP_SET )
  {
    pInstr = (PBYTE)VdmMapFlat(
                    VdmBreakPoints[0].Seg,
                    VdmBreakPoints[0].Offset,
                    !(VdmBreakPoints[0].Flags & VDMBP_V86));
    if (pInstr)
    {
      if ( *pInstr == 0xCC )
        *pInstr = VdmBreakPoints[0].Opcode;
    }
  }
  pInstr = (PBYTE)VdmMapFlat(wSeg, wOffset, fProtectedMode);
  if ( pInstr )
  {
    VdmBreakPoints[0].Seg = wSeg;
    VdmBreakPoints[0].Offset = wOffset;
    VdmBreakPoints[0].Flags = (fProtectedMode?VDMBP_V86:0) | VDMBP_SET | VDMBP_ENABLED;
    VdmBreakPoints[0].Opcode = *pInstr;
    *pInstr = 0xCCu;
  }
  else
  {
    VdmBreakPoints[0].Flags = 0;
  }
}

BOOL DbgGPFault2(
    PFFRAME16   pFFrame
) {
    BOOL            fResult;

    fResult = FALSE;        // Default to Event not handled

    DbgGetContext();

    vcContext.SegEs = (ULONG)pFFrame->wES;
    vcContext.SegDs = (ULONG)pFFrame->wDS;
    vcContext.SegCs = (ULONG)pFFrame->wCS;
    vcContext.SegSs = (ULONG)pFFrame->wSS;

#ifdef MONITOR
    //
    // On x86 systems, we really might have some data in the high words
    // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to recover them.
    //
    vcContext.Edi    = MAKELONG(pFFrame->wDI,   HIWORD(px86->Edi   ));
    vcContext.Esi    = MAKELONG(pFFrame->wSI,   HIWORD(px86->Esi   ));
    vcContext.Ebx    = MAKELONG(pFFrame->wBX,   HIWORD(px86->Ebx   ));
    vcContext.Edx    = MAKELONG(pFFrame->wDX,   HIWORD(px86->Edx   ));
    vcContext.Ecx    = MAKELONG(pFFrame->wCX,   HIWORD(px86->Ecx   ));
    vcContext.Eax    = MAKELONG(pFFrame->wAX,   HIWORD(px86->Eax   ));

    vcContext.Ebp    = MAKELONG(pFFrame->wBP,   HIWORD(px86->Ebp   ));
    vcContext.Eip    = MAKELONG(pFFrame->wIP,   HIWORD(px86->Eip   ));
    vcContext.Esp    = MAKELONG(pFFrame->wSP,   HIWORD(px86->Esp   ));
    vcContext.EFlags = MAKELONG(pFFrame->wFlags,HIWORD(px86->EFlags));
#else
    vcContext.Edi    = (ULONG)pFFrame->wDI;
    vcContext.Esi    = (ULONG)pFFrame->wSI;
    vcContext.Ebx    = (ULONG)pFFrame->wBX;
    vcContext.Edx    = (ULONG)pFFrame->wDX;
    vcContext.Ecx    = (ULONG)pFFrame->wCX;
    vcContext.Eax    = (ULONG)pFFrame->wAX;

    vcContext.Ebp    = (ULONG)pFFrame->wBP;
    vcContext.Eip    = (ULONG)pFFrame->wIP;
    vcContext.Esp    = (ULONG)pFFrame->wSP;
    vcContext.EFlags = (ULONG)pFFrame->wFlags;

    if ( (getMSW() & MSW_PE) == 0 ) {
        vcContext.EFlags |= V86FLAGS_V86;
    }
#endif

    if ( fDebugged ) {
        fResult = SendVDMEvent(DBG_GPFAULT);

        if ( !fResult ) {
            DWORD dw;

            dw = SetErrorMode(0);
            try {
                RaiseException((DWORD)DBG_CONTROL_BREAK, 0, 0, (LPDWORD)0);
                fResult = TRUE;
            } except (EXCEPTION_EXECUTE_HANDLER) {
                fResult = FALSE;
            }
            SetErrorMode(dw);
        }

    } else {
        char    text[100];

        // Dump a simulated context

        OutputDebugString("NTVDM:GP Fault detected, register dump follows:\n");

        wsprintf(text,"eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n",
            vcContext.Eax,
            vcContext.Ebx,
            vcContext.Ecx,
            vcContext.Edx,
            vcContext.Esi,
            vcContext.Edi  );
        OutputDebugString(text);

        wsprintf(text,"eip=%08lx esp=%08lx ebp=%08lx iopl=%d         %s %s %s %s %s %s %s %s\n",
            vcContext.Eip,
            vcContext.Esp,
            vcContext.Ebp,
            (vcContext.EFlags & V86FLAGS_IOPL) >> V86FLAGS_IOPL_BITS,
            (vcContext.EFlags & V86FLAGS_OVERFLOW ) ? "ov" : "nv",
            (vcContext.EFlags & V86FLAGS_DIRECTION) ? "dn" : "up",
            (vcContext.EFlags & V86FLAGS_INTERRUPT) ? "ei" : "di",
            (vcContext.EFlags & V86FLAGS_SIGN     ) ? "ng" : "pl",
            (vcContext.EFlags & V86FLAGS_ZERO     ) ? "zr" : "nz",
            (vcContext.EFlags & V86FLAGS_AUXCARRY ) ? "ac" : "na",
            (vcContext.EFlags & V86FLAGS_PARITY   ) ? "po" : "pe",
            (vcContext.EFlags & V86FLAGS_CARRY    ) ? "cy" : "nc" );
        OutputDebugString(text);

        wsprintf(text,"cs=%04x  ss=%04x  ds=%04x  es=%04x  fs=%04x  gs=%04x             efl=%08lx\n",
            (WORD)vcContext.SegCs,
            (WORD)vcContext.SegSs,
            (WORD)vcContext.SegDs,
            (WORD)vcContext.SegEs,
            (WORD)vcContext.SegFs,
            (WORD)vcContext.SegGs,
            vcContext.EFlags );
        OutputDebugString(text);
    }

#ifdef MONITOR
    //
    // On x86 systems, we really might have some data in the FS and GS
    // registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to restore them.
    //
    px86->SegGs = (WORD)vcContext.SegGs;
    px86->SegFs = (WORD)vcContext.SegFs;
#else
    // No need to set FS,GS, they don't exist
#endif

    pFFrame->wES = (WORD)vcContext.SegEs;
    pFFrame->wDS = (WORD)vcContext.SegDs;
    pFFrame->wCS = (WORD)vcContext.SegCs;
    pFFrame->wSS = (WORD)vcContext.SegSs;

#ifdef MONITOR
    //
    // On x86 systems, we really might have some data in the high words
    // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to restore them.
    //
    pFFrame->wDI = LOWORD(vcContext.Edi);
    px86->Edi = MAKELONG(LOWORD(px86->Edi),HIWORD(vcContext.Edi));

    pFFrame->wSI = LOWORD(vcContext.Esi);
    px86->Esi = MAKELONG(LOWORD(px86->Esi),HIWORD(vcContext.Esi));

    pFFrame->wBX = LOWORD(vcContext.Ebx);
    px86->Ebx = MAKELONG(LOWORD(px86->Ebx),HIWORD(vcContext.Ebx));

    pFFrame->wDX = LOWORD(vcContext.Edx);
    px86->Edx = MAKELONG(LOWORD(px86->Edx),HIWORD(vcContext.Edx));

    pFFrame->wCX = LOWORD(vcContext.Ecx);
    px86->Ecx = MAKELONG(LOWORD(px86->Ecx),HIWORD(vcContext.Ecx));

    pFFrame->wAX = LOWORD(vcContext.Eax);
    px86->Eax = MAKELONG(LOWORD(px86->Eax),HIWORD(vcContext.Eax));

    pFFrame->wBP = LOWORD(vcContext.Ebp);
    px86->Ebp = MAKELONG(LOWORD(px86->Ebp),HIWORD(vcContext.Ebp));

    pFFrame->wIP = LOWORD(vcContext.Eip);
    px86->Eip = MAKELONG(LOWORD(px86->Eip),HIWORD(vcContext.Eip));

    pFFrame->wFlags = LOWORD(vcContext.EFlags);
    px86->EFlags = MAKELONG(LOWORD(px86->EFlags),HIWORD(vcContext.EFlags));

    pFFrame->wSP = LOWORD(vcContext.Esp);
    px86->Esp = MAKELONG(LOWORD(px86->Esp),HIWORD(vcContext.Esp));
#else
    pFFrame->wDI = (WORD)vcContext.Edi;
    pFFrame->wSI = (WORD)vcContext.Esi;
    pFFrame->wBX = (WORD)vcContext.Ebx;
    pFFrame->wDX = (WORD)vcContext.Edx;
    pFFrame->wCX = (WORD)vcContext.Ecx;
    pFFrame->wAX = (WORD)vcContext.Eax;


    pFFrame->wBP = (WORD)vcContext.Ebp;
    pFFrame->wIP = (WORD)vcContext.Eip;
    pFFrame->wFlags = (WORD)vcContext.EFlags;
    pFFrame->wSP = (WORD)vcContext.Esp;
#endif

    return( fResult );
}

BOOL DbgDivOverflow2(
    PTFRAME16   pTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {

        DbgGetContext();

        vcContext.SegDs = (ULONG)pTFrame->wDS;
        vcContext.SegCs = (ULONG)pTFrame->wCS;
        vcContext.SegSs = (ULONG)pTFrame->wSS;

#ifdef MONITOR
        //
        // On x86 systems, we really might have some data in the high words
        // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to recover them.
        //
        vcContext.Eax    = MAKELONG(pTFrame->wAX,   HIWORD(px86->Eax   ));
        vcContext.Eip    = MAKELONG(pTFrame->wIP,   HIWORD(px86->Eip   ));
        vcContext.Esp    = MAKELONG(pTFrame->wSP,   HIWORD(px86->Esp   ));
        vcContext.EFlags = MAKELONG(pTFrame->wFlags,HIWORD(px86->EFlags));
#else
        vcContext.Eax    = (ULONG)pTFrame->wAX;

        vcContext.Eip    = (ULONG)pTFrame->wIP;
        vcContext.Esp    = (ULONG)pTFrame->wSP;
        vcContext.EFlags = (ULONG)pTFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif

        fResult = SendVDMEvent( DBG_DIVOVERFLOW );

#ifdef MONITOR
        //
        // On x86 systems, we really might have some data in the FS and GS
        // registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to restore them.
        //
        px86->SegGs = vcContext.SegGs;
        px86->SegFs = vcContext.SegFs;
#else
        // No need to set FS,GS, they don't exist
#endif

        setES( (WORD)vcContext.SegEs );
        pTFrame->wDS = (WORD)vcContext.SegDs;
        pTFrame->wCS = (WORD)vcContext.SegCs;
        pTFrame->wSS = (WORD)vcContext.SegSs;

#ifdef MONITOR
        //
        // On x86 systems, we really might have some data in the high words
        // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to restore them.
        //
        setEDI( vcContext.Edi );
        setESI( vcContext.Esi );
        setEBX( vcContext.Ebx );
        setEDX( vcContext.Edx );
        setECX( vcContext.Ecx );

        pTFrame->wAX = LOWORD(vcContext.Eax);
        px86->Eax = MAKELONG(LOWORD(px86->Eax),HIWORD(vcContext.Eax));

        setEBP( vcContext.Ebp );

        pTFrame->wIP = LOWORD(vcContext.Eip);
        px86->Eip = MAKELONG(LOWORD(px86->Eip),HIWORD(vcContext.Eip));

        pTFrame->wFlags = LOWORD(vcContext.EFlags);
        px86->EFlags = MAKELONG(LOWORD(px86->EFlags),HIWORD(vcContext.EFlags));

        pTFrame->wSP = LOWORD(vcContext.Esp);
        px86->Esp = MAKELONG(LOWORD(px86->Esp),HIWORD(vcContext.Esp));
#else
        setDI( (WORD)vcContext.Edi );
        setSI( (WORD)vcContext.Esi );
        setBX( (WORD)vcContext.Ebx );
        setDX( (WORD)vcContext.Edx );
        setCX( (WORD)vcContext.Ecx );
        pTFrame->wAX = (WORD)vcContext.Eax;


        setBP( (WORD)vcContext.Ebp );
        pTFrame->wIP    = (WORD)vcContext.Eip;
        pTFrame->wFlags = (WORD)vcContext.EFlags;
        pTFrame->wSP    = (WORD)vcContext.Esp;
#endif


    }

    return( fResult );
}


BOOL DbgDllStart(
    PNDFRAME16  pNDFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        IMAGE_NOTE      im;

        DbgGetContext();

        EventParams[2] = (DWORD)&im;

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNDFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNDFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pNDFrame->hModule;
        im.hTask   = pNDFrame->hTask;

        fResult = SendVDMEvent( DBG_DLLSTART );

        if (VdmDbgTraceFlags & VDMDBG_BREAK_LOADDLL)
            DbgSetTemporaryBP(pNDFrame->wIP, pNDFrame->wAX, ISPESET);
    }

    return( fResult );
}


BOOL DbgTaskStop(
    PSTFRAME16  pSTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        IMAGE_NOTE      im;

        DbgGetContext();

        EventParams[2] = (DWORD)&im;

        // The code in TASK.ASM pops the frame off the stack before it IRETs
        vcContext.Esp += sizeof(STFRAME16);

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pSTFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pSTFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pSTFrame->hModule;
        im.hTask   = pSTFrame->hTask;

        fResult = SendVDMEvent( DBG_TASKSTOP );

        // See comment about what the code does above
        vcContext.Esp -= sizeof(STFRAME16);
    }

    return( fResult );
}









//
// I looked through the Win 3.0/Win 3.1 sources, and all the SDM_... commands
// I could find were this:   (The only ones we support for now will be
//                             SDM_LOADSEG,SDM_MOVESEG,SDM_FREESEG/RELEASESEG)
//
// ax = SDM_LOADSEG     =  50h
//       es:di = Module Name
//       bx    = segment number
//       cx    = loaded seg
//       dx    = instance number
//       si    = DataOrCodeSegment
// ax = SDM_MOVESEG     =  51h
//       push  destseg
//       push  sourceseg
//       push  SDM_MOVESEG
// SDM_FREESEG     =  52h
//       bx    = segment addr
// SDM_RELEASESEG  =  5Ch
//       bx    = segment addr
// SDM_5A (DebugDebug)
//       cx    = dataoffset hGlobalHeap
//       dx    = ds (SetKernelDS)
// SDM_4F (DebugInit)
//       push  SDM_4F
// SDM_0 (Out Char)
//       dl    = character to output
// SDM_0F (Out Symbol)
//       dx    = offset
//       cx    = selector
//       es    = data segment of DOSX
// SDM_GLOBALHEAP  =  3 (real mode only)
//       push  pGlobalHeap
//       push  ax
// SDM_CONWRITE    =  12h
//       ds:si = Pointer to string
//       cx    = length of string
// SDM_CONREAD     =  1
//       nothing, returns character read in al
// SDM_DGH         =  56h
//       cx    = seg DumpGlobalHeap
//       bx    = codeoffset DumpGlobalHeap
// SDM_DFL         =  57h
//       cx    = seg DumpFreeList
//       bx    = codeoffset DumpFreeList
// SDM_DLL         =  58h
//       cx    = seg DumpLRUList
//       bx    = codeoffset DumpLRUList
// SDM_LOADTASK    =  59h
//       not used
// SDM_POSTLOAD    =  60h
//       push    ax
// SDM_EXITCALL    =  62h
//       push    ax
// SDM_INT2        =  63h (Ctrl-alt-sysreq)
//       nothing.
// SDM_LOADDLL     =  64h
//       not used
// SDM_DELMODULE   =  65h
//       push    module handle
// SDM_RIN         =  9
//       push    seg ReplaceInst
//       push    codeoffset ReplaceInst
//       push    ax
// SDM_BANKLINE    = 10
//       push    EMS_calc_swap_line
//       push    ax
// SDM_NEWTASK     = 11
//       push   EMS PID
//       push   ax
// SDM_FLUSHTASK   = 12
//       push   EMS PID
//       push   ax
// SDM_SWITCHOUT   = 13
//       ds    = TDB
// SDM_SWITCHIN    = 14
//       ds    = TDB
// SDM_KEYBOARD    = 15
//       not used (thankfully, it is the same number as SDM_0F)
// SDM_MAXFUNC     = 15
//       not used

void xxxDbgDispatch()
{
    UNALIGNED WORD  *stack;
    WORD            mode;
    WORD            selector;
    WORD            segment;
    WORD            new_selector;
    BOOL            fBPRelease;
    BOOL            fData;
    LPSTR           lpModuleName;
    LPSTR           lpPathName;
    UCHAR           fPE;
    PFFRAME16       pFFrame;
    PTFRAME16       pTFrame;
    PNDFRAME16      pNDFrame;
    PSTFRAME16      pSTFrame;
    WORD            wFrame;

    fPE = ISPESET;

    stack = (UNALIGNED WORD *)Sim32GetVDMPointer(
                        ((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );

    mode = *stack++;
    switch( mode ) {
        case DBG_SEGLOAD:
            selector = *stack++;
            segment  = *stack++;
            lpModuleName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            stack += 2;
            lpPathName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            if ( lpPathName == NULL ) {
                lpPathName = "";
            }

            stack += 2;
            fData = (BOOL)(*stack++);
            SegmentLoad( lpModuleName, lpPathName, selector, segment, fData );
            break;
        case DBG_SEGMOVE:
            selector = *stack++;
            new_selector = *stack++;
            SegmentMove( selector, new_selector );
            break;
        case DBG_SEGFREE:
            fBPRelease = (BOOL)*stack++;
            selector = *stack++;
            SegmentFree( selector, fBPRelease );
            break;
        /*
        case DBG_MODFREE:
            lpModuleName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            stack += 2;
            lpPathName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            if ( lpPathName == NULL ) {
                lpPathName = "";
            }
            ModuleFree( lpModuleName, lpPathName );
            break;
        case DBG_MODLOAD:

            // Why doesn't this do anything? - See JonLe

            break;

        case DBG_SINGLESTEP:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = SingleStep( pTFrame );

            setAX((WORD)fData);
            break;

        case DBG_BREAK:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = Breakpoint( pTFrame );

            setAX((WORD)fData);
            break;
        */
        case DBG_GPFAULT2:
            wFrame = getBP() - (WORD)(FIELD_OFFSET(FFRAME16,wBP));

            pFFrame = (PFFRAME16)Sim32GetVDMPointer(
                        ((ULONG)getSS() << 16) + (ULONG)wFrame,
                        MAX_DBG_FRAME, fPE );


            fData = DbgGPFault2( pFFrame );

            setAX((WORD)fData);
            break;

        case DBG_DIVOVERFLOW:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = DbgDivOverflow2( pTFrame );

            setAX((WORD)fData);
            break;
        case DBG_DLLSTART:
            pNDFrame = (PNDFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = DbgDllStart( pNDFrame );

            setAX((WORD)fData);
            break;
        case DBG_TASKSTOP:
            pSTFrame = (PSTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );

            fData = DbgTaskStop( pSTFrame );
            break;
        case DBG_ATTACH:
            break;

        case DBG_TOOLHELP:
            ulTHHOOK = (ULONG)*stack + ((ULONG)(*(stack+1)) << 16);
            stack += 2;
            f386 = (BOOL)*stack;
            break;

        case DBG_WOWINIT:
            DbgWowhExeHead = getDX();
            DbgWowhGlobalHeap = getCX();
            break;

        default:
            setAX(0);       // Event not handled
            break;
    }
}

VOID xxxDbgNotifyNewTask(
    LPVOID    lpvNTFrame,
    UINT    uFrameSize
) {
    BOOL        fResult;
    PNTFRAME16    pNTFrame;

    pNTFrame = (PNTFRAME16)lpvNTFrame;

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        IMAGE_NOTE      im;

        DbgGetContext();

        EventParams[2] = (DWORD)&im;

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNTFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNTFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pNTFrame->hModule;
        im.hTask   = pNTFrame->hTask;

        fResult = SendVDMEvent( DBG_TASKSTART );

        if (VdmDbgTraceFlags & VDMDBG_BREAK_WOWTASK)
        {
            DbgSetTemporaryBP(pNTFrame->wCS, pNTFrame->wIP, ISPESET);
        }
    }
}


VOID xxxDbgNotifyRemoteThreadAddress(
    LPVOID  lpAddress,
    DWORD   lpBlock
) {
    lpRemoteAddress = lpAddress;
    lpRemoteBlock   = lpBlock;
}

VOID xxxDbgNotifyDebugged(
    BOOL    fNewDebugged
) {
    fDebugged = fNewDebugged;
}

BOOL xxxDbgIsDebuggee()
{
  return fDebugged;
}

VOID RestoreVDMContext(
    VDMCONTEXT *pContext
) {
  setEAX(pContext->Eax);
  setEBX(pContext->Ebx);
  setECX(pContext->Ecx);
  setEDX(pContext->Edx);
  setEIP(pContext->Eip);
  setESP(pContext->Esp);
  setEBP(pContext->Ebp);
  setEFLAGS(pContext->EFlags);
}

#pragma warning( disable: 4035 )    /* Don't complain about lack of return value */

__inline __int64 GetCycleCount ()
{
__asm   _emit   0x0F
__asm   _emit   0x31    /* rdtsc */
    // return EDX:EAX       causes annoying warning
};

ULONG DbgEventTime(
    VDM_TRACEINFO *pTraceInfo
) {

    __int64 PerformanceCount, Diff;
    DWORD Ticks, DiffTicks;

  if ( !DbgTimerInitialized )
  {
    int cpu;

    __asm {
        mov eax, 1
        _emit 0x0F            ; pentium cpuid instruction
        _emit 0xA2
        mov cpu, edx
    }
    if ( cpu & 0x10 )
      DbgTimerMode = VDMTI_TIMER_PENTIUM;
    switch (DbgTimerMode & VDMTI_TIMER_MODE)
    {
    case VDMTI_TIMER_TICK:
        pTraceInfo->TimeStamp.LowPart = GetTickCount();
        DbgTimerInitialized = TRUE;
        break;
    case VDMTI_TIMER_STAT:
    {
        PerformanceCount = GetCycleCount();
        pTraceInfo->TimeStamp.LowPart = LODWORD(PerformanceCount);
        pTraceInfo->TimeStamp.HighPart = HIDWORD(PerformanceCount);
        DbgTimerInitialized = 1;
        break;
    }
    default:
        if ( !DbgTimerInitialized )
          return 0;
    }
  }
  pTraceInfo->Flags = pTraceInfo->Flags ^ (DbgTimerMode ^ pTraceInfo->Flags) & VDMTI_TIMER_MODE;
  switch (pTraceInfo->Flags & VDMTI_TIMER_MODE)
  {
  case VDMTI_TIMER_STAT:
      PerformanceCount = GetCycleCount();
      Diff = PerformanceCount - pTraceInfo->TimeStamp.QuadPart;
      pTraceInfo->TimeStamp.LowPart = LODWORD(PerformanceCount);
      pTraceInfo->TimeStamp.HighPart = HIDWORD(PerformanceCount);
      return LODWORD(Diff);
  case VDMTI_TIMER_TICK:
      Ticks = GetTickCount();
      if (Ticks <= pTraceInfo->TimeStamp.LowPart)
          return 0;
      DiffTicks = Ticks - pTraceInfo->TimeStamp.LowPart;
      pTraceInfo->TimeStamp.LowPart = Ticks;
      return DiffTicks;
  }
  return 0;
}

void xxxDbgTraceEvent(
    VDM_TRACEINFO *pTraceInfo, 
    WORD Type, 
    WPARAM wData, 
    LPARAM lData
) {
  PVDM_TRACEENTRY pTraceEntry;

  if (*pNtVDMState & VDM_TRACE_HISTORY)
  {
    if ( !pTraceInfo->pTraceTable )
    {
      pTraceInfo->pTraceTable = (struct _VdmTraceEntry *)VirtualAlloc(0, 0x8000u, MEM_COMMIT, PAGE_READWRITE);
      if ( !pTraceInfo->pTraceTable )
        return;
      pTraceInfo->CurrentEntry = 0;
      pTraceInfo->NumPages = 8;
      pTraceInfo->Flags = 0;
    }
    pTraceEntry = &pTraceInfo->pTraceTable[pTraceInfo->CurrentEntry];
    pTraceEntry->Type = Type;
    pTraceEntry->wData = (USHORT)wData;
    pTraceEntry->lData = lData;
    pTraceEntry->Time = DbgEventTime(pTraceInfo);
    pTraceEntry->eax = getEAX();
    pTraceEntry->ebx = getEBX();
    pTraceEntry->ecx = getECX();
    pTraceEntry->edx = getEDX();
    pTraceEntry->esi = getESI();
    pTraceEntry->edi = getEDI();
    pTraceEntry->ebp = getEBP();
    pTraceEntry->esp = getESP();
    pTraceEntry->eip = getEIP();
    pTraceEntry->eflags = getEFLAGS();
    pTraceEntry->cs = getCS();
    pTraceEntry->ds = getDS();
    pTraceEntry->es = getES();
    pTraceEntry->fs = getFS();
    pTraceEntry->gs = getGS();
    pTraceEntry->ss = getSS();
    if ( ++pTraceInfo->CurrentEntry >= (pTraceInfo->NumPages * sizeof(VDM_TRACEENTRY)) )
      pTraceInfo->CurrentEntry = 0;
  }
}

BOOL xxxDbgInit(
    LPVOID VdmState, 
    DWORD TraceFlags, 
    LPVOID NtCpuInfo
) {
  VdmDbgTraceFlags = TraceFlags;
  lpVdmState = VdmState;
  IntelMemoryBase = (DWORD)VdmMapFlat(0, 0, 0);
  lpNtCpuInfo = NtCpuInfo;
  *(DWORD *)lpVdmState |= VDM_BREAK_DEBUGGER;
  if ( fDebugged )
  {
    DbgGetContext();
    SendVDMEvent(DBG_INIT);
  }
  return TRUE;
}

BOOL xxxDbgBPInt()
{
  ULONG Eip;
  BYTE *pOpcode;
  int i;

  if ( fDebugged )
  {
    DbgGetContext();

    if ( !(getMSW() & MSW_PE) || !ExpLdt[vcContext.SegCs >> 3].HighWord.Bits.Default_Big )
      Eip = LOWORD(vcContext.Eip);
    else
      Eip = vcContext.Eip;
    for (i=0; !(VdmBreakPoints[i].Flags & VDMBP_ENABLED)
         || !(VdmBreakPoints[i].Flags & VDMBP_SET)
         || vcContext.SegCs != VdmBreakPoints[i].Seg
         || Eip != VdmBreakPoints[i].Offset + 1
         || ((BYTE)~((ULONG)VdmBreakPoints[i].Flags >> 4) ^ (BYTE)getMSW()) & MSW_PE;
         i++)
    {
      if ( i>=sizeof(VdmBreakPoints)/sizeof(VdmBreakPoints[0]) )
      {
        OutputDebugStringA("VDM: Unexpected breakpoint hit\n");
        SendVDMEvent(DBG_BREAK);
        bWantsTraceInteractive = vcContext.EFlags & V86FLAGS_TRACE;
        RestoreVDMContext(&vcContext);
        return TRUE;
      }
    }
    setEIP(getEIP() - 1);
    --vcContext.Eip;
    pOpcode = (BYTE *)VdmMapFlat(
                    VdmBreakPoints[i].Seg,
                    VdmBreakPoints[i].Offset,
                    !(VdmBreakPoints[i].Flags & VDMBP_V86));
    if ( pOpcode && *pOpcode == 0xCCu )
    {
        *pOpcode = VdmBreakPoints[i].Opcode;
        VdmBreakPoints[i].Flags = VdmBreakPoints[i].Flags & (~VDMBP_FLUSH) | VDMBP_PENDING;
        if ( i == 0 )
          VdmBreakPoints[0].Flags = VdmBreakPoints[i].Flags & (~VDMBP_SET);
    }
    SendVDMEvent(DBG_BREAK);
    bWantsTraceInteractive = vcContext.EFlags & V86FLAGS_TRACE;
    if ( bWantsTraceInteractive || i )
      vcContext.EFlags |= V86FLAGS_TRACE;
    RestoreVDMContext(&vcContext);
    return TRUE;
  }
  return FALSE;
}

BOOL xxxDbgTraceInt()
{
  BYTE *pOpcode;
  BOOL bReturn = FALSE;
  int i;

  if ( fDebugged )
  {
    DbgGetContext();
    setEFLAGS(vcContext.EFlags & (~V86FLAGS_TRACE));

    for (i=0; i<sizeof(VdmBreakPoints)/sizeof(VdmBreakPoints[0]); i++)
    {
      if (VdmBreakPoints[i].Flags & VDMBP_ENABLED && 
          VdmBreakPoints[i].Flags & VDMBP_SET && 
          VdmBreakPoints[i].Flags & VDMBP_PENDING )
      {
        pOpcode = (BYTE *)VdmMapFlat(VdmBreakPoints[i].Seg, 
                    VdmBreakPoints[i].Offset, 
                    !(VdmBreakPoints[i].Flags & VDMBP_V86));
        if ( pOpcode ) *pOpcode = 0xCCu;
        VdmBreakPoints[i].Flags &= ~(VDMBP_FLUSH | VDMBP_PENDING);
		bReturn = TRUE;
      }
    }
    if ( bWantsTraceInteractive )
    {
      SendVDMEvent(DBG_BREAK);
      RestoreVDMContext(&vcContext);
      bWantsTraceInteractive = vcContext.EFlags & V86FLAGS_TRACE;
    }
    else if ( !bReturn )
    {
      OutputDebugStringA("VDM: Unexpected trace interrupt\n");
      SendVDMEvent(DBG_BREAK);
      bWantsTraceInteractive = vcContext.EFlags & V86FLAGS_TRACE;
      RestoreVDMContext(&vcContext);
    }
    return TRUE;
  }
  return FALSE;
}

BOOL xxxDbgFault(
    ULONG Event
) {
  if ( fDebugged && Event != 6 )
  {
    if ( Event == 12 )
    {
      if ( *pNtVDMState & VDM_BREAK_EXCEPTIONS )
      {
        DbgGetContext();
        return SendVDMEvent(DBG_STACKFAULT);
      }
    }
    else if ( Event == 13 )
    {
      if ( *pNtVDMState & VDM_BREAK_EXCEPTIONS )
      {
        DbgGetContext();
        return SendVDMEvent(DBG_GPFAULT);
      }
    }
  }
  return FALSE;
}

void xxxDbgDosAppStart(
    WORD wSeg, 
    WORD wOffset
) {
  if ( fDebugged )
  {
    if (VdmDbgTraceFlags & VDMDBG_BREAK_DOSTASK)
      DbgSetTemporaryBP(wSeg, wOffset, FALSE);
  }
}

void xxxDbgSegmentNotice(
    WORD wType, 
    WORD wModuleSeg, 
    WORD wLoadSeg, 
    WORD wNewSeg, 
    LPSTR lpModuleName, 
    LPSTR lpModulePath, 
    DWORD dwImageLen
) {
  if ( fDebugged )
  {
    switch ( wType )
    {
      case DBG_MODLOAD:
        ModuleLoad(lpModuleName, lpModulePath, wLoadSeg, dwImageLen);
        break;
      case DBG_MODFREE:
        ModuleFree(lpModuleName, lpModulePath);
        break;
      case DBG_SEGMOVE:
        ModuleSegmentMove(lpModuleName, lpModulePath, wModuleSeg, wLoadSeg, wNewSeg, dwImageLen);
        break;
    }
  }
}

