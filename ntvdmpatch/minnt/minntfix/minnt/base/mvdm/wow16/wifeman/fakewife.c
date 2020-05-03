/****************************************************************************\
*
* FAKEWIFE.C : Far East Windows Intelligent Font Environment definitions
*
******************************************************************************
*
* This module stubs most of the WIFE functions, as they are not supported in
* WOW16. Reconstructed all definitions for accuracy.
*
* History:
* 02-May-2020   leecher1337
*               Initial version
*
\****************************************************************************/

#define NOSCALABLEFONT
#include <windows.h>
#include <gdidefs.h>
#define WIFE_CREATE_DEVICEDRIVER
#define WIFE_ENABLE_GDIDEFS_SUPPORT
#define WIFE_ENABLE_FONT_STRUCT_MAKE
#define WIFE_ENABLE_QUICK_SEARCH_TABLE
#define WIFE_ENABLE_NOTIFY_FUNCTIONS
#define WIFE_ENABLE_EUDC_SUPPORT
#define WIFE_ENABLE_FCM
#include "wifeman.h"
#include "wife.h"


/****************************************************************************
    definitions not present in wife.h (may not be accurate, made-up names)
****************************************************************************/
typedef struct {
	USHORT	uBestValue;
	HANDLE	hBestFD;
	LONG	hBestValue;
	LONG	oBestValue;
	BOOL	bBestSinf;
} MAPRESULT, FAR * LPMAPRESULT;

typedef struct {
	HFD		hFD;
	HFF		hFF;
	ULONG	ulOrderLV;
	ULONG	ulOrderVV;
	ULONG	ulOrderH;
} DEFFONTINFO, FAR * LPDEFFONTINFO;

typedef struct {
	USHORT	GaijiInfoSize;
	LPBYTE	lpGaijiInData;
	LPBYTE	lpGaijiPFont;
	LPBYTE	lpGaijiTXForm;
	LPBYTE	lpGaijiDMode;
} GAIJIINFO, FAR * LPGAIJIINFO;

typedef BOOL (FAR PASCAL * LPCBCLOSEFC)
                ( HFD hFD, HFF hFF, ULONG ulFont );


/****************************************************************************
    local definitions
****************************************************************************/
typedef int (WINAPI *fpLoadString)(HINSTANCE, UINT, LPSTR, int);
typedef int (WINAPI *fpMessageBox)(HWND, LPCSTR, LPCSTR, UINT);
typedef int (FAR CDECL *fpwsprintf)(LPSTR lpszOut, LPCSTR lpszFmt, ...);

#define ORD_MESSAGEBOX	1
#define ORD_LOADSTRING	176
#define ORD_WSPRINTF	420

static char _szMsg[256], _szTitle[256], _szNewMsg[256];

static SHORT InternalWarningMessageByString(HWND hWnd, LPCSTR szMsg, LPCSTR szTitle, USHORT mode);
static void _NotifyNoSuport();


extern HINSTANCE hInst;                     // our module handle 

/***********************************************/
/*         Font Driver Interface               */
/***********************************************/

// USHORT FAR PASCAL MiscGetEUDCLeadByteRange( CHAR_SET csCharSet )
// This is just a thunk to WOW32, see wmthunks.asm

HFF FAR PASCAL FdiLoadFontFile( HFD hFD, LPSTR lpszPackName )
{
	_NotifyNoSuport();
	return -1;
}

LONG FAR PASCAL FdiUnloadFontFile( HFD hFD, HFF hFF )
{
	_NotifyNoSuport();
	return -1;
}

LONG FAR PASCAL FdiQueryFaces( 
	HFD hFD, 
	HFF hFF,            // Font File handle 
	LPIFIMETRICS lpIfiMetrics,   // Buffer for the metrics 
	ULONG cMetricLen,    // Length of the metrics structure 
	ULONG cFontCount,    // # of fonts wanted. 
	ULONG cStart         // index of the font to start with 
)
{
	_NotifyNoSuport();
	return 0;
}

HFC FAR PASCAL FdiOpenFontContext( HFD hFD, HFF hFF, ULONG ulFont )
{
	_NotifyNoSuport();
	return NULL;
}

LONG FAR PASCAL FdiSetFontContext(
	HFD hFD, 
	HFC hFC,
	LPCONTEXTINFO lpContextInfo
)
{
	_NotifyNoSuport();
	return 0;
}

LONG FAR PASCAL FdiCloseFontContext( HFD hFD, HFC hFC )
{
	_NotifyNoSuport();
	return NULL;
}


