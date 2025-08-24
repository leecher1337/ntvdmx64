/*
 * SoftPC Revision 3.0
 *
 * Title    : vdmexts.c
 *
 * Description  : Glue code between vdmexts.dll and YODA 
 *
 * Author   : Han (leecher1337) Solo
 *
 * Notes    : It's not my fault.
 *
 */
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501  // Vectored exception handlers
#include "insignia.h"
#include "host_def.h"

#ifdef YODA
#include "xt.h"
#define CPU_PRIVATE
#include CpuH
#include <ntsdexts.h>
#include <stdio.h>
#include <malloc.h>
#include <imagehlp.h>
#include <vdmdbg.h>
#include <dbgexp.h>
#include "yoda.h"
#include "trace.h"
#include "dbginfo.h"

#ifdef SPC386
#define DASM_INTERNAL
#include <dasm.h>
#else /* SPC386 */
IMPORT word dasm IPT5(char *, i_output_stream, word, i_atomicsegover, word, i_segreg, word, i_segoff, int, i_nInstr);
#endif /* SPC386 */

#define SYMBOL_PATH "_NT_SYMBOL_PATH"
#define ALTERNATE_SYMBOL_PATH "_NT_ALT_SYMBOL_PATH"
#define SYMBOLSIZE      256

#define dprintf printf

/* Imports */
extern ULONG dmaGetAdaptor();

#define ES_YODA         8
extern int EventStatus;
extern BOOL bDbgDebuggerLoaded;
extern VOID VdmDbgAttach(VOID);
extern VOID DBGNotifyDebugged(BOOL fNewDebugged);

/* These are the correct types for reference
extern DMA_ADAPT *dmaGetAdaptor();
extern PDOSSF pSFTHead;
extern PUSHORT pusCurrentPDB;
extern VDM_INTERRUPTHANDLER DpmiInterruptHandlers[256];
extern VDM_FAULTHANDLER DpmiFaultHandlers[32];
extern RMCB_INFO DpmiRmcb[MAX_RMCBS];
extern USHORT RMCallBackBopSeg;
extern XMEM_BLOCK  XmemHead;
extern LDT_ENTRY *Ldt;
extern ULONG InitialVdmDbgFlags;
extern ULONG InitialVdmTibFlags;
extern VDMVIRTUALICA VirtualIca[];
extern PVDM_TRACEINFO pVdmTraceInfo;
extern NT_CPU_INFO nt_cpu_info;
*/

/* Types */
typedef struct _segentry {
    int     type;
    LPSTR   path_name;
    WORD    selector;
    WORD    segment;
    DWORD   ImgLen;    // MODLOAD only
} SEGENTRY;

#define SEGTYPE_AVAILABLE   0
#define SEGTYPE_V86         1
#define SEGTYPE_PROT        2

#define MAXSEGENTRY 1024

/* Globals */
SEGENTRY segtable[MAXSEGENTRY];
BOOL (WINAPI *pfnVDMProcessException)(LPDEBUG_EVENT);
BOOL (WINAPI *pfnVDMGetSelectorModule)(HANDLE,HANDLE,WORD,PUINT,LPSTR, UINT,LPSTR, UINT);
BOOL fVDMInitDone = FALSE;
BOOL fVDMActive = FALSE;

/* Locals */
static const char *DebuggerName = "YODA";
static NTSD_EXTENSION_APIS NtsdExtensions = {0};
static HANDLE hProcess, hModule = NULL;
static IMAGEHLP_SYMBOL current_sym = {0};
static PIMAGEHLP_SYMBOL sym = &current_sym;

UCHAR   chSymbolSuffix = 'a';   //  suffix to add to symbol if search
                                //  failure - 'n'-none 'a'-'w'-append

static BOOL GetOffsetFromSym(PUCHAR pString, PULONG pOffset, CHAR iModule );
BOOL VdmExtCommand(char *pname, char *args);

// getenv() in NTVDM is an internal function, that parses ntvdm environment, 
// so we have a static verison here that parses public Environment
static char *getenv( const char *varname )
{
    static char result[1024];

    if (!GetEnvironmentVariable(varname, result, sizeof(result)))
        return NULL;
    return result;
}

