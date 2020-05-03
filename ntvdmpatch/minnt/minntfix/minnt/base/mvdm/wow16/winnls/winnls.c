#include <windows.h>
#define TAIWAN
#include <winnls.h>

/* Missing declaraions, from ime31.h */
typedef struct tagIMESTRUCT {
    UINT        fnc;            // function code
    WPARAM      wParam;         // word parameter
    UINT        wCount;         // word counter
    UINT        dchSource;      // offset to Source from top of memory object
    UINT        dchDest;        // offset to Desrination from top of memory object
    LPARAM      lParam1;
    LPARAM      lParam2;
    LPARAM      lParam3;
} IMESTRUCT;

typedef IMESTRUCT      *PIMESTRUCT;
typedef IMESTRUCT NEAR *NPIMESTRUCT;
typedef IMESTRUCT FAR  *LPIMESTRUCT;


int FAR PASCAL WEP(int);

void FAR PASCAL InquireWINNLS(void)
{
	
}

LRESULT  WINAPI HookKeyboardMessage(int code, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL FAR PASCAL WINNLSSetKeyboardHook (BOOL x)
{
	return FALSE;
}

BOOL FAR PASCAL WINNLSSetIMEHandle(LPSTR lpszName, HWND hWnd)
{
	return FALSE;
}

BOOL FAR PASCAL WINNLSSetIMEStatus(HWND hWnd, BOOL status)
{
	return FALSE;
}

#ifdef KOREA
BOOL FAR PASCAL WINNLSSetIMEHotkey(HWND hWnd, WORD key, WORD key2)
#else
BOOL FAR PASCAL WINNLSSetIMEHotkey(HWND hWnd, WORD key)	/* ;Internal */
#endif //KOREA
{
	return FALSE;
}

WORD FAR PASCAL WINNLSGetKeyState(void)
{
	return 0;
}

BOOL FAR PASCAL WINNLSSetKeyState(WORD wState)
{
	return FALSE;
}

BOOL FAR PASCAL IMPAddIME(LPIMEPRO lpIMEPro)
{
	return FALSE;
}

BOOL FAR PASCAL IMPDeleteIME(LPIMEPRO lpIMEPro)
{
	return FALSE;
}

BOOL FAR PASCAL IMPModifyIME(LPSTR lpStr, LPIMEPRO lpIMEPro)
{
	return FALSE;
}

WORD FAR PASCAL IMPGetDefaultIME(LPIMEPRO lpIMEPro)
{
	return 0;
}

WORD FAR PASCAL IMPSetDefaultIME(LPIMEPRO lpIMEPro)
{
	return 0;
}

BOOL FAR PASCAL WINNLSSendString(HWND hWnd, WORD wFunc, LPVOID lpData)
{
	return FALSE;
}

BOOL FAR PASCAL WINNLSPostAppMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

LRESULT FAR PASCAL WINNLSSendAppMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return NULL;
}

void FAR PASCAL WINNLSSetSysIME(HWND hWnd)
{
}

HWND FAR PASCAL WINNLSGetSysIME(void)
{
	return NULL;
}

LONG FAR PASCAL WINNLSIMEControl(HWND hWnd, HWND hWnd2, LPIMESTRUCT lpIMEStruct)
{
	return 0;
}

BOOL FAR PASCAL WINNLSSendControl(WORD a, WORD b)
{
	return FALSE;
}


BOOL FAR PASCAL WINNLSQueryIMEInfo(HWND hWnd1, HWND hWnd2, LPIMEINFO lpIMEInfo)
{
	return FALSE;
}

BOOL FAR PASCAL IMPEnableIME(HWND hWnd, LPIMEPRO lpIMEPro, BOOL bEnable)
{
	return FALSE;
}

BOOL FAR PASCAL IMPSetFirstIME(HWND hWnd, LPIMEPRO lpIMEPro)
{
	return FALSE;
}

BOOL FAR PASCAL IMPGetFirstIME(HWND hWnd, LPIMEPRO lpIMEPro)
{
	return FALSE;
}

BOOL FAR PASCAL IMPSetUsrFont(HWND hWnd, LPIMEPRO lpIMEPro)
{
	return FALSE;
}

BOOL FAR PASCAL InquireIME(void)
{
	return FALSE;
}

BOOL FAR PASCAL IMPRetrieveIME(LPIMEPRO lpIMEPro, WPARAM wParam)
{
	return FALSE;
}

BOOL FAR PASCAL WINNLSDefIMEProc(HWND hWnd, HDC hDC, WPARAM wParam1, WPARAM wParam2,  LPARAM lParam1, LPARAM lParam2)
{
	return FALSE;
}

LRESULT FAR PASCAL ControlIMEMessage(HWND hWnd, LPIMEPRO lpIMEPro, WPARAM wParam1, WPARAM wParam2, LPARAM lParam)
{
	return 0;
}


/*-----------------------------------------------------------------------
|	
| DLL specific routines
|	
---------------------------------------------------------------WESC----*/

/*-----------------------------------------------------------------------
|   WEP
-----------------------------------------------------------------------*/
int FAR PASCAL WEP (int wSystemExit)
{
   return 1;
}