LONG FAR PASCAL FdiQueryFaceAttr(
	HFD hFD, 
	HFC       hFC,
	ULONG     iQuery,
	LPVOID    lpBuffer,
	ULONG     cb,
	LPVOID    lpIndex,
	SHORT     Start
)
{
	_NotifyNoSuport();
	return 0;
}

LONG FAR PASCAL FdiQueryCharAttr(
	HFD hFD, 
	HFC        hFC,
	LPCHARATTR lpCharAttr,
	LPBITMAPMETRICS lpbmm
)
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FdiControl(
	HFD hFD, 
	HFC hFC,
	LPCONTROLTAG lpControlTag,
	LPVOID lpOutData
)
{
	_NotifyNoSuport();
	return 0;
}

LONG FAR PASCAL  FdiClaimFontFile( HFD hFD, LPSTR lpszFileName )
{
	_NotifyNoSuport();
	return 0;
}

LONG FAR PASCAL FdiConvertFontFile(
        HFD hFD,
        LPSTR lpszSrcFileName,
        LPSTR lpszDestDirName,
        LPSTR lpszResultPackName
)
{
	_NotifyNoSuport();
	return 0;
}


/***********************************************/
/*         Module Install Manager              */
/***********************************************/

HFD FAR PASCAL MimAddFontDriver( LPSTR lpszFileName )
{
	_NotifyNoSuport();
	return NULL;
}

HFF FAR PASCAL MimAddFontPack( HFD hFD, LPSTR lpszPackName )
{
	_NotifyNoSuport();
	return NULL;
}

BOOL FAR PASCAL MimRemoveFontDriver( HFD hFD )
{
	_NotifyNoSuport();
	return FALSE;
}

BOOL FAR PASCAL MimRemoveFontPack( HFD hFD, HFF hFontPack )
{
	_NotifyNoSuport();
	return FALSE;
}

HFD FAR PASCAL MimEnumFontDrivers( HFD hLastDriver )
{
	_NotifyNoSuport();
	return NULL;
}

HFF FAR PASCAL MimEnumFontPacks( HFD hFD, HFF hLastPack )
{
	_NotifyNoSuport();
	return NULL;
}

HFC FAR PASCAL MimOpenFontContext( HFD hFD, HFF hFF, ULONG ulFont )
{
	_NotifyNoSuport();
	return NULL;
}

LONG FAR PASCAL MimCloseFontContext( HFD hFD, HFF hFF, HFC hFC )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL MimGetInformations( 
	MIMGETINFO_ORDER order,
	HFD hFD,
	HFF hFF,
	LPVOID AnswerBuffer,
	USHORT SizeOfBuffer
)
{
	_NotifyNoSuport();
	return 0;
}

/***********************************************/
/*  WIFEMAN's Miscellaneous Service Functions  */
/***********************************************/


VOID FAR PASCAL MiscSetErroInfo( ULONG ErrorCode )
{
	_NotifyNoSuport();
}

ULONG FAR PASCAL MiscGetErroInfo( VOID )
{
	_NotifyNoSuport();
	return 0;
}

