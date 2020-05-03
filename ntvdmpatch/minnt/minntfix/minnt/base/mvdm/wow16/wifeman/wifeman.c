/****************************************************************************\
*
* WIFE.C : Far East Windows Intelligent Font Environment definitions
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

#define WIFE_VERSION	0x0109

static char m_szMsg[256], m_szTitle[256], m_szMsgArg[256];

static SHORT MyMessageBox(HWND hWnd, LPCSTR szMsg, LPCSTR szTitle, USHORT mode);
static void NotSupported();

/****************************************************************************
    global data
****************************************************************************/
HINSTANCE ghInst;                     // our module handle 


/***********************************************/
/*         Font Driver Interface               */
/***********************************************/

// USHORT FAR PASCAL MiscGetEUDCLeadByteRange( CHAR_SET csCharSet )
// This is just a thunk to WOW32, see wmthunks.asm

HFF FAR PASCAL FdiLoadFontFile( HFD hFD, LPSTR lpszPackName )
{
	NotSupported();
	return -1;
}

LONG FAR PASCAL FdiUnloadFontFile( HFD hFD, HFF hFF )
{
	NotSupported();
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
	NotSupported();
	return 0;
}

HFC FAR PASCAL FdiOpenFontContext( HFD hFD, HFF hFF, ULONG ulFont )
{
	NotSupported();
	return NULL;
}

LONG FAR PASCAL FdiSetFontContext(
	HFD hFD, 
	HFC hFC,
	LPCONTEXTINFO lpContextInfo
)
{
	NotSupported();
	return 0;
}

LONG FAR PASCAL FdiCloseFontContext( HFD hFD, HFC hFC )
{
	NotSupported();
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
	NotSupported();
	return 0;
}

LONG FAR PASCAL FdiQueryCharAttr(
	HFD hFD, 
	HFC        hFC,
	LPCHARATTR lpCharAttr,
	LPBITMAPMETRICS lpbmm
)
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL FdiControl(
	HFD hFD, 
	HFC hFC,
	LPCONTROLTAG lpControlTag,
	LPVOID lpOutData
)
{
	NotSupported();
	return 0;
}

LONG FAR PASCAL  FdiClaimFontFile( HFD hFD, LPSTR lpszFileName )
{
	NotSupported();
	return 0;
}

LONG FAR PASCAL FdiConvertFontFile(
        HFD hFD,
        LPSTR lpszSrcFileName,
        LPSTR lpszDestDirName,
        LPSTR lpszResultPackName
)
{
	NotSupported();
	return 0;
}


/***********************************************/
/*         Module Install Manager              */
/***********************************************/

HFD FAR PASCAL MimAddFontDriver( LPSTR lpszFileName )
{
	NotSupported();
	return NULL;
}

HFF FAR PASCAL MimAddFontPack( HFD hFD, LPSTR lpszPackName )
{
	NotSupported();
	return NULL;
}

BOOL FAR PASCAL MimRemoveFontDriver( HFD hFD )
{
	NotSupported();
	return FALSE;
}

BOOL FAR PASCAL MimRemoveFontPack( HFD hFD, HFF hFontPack )
{
	NotSupported();
	return FALSE;
}

HFD FAR PASCAL MimEnumFontDrivers( HFD hLastDriver )
{
	NotSupported();
	return NULL;
}

HFF FAR PASCAL MimEnumFontPacks( HFD hFD, HFF hLastPack )
{
	NotSupported();
	return NULL;
}

HFC FAR PASCAL MimOpenFontContext( HFD hFD, HFF hFF, ULONG ulFont )
{
	NotSupported();
	return NULL;
}

LONG FAR PASCAL MimCloseFontContext( HFD hFD, HFF hFF, HFC hFC )
{
	NotSupported();
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
	NotSupported();
	return 0;
}

/***********************************************/
/*  WIFEMAN's Miscellaneous Service Functions  */
/***********************************************/


BOOL FAR PASCAL MiscIsDBCSLeadByte( CHAR_SET CharSet, USHORT ch )
{
    return IsDBCSLeadByte((UCHAR)ch);
}

VOID FAR PASCAL MiscSetErroInfo( ULONG ErrorCode )
{
	NotSupported();
}

ULONG FAR PASCAL MiscGetErroInfo( VOID )
{
	NotSupported();
	return 0;
}