static ULONG
GetExpressionRoutine(char * CommandString)
{
    ULONG value;

    // First check, if it's an internal NTVDM symbol, we can use a shortcut here
    if (strncmp(CommandString, "ntvdm!", 6) == 0)
    {
        CommandString += 6;

        if (_stricmp(CommandString, "adaptor") == 0)
            return dmaGetAdaptor();

#define _X(k) { extern ULONG k; if (_stricmp(CommandString, #k) == 0) return (ULONG)&k; }
        _X(pSFTHead);
        _X(pusCurrentPDB);
        _X(DpmiInterruptHandlers);
        _X(DpmiFaultHandlers); 
#ifdef NEW_DPMIHOST // We are not using 'new' DPMI
        _X(DpmiRmcb);
        _X(RMCallBackBopSeg);
#endif
        _X(XmemHead);
        //_X(DosMemHead);
        _X(Ldt);
        _X(InitialVdmDbgFlags);
        _X(InitialVdmTibFlags);
        _X(VirtualIca);
        _X(pVdmTraceInfo);
        _X(nt_cpu_info);
        _X(Start_of_M_area);
#undef _X

        CommandString -= 6;
    }
    
    if ( strcmp(CommandString,"WOW_BIG_BDE_HACK") == 0 ) {
        return( (ULONG)(&segtable[0]) );
    }

    if (strchr(CommandString, '!') && GetOffsetFromSym(CommandString, &value, 0))
        return value;

    if (sscanf(CommandString, "%x", &value))
        return value;

    return 0;
}

static BOOL
GetOffsetFromSym(
    PUCHAR  pString,
    PULONG  pOffset,
    CHAR    iModule
    )
{
    UCHAR   SuffixedString[SYMBOLSIZE + 64];
    UCHAR   Suffix[4];

    if ( pString[0] == 0 || pString[1] == 0 ) return FALSE;

    if (SymGetSymFromName( hProcess, pString, sym )) {
        *pOffset = sym->Address;
        return TRUE;
    }

    if (chSymbolSuffix != 'n') {
        strcpy( SuffixedString, pString );
        Suffix[0] = chSymbolSuffix;
        Suffix[1] = '\0';
        strcat( SuffixedString, Suffix );
        if (SymGetSymFromName( hProcess, SuffixedString, sym )) {
            *pOffset = sym->Address;
            return TRUE;
        }
    }

    return FALSE;
}


static void
GetSymbolRoutine (
    ULONG_PTR offset,
    PUCHAR pchBuffer,
    ULONG_PTR *pDisplacement
    )
{
    IMAGEHLP_MODULE     mi;

    if (SymGetModuleInfo( hProcess, (ULONG)offset, &mi )) {
        if (SymGetSymFromAddr( hProcess, (ULONG)offset, pDisplacement, sym )) {
            sprintf( pchBuffer, "%s!%s", mi.ModuleName, sym->Name );
            return;
        }
    }
    *pchBuffer = 0;
    *pDisplacement = (ULONG)offset;
}

static DWORD disasmExportRoutine(ULONG_PTR *lpOffset, LPSTR lpBuffer, ULONG fShowEA)
{
    // This is utter nonsense, but vdmdbg ext shouldn't call it anyway

    // extern IU16 dasm IPT4(char *, txt, IU16, seg, LIN_ADDR, off, SIZE_SPECIFIER, default_size);
    dasm(lpBuffer, (IU16)(*lpOffset >> 4), (USHORT)*lpOffset, fShowEA?THIRTY_TWO_BIT:SIXTEEN_BIT);
    return TRUE;
}

static DWORD CheckControlC (VOID)
{
    // This is nonsense either, but it also shouldn't get called 

    if (EventStatus & ES_YODA) 
    {
        EventStatus &= ~ES_YODA;
        return 1;
    }
    return 0;
}