USHORT FAR PASCAL ubstrlen( LPUBCHAR cp )
{
	_NotifyNoSuport();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrcpy( LPUBCHAR dst, LPUBCHAR src )
{
	_NotifyNoSuport();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrncpy( LPUBCHAR dst, LPUBCHAR src, USHORT limit )
{
	_NotifyNoSuport();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrcat( LPUBCHAR dst, LPUBCHAR src )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL ubstrcmp( LPUBCHAR str1, LPUBCHAR str2 )
{
	_NotifyNoSuport();
	return 0;
}

USHORT FAR PASCAL AscizToUz( 
	LPUBCHAR dst, 
	LPSTR src, 
	USHORT limit,
	CHAR_SET CharSet
)
{
	_NotifyNoSuport();
	return 0;
}

USHORT FAR PASCAL UzToAsciz(
	LPSTR dst, 
	LPUBCHAR src, 
	USHORT limit 
)
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL MiscWarningMessage( 
	HANDLE hInst, 
	USHORT idsMsg, 
	USHORT idsTitle, 
	USHORT mode 
)
{
	HMODULE hMod = GetModuleHandle("USER.EXE");
	fpLoadString pLoadString;

	if (hMod && ((FARPROC)pLoadString = GetProcAddress(hMod, MAKELP(NULL, ORD_LOADSTRING))))
	{
		if (pLoadString(hInst, idsMsg, _szMsg, sizeof(_szMsg)) >= sizeof(_szMsg))
			_szMsg[sizeof(_szMsg)-1]=0;
		if (pLoadString(hInst, idsTitle, _szTitle, sizeof(_szTitle)) >= sizeof(_szTitle))
			_szTitle[sizeof(_szTitle)-1]=0;
		return InternalWarningMessageByString(NULL, _szMsg, _szTitle, mode);
	}

	return IDCANCEL;
}

SHORT FAR PASCAL MiscWarningMessageWithArgument(
        HANDLE hInst,
        USHORT idsMsg,
        USHORT idsTitle,
        LPSTR lpszArgument,
        USHORT mode
)
{
	HMODULE hMod = GetModuleHandle("USER.EXE");
	fpLoadString pLoadString;
	fpwsprintf pwsprintf;

	if (hMod && 
		((FARPROC)pLoadString = GetProcAddress(hMod, MAKELP(NULL, ORD_LOADSTRING)))
		((FARPROC)pwsprintf= GetProcAddress(hMod, MAKELP(NULL, ORD_WSPRINTF)))
		)
	{
		if (pLoadString(hInst, idsMsg, _szMsg, sizeof(_szMsg)) >= sizeof(_szMsg))
			_szMsg[sizeof(_szMsg)-1]=0;
		if (pLoadString(hInst, idsTitle, _szTitle, sizeof(_szTitle)) >= sizeof(_szTitle))
			_szTitle[sizeof(_szTitle)-1]=0;
		pwsprintf(_szNewMsg, _szMsg, lpszArgument);
		return InternalWarningMessageByString(NULL, _szMsg, _szTitle, mode);
	}

	return IDCANCEL;
}



BYTE FAR PASCAL MiscConvertFontFamily( LPSTR szFamilyName )
{
	_NotifyNoSuport();
	return 0;
}


BYTE FAR PASCAL MiscConvertCharSet( LPSTR CharSetString )
{
	_NotifyNoSuport();
	return 0;
}

VOID FAR PASCAL MiscIfiMetricsToLogFont(
        LPLOGFONT lpLogFont,
        LPIFIMETRICS lpIFIMetrics,
        USHORT usLogicalMapFlag
)
{
	_NotifyNoSuport();
}

VOID FAR PASCAL MiscIfiMetricsToTextMetrics(
        LPTEXTMETRIC lpTextMetrics,
        LPIFIMETRICS lpIFIMetrics
)
{
	_NotifyNoSuport();
}

VOID FAR PASCAL MiscMakeTextXform(
        LPTEXTXFORM lpTXF,
        LPIFIMETRICS lpIM,
        LPLOGFONT lpLF
)
{
	_NotifyNoSuport();
}


VOID FAR PASCAL MiscTrancateString( 
	LPSTR lpszStr, 
	SHORT length, 
	SHORT CharSet 
)
{
	_NotifyNoSuport();
}

LPVOID FAR PASCAL MiscAddHugePtr( LPVOID src, ULONG offset )
{
	_NotifyNoSuport();
	return NULL;
}

USHORT FAR PASCAL MiscGetSegmentIncrement( VOID )
{
	_NotifyNoSuport();
	return 0;
}

BOOL FAR PASCAL MiscIsWifeControl( SHORT function )
{
	_NotifyNoSuport();
	return FALSE;
}

BOOL FAR PASCAL MiscIsGaijiControl( SHORT function )
{
	_NotifyNoSuport();
	return FALSE;
}


BOOL FAR PASCAL MiscStretchMonoFontImage(
        LPVOID  lpDestImage,
        USHORT  usSizeOfDestX,
        USHORT  usSizeOfDestY,
        LPVOID  lpSrcImage,
        USHORT  usSizeOfSrcX,
        USHORT  usSizeOfSrcY
)
{
	_NotifyNoSuport();
	return FALSE;
}

LP_QUICK_SEARCH_TABLE FAR PASCAL MiscValidateQuickSearchTable(
        USHORT usLogicalMapFlag
)
{
	_NotifyNoSuport();
	return NULL;
}

BOOL FAR PASCAL MiscUnvalidateQuickSearchTable(
        USHORT usLogicalMapFlag
)
{
	_NotifyNoSuport();
	return FALSE;
}

BOOL FAR PASCAL MiscRegisterNotifyFunction(
        LPNOTIFYCALLBACKFUNC lpfnCallBack
)
{
	_NotifyNoSuport();
	return FALSE;
}

BOOL FAR PASCAL MiscUnregisterNotifyFunction(
        LPNOTIFYCALLBACKFUNC lpfnCallBack
)
{
	_NotifyNoSuport();
	return FALSE;
}



/***********************************************/
/*         Font Cacheing Manager               */
/***********************************************/

HFB FAR PASCAL FcmCreateCacheBuffer(
	HFD hFD,
	HFF hFF,
	ULONG ulFont,
	LPTEXTXFORM lpXform,
	USHORT usMinorCharSet,
	USHORT usAttribute
)
{
	_NotifyNoSuport();
	return NULL;
}

BOOL FAR PASCAL FcmDeleteCacheBuffer( HFB hFontBuffer )
{
	_NotifyNoSuport();
	return FALSE;
}

SHORT FAR PASCAL FcmRequestImages(
        HFB hFontBuffer,
        LP_FCM_CHARACTERISTIC AnswerBuffer,
        short length
)
{
	_NotifyNoSuport();
	return 0;
}

BOOL FAR PASCAL FcmReleaseImages(
        HFB hFontBuffer,
        LP_FCM_CHARACTERISTIC AnswerBuffer,
        short length
)
{
	_NotifyNoSuport();
	return FALSE;
}

HFB FAR PASCAL FcmRequestDefaultFB(
        LPFONTINFO lpFont,
        LPTEXTXFORM lpTextXform
)
{
	_NotifyNoSuport();
	return NULL;
}

BOOL FAR PASCAL FcmReleaseDefaultFB( HFB hFB )
{
	_NotifyNoSuport();
	return FALSE;
}

DWORD FAR PASCAL FcmCalculateTextExtent(
        HFB         hFB,
        LPSTR       lpString,
        short       count,
        LPFONTINFO  lpFont,
        LPDRAWMODE  lpDrawMode,
        LPTEXTXFORM lpXform,
        LPSHORT     lpCharWidths,
        USHORT      usMode
)
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FcmCalculateOutputPositions(
        HFB             hFB,
        LP_FCM_EXTCHARACTERISTIC lpFcmCharacteristics,
        SHORT           nArrayLength,
        LPSHORT         lpx,
        LPSHORT         lpy,
        LPRECT          lpClipRect,
        LPSTR           FAR * lplpString,
        LPSHORT         lpcount,
        LPFONTINFO      lpFont,
        LPDRAWMODE      lpDrawMode,
        LPTEXTXFORM     lpXform,
        LPSHORT         FAR * lplpCharWidths,
        USHORT          usMode
)
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FcmCleanUp( VOID )
{
	_NotifyNoSuport();
	return 0;
}

HFB FAR PASCAL FcmEnumFontBuffers( HFB hLastBuffer )
{
	_NotifyNoSuport();
	return NULL;
}

SHORT FAR PASCAL FcmCalcByteWidth( SHORT nBitWidth )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FcmForceCacheIn( HFB hFB, LPUBCHAR lpubStr )
{
	_NotifyNoSuport();
	return 0;
}

BOOL FAR PASCAL FcmValidateFC( HFB hFB )
{
	_NotifyNoSuport();
	return FALSE;
}

BOOL FAR PASCAL FcmUnvalidateFC( HFB hFB )
{
	_NotifyNoSuport();
	return FALSE;
}

LONG FAR PASCAL FcmQueryFaceAttr(
        HFB         hFB,
        ULONG       iQuery,
        LPABC_TRIPLETS  lpBuffer,
        ULONG       cb,
        LPUBCHAR    lpIndex,
        UBCHAR      Start
)
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FcmProcessDeviceControl(
        HFB hFB,
        SHORT nFunction,
        GAIJIINFO FAR * lpGI,
        LPVOID lpOutData
)
{
	_NotifyNoSuport();
	return 0;
}

USHORT FAR PASCAL FcmGetEUDCLeadByteRange( HFB hFB )
{
	_NotifyNoSuport();
	return 0;
}

HFB FAR PASCAL FcmGetEUDCFB( HFB hFB )
{
	_NotifyNoSuport();
	return NULL;
}

USHORT FAR PASCAL FcmGetCharWidth(
        HFB         hFB,
        LPUSHORT    lpBuffer,
        USHORT      wFirstChar,
        USHORT      wLastChar,
        LPFONTINFO  lpFont,
        LPDRAWMODE  lpDrawMode,
        LPTEXTXFORM lpFontTrans,
        USHORT      usExpandPixels,
        USHORT      usMode
)
{
	_NotifyNoSuport();
	return 0;
}

HFB FAR PASCAL FcmRequestDefaultFBEx(
        LPFONTINFO lpFont,
        LPTEXTXFORM lpTextXform,
		WORD wControlFlag
)
{
	_NotifyNoSuport();
	return NULL;
}






/***********************************************/
/*            Realize functions                */
/***********************************************/


WORD FAR PASCAL MiscRealizeWifeFont(
        LPLOGFONT lpLogFont,
        LPFONTINFO lpExtFont,
        LPTEXTXFORM lpTextXform
)
{
	_NotifyNoSuport();
	return 0;
}

BOOL FAR PASCAL MiscDeleteWifeFont(
        LPFONTINFO lpExtFont
)
{
	_NotifyNoSuport();
	return FALSE;
}

VOID FAR PASCAL MapWifeFont(LPLOGFONT lpLogFont, LPMAPRESULT lpMapResult)
{
	_NotifyNoSuport();
}


VOID FAR PASCAL SetCloseOldestFCFunc(
        LPCBCLOSEFC lpfnCallbackFunc
)
{
	_NotifyNoSuport();
}


LONG FAR PASCAL CachedOpenFontContext(
        HFD hFD,
		HFF hFF, 
		ULONG ulFont,
		LPCONTEXTINFO lpContextInfo,
		SHORT sMinorCharSet,
		SHORT sAttribute)
{
	_NotifyNoSuport();
	return 0;
}

VOID FAR PASCAL FillFontInfo(
	LPFONTINFO lpExtFont, 
	LPIFIMETRICS lpIfiMetrics,
	HFD hFD,
	HFF hFF,
	LONG MetricsOrder,
	SHORT DegitizedResolution)
{
	_NotifyNoSuport();
}

SHORT FAR PASCAL FillContextInfo(
        USHORT ftHeight,
		USHORT ftWidth,
		LPIFIMETRICS lpIfiMetrics,
		HFD hFD,
		LPCONTEXTINFO lpContextInfo,
		LPCONTEXTINFO lpContextInfoEUDC,
		LPEUDC_CONTEXT lpEUDCContext,
		LPSHORT lpbSelfMode
)
{
	_NotifyNoSuport();
	return 0;
}

VOID FAR PASCAL GetEUDCFD(
        HFD *lphFD,
		HFF *lphFF
)
{
	_NotifyNoSuport();
}

VOID FAR PASCAL GetDefaultFontInfo( LPDEFFONTINFO lpInfo )
{
	_NotifyNoSuport();
}

VOID FAR PASCAL DebugAssertion ( VOID )
{
	_NotifyNoSuport();
}

SHORT FAR PASCAL DelayedInit ( VOID )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL FirstInit ( VOID )
{
	_NotifyNoSuport();
	return 0;
}


/***********************************************/
/*    DIAGNOSTIC TEST&CHECKING FUNCTIONS       */
/***********************************************/

LPVOID FAR PASCAL DiagLocalLockAnything( LOCALHANDLE hAny )
{
	_NotifyNoSuport();
	return NULL;
}

LPVOID FAR PASCAL DiagLocalUnlockAnything( LOCALHANDLE hAny )
{
	_NotifyNoSuport();
	return NULL;
}

SHORT FAR PASCAL DiagGetConfirmString( LPSTR buffer, SHORT length )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL DiagSelfCheck( VOID )
{
	_NotifyNoSuport();
	return 0;
}

SHORT FAR PASCAL DiagSelfCheckAndWarning( VOID )
{
	_NotifyNoSuport();
	return 0;
}


static SHORT InternalWarningMessageByString(
	HWND hWnd,
	LPCSTR szMsg,
	LPCSTR szTitle,
	USHORT mode)
{
	HMODULE hMod = GetModuleHandle("USER.EXE");
	fpMessageBox pMessageBox;

	if (hMod && ((FARPROC)pMessageBox = GetProcAddress(hMod, MAKELP(NULL, ORD_MESSAGEBOX))))
	{
		return pMessageBox(hWnd, szMsg, szTitle, mode);
	}

	return IDCANCEL;
}

static void _NotifyNoSuport()
{
	if (MiscWarningMessage(hInst, IDS_FUNCNOTSUPP, IDS_APPTITLE, MB_YESNO) == IDNO)
		FatalAppExit(0, NULL);
}