USHORT FAR PASCAL ubstrlen( LPUBCHAR cp )
{
	NotSupported();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrcpy( LPUBCHAR dst, LPUBCHAR src )
{
	NotSupported();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrncpy( LPUBCHAR dst, LPUBCHAR src, USHORT limit )
{
	NotSupported();
	return 0;
}

LPUBCHAR FAR PASCAL ubstrcat( LPUBCHAR dst, LPUBCHAR src )
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL ubstrcmp( LPUBCHAR str1, LPUBCHAR str2 )
{
	NotSupported();
	return 0;
}

USHORT FAR PASCAL AscizToUz( 
	LPUBCHAR dst, 
	LPSTR src, 
	USHORT limit,
	CHAR_SET CharSet
)
{
	NotSupported();
	return 0;
}

USHORT FAR PASCAL UzToAsciz(
	LPSTR dst, 
	LPUBCHAR src, 
	USHORT limit 
)
{
	NotSupported();
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
		if (pLoadString(hInst, idsMsg, m_szMsg, sizeof(m_szMsg)) >= sizeof(m_szMsg))
			m_szMsg[sizeof(m_szMsg)-1]=0;
		if (pLoadString(hInst, idsTitle, m_szTitle, sizeof(m_szTitle)) >= sizeof(m_szTitle))
			m_szTitle[sizeof(m_szTitle)-1]=0;
		return MyMessageBox(NULL, m_szMsg, m_szTitle, mode);
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
		if (pLoadString(hInst, idsMsg, m_szMsg, sizeof(m_szMsg)) >= sizeof(m_szMsg))
			m_szMsg[sizeof(m_szMsg)-1]=0;
		if (pLoadString(hInst, idsTitle, m_szTitle, sizeof(m_szTitle)) >= sizeof(m_szTitle))
			m_szTitle[sizeof(m_szTitle)-1]=0;
		pwsprintf(m_szMsgArg, m_szMsg, lpszArgument);
		return MyMessageBox(NULL, m_szMsg, m_szTitle, mode);
	}

	return IDCANCEL;
}


ULONG FAR PASCAL MiscGetVersion( VOID )
{
	return WIFE_VERSION;
}

BYTE FAR PASCAL MiscConvertFontFamily( LPSTR szFamilyName )
{
	NotSupported();
	return 0;
}


BYTE FAR PASCAL MiscConvertCharSet( LPSTR CharSetString )
{
	NotSupported();
	return 0;
}

VOID FAR PASCAL MiscIfiMetricsToLogFont(
        LPLOGFONT lpLogFont,
        LPIFIMETRICS lpIFIMetrics,
        USHORT usLogicalMapFlag
)
{
	NotSupported();
}

VOID FAR PASCAL MiscIfiMetricsToTextMetrics(
        LPTEXTMETRIC lpTextMetrics,
        LPIFIMETRICS lpIFIMetrics
)
{
	NotSupported();
}

VOID FAR PASCAL MiscMakeTextXform(
        LPTEXTXFORM lpTXF,
        LPIFIMETRICS lpIM,
        LPLOGFONT lpLF
)
{
	NotSupported();
}


VOID FAR PASCAL MiscTrancateString( 
	LPSTR lpszStr, 
	SHORT length, 
	SHORT CharSet 
)
{
	NotSupported();
}

LPVOID FAR PASCAL MiscAddHugePtr( LPVOID src, ULONG offset )
{
	NotSupported();
	return NULL;
}

USHORT FAR PASCAL MiscGetSegmentIncrement( VOID )
{
	NotSupported();
	return 0;
}

BOOL FAR PASCAL MiscIsWifeControl( SHORT function )
{
	NotSupported();
	return FALSE;
}

BOOL FAR PASCAL MiscIsGaijiControl( SHORT function )
{
	NotSupported();
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
	NotSupported();
	return FALSE;
}

LP_QUICK_SEARCH_TABLE FAR PASCAL MiscValidateQuickSearchTable(
        USHORT usLogicalMapFlag
)
{
	NotSupported();
	return NULL;
}

BOOL FAR PASCAL MiscUnvalidateQuickSearchTable(
        USHORT usLogicalMapFlag
)
{
	NotSupported();
	return FALSE;
}

BOOL FAR PASCAL MiscRegisterNotifyFunction(
        LPNOTIFYCALLBACKFUNC lpfnCallBack
)
{
	NotSupported();
	return FALSE;
}

BOOL FAR PASCAL MiscUnregisterNotifyFunction(
        LPNOTIFYCALLBACKFUNC lpfnCallBack
)
{
	NotSupported();
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
	NotSupported();
	return NULL;
}

BOOL FAR PASCAL FcmDeleteCacheBuffer( HFB hFontBuffer )
{
	NotSupported();
	return FALSE;
}

SHORT FAR PASCAL FcmRequestImages(
        HFB hFontBuffer,
        LP_FCM_CHARACTERISTIC AnswerBuffer,
        short length
)
{
	NotSupported();
	return 0;
}

BOOL FAR PASCAL FcmReleaseImages(
        HFB hFontBuffer,
        LP_FCM_CHARACTERISTIC AnswerBuffer,
        short length
)
{
	NotSupported();
	return FALSE;
}

HFB FAR PASCAL FcmRequestDefaultFB(
        LPFONTINFO lpFont,
        LPTEXTXFORM lpTextXform
)
{
	NotSupported();
	return NULL;
}

BOOL FAR PASCAL FcmReleaseDefaultFB( HFB hFB )
{
	NotSupported();
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
	NotSupported();
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
	NotSupported();
	return 0;
}

SHORT FAR PASCAL FcmCleanUp( VOID )
{
	NotSupported();
	return 0;
}

HFB FAR PASCAL FcmEnumFontBuffers( HFB hLastBuffer )
{
	NotSupported();
	return NULL;
}

SHORT FAR PASCAL FcmCalcByteWidth( SHORT nBitWidth )
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL FcmForceCacheIn( HFB hFB, LPUBCHAR lpubStr )
{
	NotSupported();
	return 0;
}

BOOL FAR PASCAL FcmValidateFC( HFB hFB )
{
	NotSupported();
	return FALSE;
}

BOOL FAR PASCAL FcmUnvalidateFC( HFB hFB )
{
	NotSupported();
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
	NotSupported();
	return 0;
}

SHORT FAR PASCAL FcmProcessDeviceControl(
        HFB hFB,
        SHORT nFunction,
        GAIJIINFO FAR * lpGI,
        LPVOID lpOutData
)
{
	NotSupported();
	return 0;
}

USHORT FAR PASCAL FcmGetEUDCLeadByteRange( HFB hFB )
{
	NotSupported();
	return 0;
}

HFB FAR PASCAL FcmGetEUDCFB( HFB hFB )
{
	NotSupported();
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
	NotSupported();
	return 0;
}

HFB FAR PASCAL FcmRequestDefaultFBEx(
        LPFONTINFO lpFont,
        LPTEXTXFORM lpTextXform,
		WORD wControlFlag
)
{
	NotSupported();
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
	NotSupported();
	return 0;
}

BOOL FAR PASCAL MiscDeleteWifeFont(
        LPFONTINFO lpExtFont
)
{
	NotSupported();
	return FALSE;
}

VOID FAR PASCAL MapWifeFont(LPLOGFONT lpLogFont, LPMAPRESULT lpMapResult)
{
	NotSupported();
}


VOID FAR PASCAL SetCloseOldestFCFunc(
        LPCBCLOSEFC lpfnCallbackFunc
)
{
	NotSupported();
}


LONG FAR PASCAL CachedOpenFontContext(
        HFD hFD,
		HFF hFF, 
		ULONG ulFont,
		LPCONTEXTINFO lpContextInfo,
		SHORT sMinorCharSet,
		SHORT sAttribute)
{
	NotSupported();
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
	NotSupported();
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
	NotSupported();
	return 0;
}

VOID FAR PASCAL GetEUDCFD(
        HFD *lphFD,
		HFF *lphFF
)
{
	NotSupported();
}

VOID FAR PASCAL GetDefaultFontInfo( LPDEFFONTINFO lpInfo )
{
	NotSupported();
}

VOID FAR PASCAL DebugAssertion ( VOID )
{
	NotSupported();
}

SHORT FAR PASCAL DelayedInit ( VOID )
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL FirstInit ( VOID )
{
	NotSupported();
	return 0;
}


/***********************************************/
/*    DIAGNOSTIC TEST&CHECKING FUNCTIONS       */
/***********************************************/

LPVOID FAR PASCAL DiagLocalLockAnything( LOCALHANDLE hAny )
{
	NotSupported();
	return NULL;
}

LPVOID FAR PASCAL DiagLocalUnlockAnything( LOCALHANDLE hAny )
{
	NotSupported();
	return NULL;
}

SHORT FAR PASCAL DiagGetConfirmString( LPSTR buffer, SHORT length )
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL DiagSelfCheck( VOID )
{
	NotSupported();
	return 0;
}

SHORT FAR PASCAL DiagSelfCheckAndWarning( VOID )
{
	NotSupported();
	return 0;
}


static SHORT MyMessageBox(
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

static void NotSupported()
{
	if (MiscWarningMessage(ghInst, IDS_FUNCNOTSUPP, IDS_APPTITLE, MB_YESNO) == IDNO)
		FatalAppExit(0, NULL);
}

int NEAR PASCAL
LibMain(
    HINSTANCE hInstance,
    UINT cbHeap,
    LPSTR lpCmdLine
    )
{ 
	ghInst = hInstance; 
	return TRUE;
}