BOOL
SymbolCallbackFunction(
    HANDLE  hProcess,
    ULONG   ActionCode,
    PVOID   CallbackData,
    PVOID   UserContext
    )
{
    PIMAGEHLP_DEFERRED_SYMBOL_LOAD  idsl;
    IMAGEHLP_MODULE                 mi;


    switch( ActionCode ) {
        case CBA_DEFERRED_SYMBOL_LOAD_START:
            idsl = (PIMAGEHLP_DEFERRED_SYMBOL_LOAD) CallbackData;
            _strlwr( idsl->FileName );
            dprintf( "Loading symbols for 0x%08x %16s ->   ",
                idsl->BaseOfImage,
                idsl->FileName
                );
            return TRUE;

        case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
            idsl = (PIMAGEHLP_DEFERRED_SYMBOL_LOAD) CallbackData;
            dprintf( "*** Error: could not load symbols\n" );
            break;

        case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
            idsl = (PIMAGEHLP_DEFERRED_SYMBOL_LOAD) CallbackData;
            dprintf( "%s\n", idsl->FileName );
            if (SymGetModuleInfo( hProcess, idsl->BaseOfImage, &mi )) {
                if (mi.SymType == SymNone) {
                    dprintf( "*** ERROR: Module load completed but symbols could not be loaded for %s\n",
                        idsl->FileName);
                }
                return TRUE;
            }
            dprintf( "\n" );
            break;

        case CBA_SYMBOLS_UNLOADED:
            idsl = (PIMAGEHLP_DEFERRED_SYMBOL_LOAD) CallbackData;
            dprintf( "Symbols unloaded for 0x%08x %s\n",
                idsl->BaseOfImage,
                idsl->FileName
                );
            break;

        default:
            return FALSE;
    }

    return FALSE;
}

static void
SetSymbolSearchPath(void)
{
    LPSTR lpSymPathEnv;
    LPSTR lpAltSymPathEnv;
    LPSTR lpSystemRootEnv;
    LPSTR lpSymPath;
    ULONG cbSymPath;
    ULONG dw;
    CHAR  *SymbolSearchPath;

    cbSymPath = 18;
    if (lpSymPathEnv = getenv(SYMBOL_PATH)) {
        cbSymPath += strlen(lpSymPathEnv) + 1;
    }
    if (lpAltSymPathEnv = getenv(ALTERNATE_SYMBOL_PATH)) {
        cbSymPath += strlen(lpAltSymPathEnv) + 1;
    }

    if (lpSystemRootEnv = getenv("SystemRoot")) {
        cbSymPath += (strlen(lpSystemRootEnv) + 1) * 2 + 10;
    }

    SymbolSearchPath = calloc(cbSymPath, 1);

    if (lpAltSymPathEnv) {
        lpAltSymPathEnv = _strdup(lpAltSymPathEnv);
        lpSymPath = strtok(lpAltSymPathEnv, ";");
        while (lpSymPath) {
            dw = GetFileAttributes(lpSymPath);
            if ( (dw != 0xffffffff) && (dw & FILE_ATTRIBUTE_DIRECTORY) ) {
                if (*SymbolSearchPath) {
                    strcat(SymbolSearchPath, ";");
                }
                strcat(SymbolSearchPath, lpSymPath);
            }
            lpSymPath = strtok(NULL, ";");
        }
        free(lpAltSymPathEnv);
    }

    if (lpSymPathEnv) {
        lpSymPathEnv = _strdup(lpSymPathEnv);
        lpSymPath = strtok(lpSymPathEnv, ";");
        while (lpSymPath) {
            dw = GetFileAttributes(lpSymPath);
            if ( (dw != 0xffffffff) && (dw & FILE_ATTRIBUTE_DIRECTORY) ) {
                if (*SymbolSearchPath) {
                    strcat(SymbolSearchPath, ";");
                }
                strcat(SymbolSearchPath, lpSymPath);
            }
            lpSymPath = strtok(NULL, ";");
        }
        free(lpSymPathEnv);
    }

    if (lpSystemRootEnv) {
        dw = GetFileAttributes(lpSystemRootEnv);
        if ( (dw != 0xffffffff) && (dw & FILE_ATTRIBUTE_DIRECTORY) ) {
            if (*SymbolSearchPath) {
                strcat(SymbolSearchPath, ";");
            }
            strcat(SymbolSearchPath,lpSystemRootEnv);
            strcat(SymbolSearchPath, ";");
            strcat(SymbolSearchPath,lpSystemRootEnv);
            strcat(SymbolSearchPath,"\\System32");
        }
    }

    SymSetSearchPath( hProcess, SymbolSearchPath );

    dprintf("Symbol search path is: %s\n",
            *SymbolSearchPath ?
               SymbolSearchPath :
               "*** Invalid *** : Verify _NT_SYMBOL_PATH setting" );
               
    free(SymbolSearchPath);
}

LONG
fnBangCommandExceptionFilter(
    struct _EXCEPTION_POINTERS *ExceptionInfo,
    LPSTR modname,
    LPSTR pname
    )
{
    dprintf("%s: %08x Exception in %s.%s debugger extension.\n",
            "YODA",
            ExceptionInfo->ExceptionRecord->ExceptionCode,
            modname,
            pname
           );

    dprintf("      PC: %08x  VA: %08x  R/W: %x  Parameter: %x\n",
            ExceptionInfo->ExceptionRecord->ExceptionAddress,
            ExceptionInfo->ExceptionRecord->ExceptionInformation[1],
            ExceptionInfo->ExceptionRecord->ExceptionInformation[0],
            ExceptionInfo->ExceptionRecord->ExceptionInformation[2]
           );

    return EXCEPTION_EXECUTE_HANDLER;
}

static
BOOL LoadVdmDBG(void)
{
    if ( !fVDMInitDone ) {
        HANDLE  hmodVDM;

        hmodVDM = LoadLibrary("VDMDBG.DLL");

        if ( hmodVDM != (HANDLE)NULL ) {
            pfnVDMProcessException = (BOOL (WINAPI *)(LPDEBUG_EVENT))
                GetProcAddress( hmodVDM, "VDMProcessException" );
            pfnVDMGetSelectorModule = (BOOL (WINAPI *)(HANDLE,HANDLE,WORD,PUINT,LPSTR,UINT,LPSTR,UINT))
                GetProcAddress( hmodVDM, "VDMGetSelectorModule" );
            
            fVDMActive = pfnVDMProcessException && pfnVDMGetSelectorModule;

        } else {
            dprintf("LoadLibrary(VDMDBG.DLL) failed\n");
			return FALSE;
        }
    }
	return TRUE;
}

static
BOOL VDMEvent(DEBUG_EVENT* pDebugEvent) 
{
    LPSTR           Str;
    LPSTR           pFileName;
    BOOL            b;
    DWORD           address;
    PULONG_PTR      lpdw;
    int             segslot;
    int             mode;
    BOOL            fData;
    WORD            selector;
    WORD            segment = 0;
    WORD            newselect;
    BOOL            fStop;
    DWORD           ImgLen;
    BOOL            fResult;
    BOOL            fNeedSegTableEdit;
    BOOL            fNeedInteractive;
    BOOL            fProcess;
    BOOL            fVerbose;
    SEGMENT_NOTE    se;
    IMAGE_NOTE      im;
    WORD            EventFlags;

	LoadVdmDBG();
	fVDMInitDone = TRUE;
    if ( !fVDMActive ) {
        return( TRUE );
    } else {
        fProcess = (*pfnVDMProcessException)(pDebugEvent);
    }

    lpdw = &(pDebugEvent->u.Exception.ExceptionRecord.ExceptionInformation[0]);
  
    fResult = TRUE;
    fNeedSegTableEdit = FALSE;
    fNeedInteractive = FALSE;
    fVerbose = TRUE; //FALSE;

    mode = LOWORD(lpdw[0]);
    EventFlags = HIWORD(lpdw[0]);

    // Has the caller explicitly asked for interaction?
    if (EventFlags & VDMEVENT_NEEDS_INTERACTIVE) {
        fNeedInteractive = TRUE;
    }
    if (EventFlags & VDMEVENT_VERBOSE) {
        fVerbose = TRUE;
    }


    switch( mode ) {
        case DBG_SEGLOAD:
        case DBG_SEGMOVE:
        case DBG_SEGFREE:
        case DBG_MODLOAD:
        case DBG_MODFREE:
            memcpy(&se, (LPVOID)lpdw[2], sizeof(se));
            break;
        case DBG_DLLSTART:
        case DBG_DLLSTOP:
        case DBG_TASKSTART:
        case DBG_TASKSTOP:
            memcpy(&im, (LPVOID)lpdw[2], sizeof(im));
            break;
    }

    switch( mode ) {
        default:
            fResult = FALSE;
            break;

        case DBG_SEGLOAD:
            fNeedSegTableEdit = TRUE;

            selector = se.Selector1;
            segment  = se.Segment;
            fData    = (BOOL)se.Type;

            segslot = 0;
            while ( segslot < MAXSEGENTRY ) {
                if ( segtable[segslot].type != SEGTYPE_AVAILABLE ) {
                    if ( _stricmp(segtable[segslot].path_name, se.FileName) == 0 ) {
                        break;
                    }
                }
                segslot++;
            }
            if ( segslot == MAXSEGENTRY ) {
                if ( strlen(se.FileName) != 0 ) {
                    dprintf("Loading [%s]\n", se.FileName );
                }
            }
            if (fVerbose) {
                dprintf("VDM SegLoad: %s(%x) %s => %x\n",
                                        se.FileName,
                                        segment,
                                        fData ? "Data" : "Code",
                                        selector);
            }
            break;
        case DBG_SEGMOVE:
            fNeedSegTableEdit = TRUE;
            selector  = se.Selector1;
            newselect = se.Selector2;
            segment  = se.Segment;
            pFileName = se.FileName;
            ImgLen = se.Length;
            
            if ( newselect == 0 ) {
                mode = DBG_SEGFREE;
            } else if (segment > 1) {
                //
                // real mode module is getting split up into different
                // segments, so create a new segtable entry
                //
                segslot = 0;
                while ( segslot < MAXSEGENTRY ) {
                    if (( segtable[segslot].type != SEGTYPE_AVAILABLE ) &&
                        ( segtable[segslot].selector == selector )) {
                        mode = DBG_MODLOAD;
                        segment--;          //make it zero-based
                        selector = newselect;
                        pFileName = segtable[segslot].path_name;
                        // Don't have the image length here so
                        // just choose one.
                        ImgLen = segtable[segslot].ImgLen;
                        break;
                    }
                    segslot++;
                }
            }
            if (fVerbose) {
                dprintf("VDM SegMove: %s (%x) %x => %x, len = %x\n",
                                        pFileName, segment, selector, newselect, ImgLen);
            }
           break;
        case DBG_SEGFREE:
            fNeedSegTableEdit = TRUE;
            selector = se.Selector1;
            if (fVerbose) {
                dprintf("VDM SegFree: %x\n",selector);
            }
            break;
        case DBG_MODFREE:
            fNeedSegTableEdit = TRUE;

            if ( strlen(se.FileName) != 0 ) {
                dprintf("Freeing [%s]\n", se.FileName );
            } else if (fVerbose) {
                dprintf("VDM ModFree: unknown module\n");
            }
            break;
        case DBG_MODLOAD:
            fNeedSegTableEdit = TRUE;
            selector = se.Selector1;
            ImgLen   = se.Length;
            segment = se.Segment;
            pFileName = se.FileName;

            segslot = 0;
            while ( segslot < MAXSEGENTRY ) {
                if ( segtable[segslot].type != SEGTYPE_AVAILABLE ) {
                    if ( _stricmp(segtable[segslot].path_name, se.FileName) == 0 ) {
                        break;
                    }
                }
                segslot++;
            }
            if ( segslot == MAXSEGENTRY ) {
                if ( strlen(se.FileName) != 0 ) {
                    dprintf("Loading [%s]\n", se.FileName );
                }
            }
            if (fVerbose) {
                dprintf("VDM ModLoad: %s => %s %x, len=%x\n",
                                        se.FileName,
                                        selector?"selector":"segment", selector?selector:segment,
                                        ImgLen);
            }
            break;
        case DBG_SINGLESTEP:
            fNeedInteractive = TRUE;
            break;
        case DBG_BREAK:
            fNeedInteractive = TRUE;
            break;
        case DBG_GPFAULT:
            dprintf("%s: access violation in VDM\n", DebuggerName);
            fNeedInteractive = TRUE;
            break;
        case DBG_INSTRFAULT:
            dprintf("%s: invalid opcode fault in VDM\n", DebuggerName);
            fNeedInteractive = TRUE;
            break;
        case DBG_DIVOVERFLOW:
            dprintf("%s: divide overflow in VDM\n", DebuggerName);
            fNeedInteractive = TRUE;
            break;
        case DBG_STACKFAULT:
            dprintf("%s: stack fault in VDM\n", DebuggerName);
            fNeedInteractive = TRUE;
            break;
        case DBG_TASKSTART:
            if ( fNeedInteractive || fVerbose ) {
                dprintf("VDM Start Task <%s:%s>\n",
                        im.Module,
                        im.FileName );
            }
            if (*(ULONG*)((VDMINTERNALINFO*)lpdw[3])->lpVdmDbgFlags & VDMDBG_BREAK_WOWTASK) fNeedInteractive = TRUE;
            break;
        case DBG_DLLSTART:
            if ( fNeedInteractive || fVerbose ) {
                dprintf("%s: VDM Start Dll <%s:%s>\n", DebuggerName, im.Module, im.FileName );
            }
            if (*(ULONG*)((VDMINTERNALINFO*)lpdw[3])->lpVdmDbgFlags & VDMDBG_BREAK_LOADDLL) fNeedInteractive = TRUE;
            break;
        case DBG_TASKSTOP:
            fNeedInteractive = FALSE;
            break;
        case DBG_DLLSTOP:
            fNeedInteractive = FALSE;
            break;
    }

    if ( fNeedInteractive ) {
        char    text[128];
        char    path[128];
        DWORD   cSeg;

        // Module lookup
        b = (*pfnVDMGetSelectorModule)(GetCurrentProcess(),GetCurrentThread(),
                (WORD)getCS(), &cSeg, text, 128, path, 128 );

        if ( b ) {
            dprintf("%s:%d!%04x:\n", text, cSeg, (WORD)getEIP() );
        }
        force_yoda();
    }
    
    if ( fNeedSegTableEdit ) {
        segslot = 0;
        fStop = FALSE;
        while ( segslot < MAXSEGENTRY ) {
            switch( mode ) {
                case DBG_SEGLOAD:
                    if ( segtable[segslot].type == SEGTYPE_AVAILABLE ) {
                        segtable[segslot].segment = segment;
                        segtable[segslot].selector = selector;
                        // This notification message is used only by wow in prot
                        // It could be determined from the current mode to be
                        // correct
                        segtable[segslot].type = SEGTYPE_PROT;
                        Str = calloc(1,strlen(se.FileName)+1);
                        if ( !Str ) {
                            return( fResult );
                        }
                        strcpy( Str, se.FileName );
                        segtable[segslot].path_name = Str;
                        segtable[segslot].ImgLen = 0;
                        fStop = TRUE;
                    }
                    break;
                case DBG_SEGMOVE:
                    if (( segtable[segslot].type != SEGTYPE_AVAILABLE ) &&
                        ( segtable[segslot].selector == selector )) {
                        segtable[segslot].selector = newselect;
                        fStop = TRUE;
                    }
                    break;
                case DBG_SEGFREE:
                    if ( segtable[segslot].selector == selector ) {
                        fStop = TRUE;
                        segtable[segslot].type = SEGTYPE_AVAILABLE;
                        free(segtable[segslot].path_name);
                        segtable[segslot].path_name = NULL;
                    }
                    break;
                case DBG_MODFREE:
                    if ( segtable[segslot].type != SEGTYPE_AVAILABLE ) {
                        if ( _stricmp(segtable[segslot].path_name,se.FileName) == 0 ) {
                            segtable[segslot].type = SEGTYPE_AVAILABLE;
                            free(segtable[segslot].path_name);
                            segtable[segslot].path_name = NULL;
                        }
                    }
                    break;
                case DBG_MODLOAD:
                    if ( segtable[segslot].type == SEGTYPE_AVAILABLE ) {
                        segtable[segslot].segment  = segment;
                        segtable[segslot].selector = selector;
                        // This notification message is used only by v86 dos
                        // It could be determined from the current mode to be
                        // correct
                        segtable[segslot].type = SEGTYPE_V86;
                        Str = calloc(1,strlen(pFileName)+1);
                        if ( !Str ) {
                            return( fResult );
                        }
                        strcpy( Str, pFileName );
                        segtable[segslot].path_name = Str;
                        segtable[segslot].ImgLen = ImgLen;
                        fStop = TRUE;
                    }
                    break;

            }
            if ( fStop ) {
                break;
            }
            segslot++;
        }
        if ( segslot == MAXSEGENTRY ) {
            if ( mode == DBG_SEGLOAD ) {
                dprintf("%s: Warning - adding selector %04X for segment %d, segtable full\n",
                         DebuggerName, selector, segment );
            }
        }
    }

    return( fResult );
}

LONG VdmDebugExceptionHandler(
  EXCEPTION_POINTERS *ExceptionInfo
)
{
    DEBUG_EVENT     DebugEvent = {0};

    if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_VDM_EVENT)
    {
        DebugEvent.dwProcessId = GetCurrentProcessId();
        DebugEvent.dwThreadId = GetCurrentThreadId();
        DebugEvent.u.Exception.dwFirstChance = 1;
        memcpy(&DebugEvent.u.Exception.ExceptionRecord, 
            ExceptionInfo->ExceptionRecord, sizeof(DebugEvent.u.Exception.ExceptionRecord));
        if (VDMEvent(&DebugEvent))
            return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

PVOID
SetYodaVDMEventHandler(BOOL fEnable)
{
    static PVOID pExceptionHandler = NULL;
    
    if (fEnable) {
        if (!pExceptionHandler)
            pExceptionHandler = AddVectoredExceptionHandler(TRUE, VdmDebugExceptionHandler);
        if (!bDbgDebuggerLoaded)
        {
            VdmDbgAttach();
            DBGNotifyDebugged(TRUE);
        }
    } else {
        if (pExceptionHandler) {
            if (RemoveVectoredExceptionHandler(pExceptionHandler)) pExceptionHandler = NULL;
        }
    }
    printf ("YODA debug event handler %s\n", pExceptionHandler?"installed":"uninstalled");
    return pExceptionHandler;
}

static VOID dfprintf( char *lpszFormatStr, ... )
{
    UINT n;
    CHAR buf[256];
    va_list va;

    va_start(va, lpszFormatStr);
    // Cannot use vfprintf, as it's not wrapped by fprt.c which redirects output to debugger
    n = vsprintf(buf, lpszFormatStr, va);
    fprintf(trace_file, buf);
    va_end(va);
}

static int trace_hook(int *dump_info)
{
    if ((*dump_info & DUMP_REG) || (*dump_info & DUMP_FLAGS) || (*dump_info & DUMP_INST))
    {
        NtsdExtensions.lpOutputRoutine = dfprintf;
        VdmExtCommand("r", "");
        NtsdExtensions.lpOutputRoutine = dprintf;
        *dump_info &= ~(DUMP_REG | DUMP_FLAGS | DUMP_INST);
        return 1;
    }
    return 0;
}

BOOL VdmExtCommand(char *pname, char *args)
{
    PNTSD_EXTENSION_ROUTINE ExtensionRoutine = NULL;
    char *modname = "vdmexts.dll";
    HANDLE hThread;
    static BOOL fSymInitialized = FALSE;

    hProcess = GetCurrentProcess();
    hThread = GetCurrentThread();

    if ( NtsdExtensions.nSize == 0 ) {
        NtsdExtensions.nSize = sizeof(NtsdExtensions);
        NtsdExtensions.lpOutputRoutine = dprintf;
        NtsdExtensions.lpGetExpressionRoutine = GetExpressionRoutine;
        NtsdExtensions.lpGetSymbolRoutine = GetSymbolRoutine;
        NtsdExtensions.lpDisasmRoutine = disasmExportRoutine;
        NtsdExtensions.lpCheckControlCRoutine = CheckControlC;
    }

    if ( !hModule )
    {
        if (!(hModule = LoadLibrary(modname)))
        {
            dprintf("Error %d loading %s\n", GetLastError(), modname);
            return FALSE;
        }
        trace_install_hook(trace_hook);
    }

    if (!bDbgDebuggerLoaded)
    {
        if (!GetModuleHandle("NTVDMD.DLL")) DBGInit ();
        if (!bDbgDebuggerLoaded) SetYodaVDMEventHandler(TRUE);
    }

    if (!fSymInitialized)
    {
        if (fSymInitialized = SymInitialize( hProcess, NULL, TRUE ))
        {
            SymRegisterCallback( hProcess, SymbolCallbackFunction, NULL );
            SetSymbolSearchPath();
        }
    }

    if (!(ExtensionRoutine = (PNTSD_EXTENSION_ROUTINE)GetProcAddress(hModule, pname)))
    {
        if (!_stricmp( pname, "unload" )) {
            FreeLibrary(hModule);
            hModule = NULL;
            trace_install_hook(NULL);
            return TRUE;
        } else {
            dprintf("GetProcAddress(\"%s\",\"%s\") failed with %d\n",
                modname, pname, GetLastError());
            return FALSE;
        }
    }

	LoadVdmDBG();

    __try {
        (ExtensionRoutine)(
            hProcess,
            hThread,
            effective_addr(getCS(), getIP()),
            &NtsdExtensions,
            args
            );
    }
    __except (fnBangCommandExceptionFilter(GetExceptionInformation(),modname,pname)) {
        return FALSE;
    }

    return TRUE;
}
#else /* YODA */
PVOID
SetYodaVDMEventHandler(BOOL fEnable)
{
  return NULL;
}
#endif /* YODA */
