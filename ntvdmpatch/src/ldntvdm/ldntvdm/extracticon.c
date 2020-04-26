/* Project: ldntvdm
 * Module : extracticon
 * Author : leecher@dose.0wnz.at
 * Descr. : Hooks PrivateExtractIconsW
 *          in order to add back support to extract Icons from 16bit 
 *          NE executables so that their icons can be shown again.
 * Changes: 25.04.2020  - Created
 */
#include "extract.h"
#ifdef EXTRACTICON_HOOK
#include "detour.h"
#include <inttypes.h>

fpLookupIconIdFromDirectoryEx pLookupIconIdFromDirectoryEx = NULL;
fpCreateIconFromResourceEx pCreateIconFromResourceEx = NULL;

typedef UINT (WINAPI *fpPrivateExtractIconsW)(
	LPCWSTR szFileName,
	int     nIconIndex,
	int     cxIcon,
	int     cyIcon,
	HICON   *phicon,
	UINT    *piconid,
	UINT    nIcons,
	UINT    flags
	);

typedef BOOL (WINAPI *fpReadFile)(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
	);

fpPrivateExtractIconsW PrivateExtractIconsWReal = NULL;
fpReadFile ReadFileReal;

static CRITICAL_SECTION m_cs;
static BOOL m_fIsMZ = FALSE;
static UINT m_result = 0;
static int m_nIconIndex, m_cxIcon, m_cyIcon;
static HICON *m_phicon;
static UINT *m_piconid, m_nIcons, m_flags;

static BOOL InitExtractIcon(void)
{
	HMODULE hUser = GetModuleHandle(_T("user32.dll"));
	if (!hUser ||
		!(pLookupIconIdFromDirectoryEx = (fpLookupIconIdFromDirectoryEx)GetProcAddress(hUser, "LookupIconIdFromDirectoryEx")) ||
		!(pCreateIconFromResourceEx = (fpCreateIconFromResourceEx)GetProcAddress(hUser, "CreateIconFromResourceEx")))
		return FALSE;
	return TRUE;
}

/* You may be surprised that we hook ReadFile here, in order to find
 * out if a MZ file is parsed, but the hook doesn't unnecessarily slow us
 * down, as only PrivateExtractIconsWHook uses this function in user32.dll 
 * Using this method prevents us from having to recreate the code of PrivateExtractIconsW
 */
BOOL ReadFileHook(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	BOOL result = ReadFileReal(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (result && nNumberOfBytesToRead == 12)
	{
		if (m_fIsMZ = *((WORD*)lpBuffer) == 'ZM')
		{
			if (!pLookupIconIdFromDirectoryEx)
			{
				if (!InitExtractIcon())
				{
					pLookupIconIdFromDirectoryEx = NULL;
					m_fIsMZ = FALSE;
					return result;
				}
			}

			m_result = ExtractIconFromEXE(hFile,
				m_nIconIndex,
				m_cxIcon,
				m_cyIcon,
				m_phicon,
				m_piconid,
				m_nIcons,
				m_flags);
		}
	}
	return result;
}

UINT WINAPI PrivateExtractIconsWHook(LPCWSTR szFileName, int nIconIndex, int cxIcon, int cyIcon, 
	HICON *phicon, UINT *piconid, UINT nIcons, UINT flags)
{
	UINT     result;

	TRACE("PrivateExtractIconsWHook(%S)", szFileName);
	EnterCriticalSection(&m_cs);
	m_nIconIndex = nIconIndex;
	m_cxIcon = cxIcon;
	m_cyIcon = cyIcon;
	m_phicon = phicon;
	m_piconid = piconid;
	m_nIcons = nIcons;
	m_flags = flags;
	result = PrivateExtractIconsWReal(szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid, nIcons, flags);
	if (m_fIsMZ)
	{
		result = m_result;
		m_fIsMZ = FALSE;
	}
	LeaveCriticalSection(&m_cs);
	return result;
}

BOOL HookExtractIcon()
{
	HMODULE hUser = GetModuleHandle(_T("user32.dll"));

	InitializeCriticalSection(&m_cs);
	if (hUser && !PrivateExtractIconsWReal)
	{
		// As many functions are using this, we cannot do IAT hooking but have to do an inline hook here
		PrivateExtractIconsWReal = GetProcAddress(hUser, "PrivateExtractIconsW");
		PrivateExtractIconsWReal = (fpPrivateExtractIconsW)Hook_Inline(PrivateExtractIconsWReal, PrivateExtractIconsWHook);
	}
	if (hUser)
	{
		if (Hook_IAT_x64_IAT((LPBYTE)hUser, "api-ms-win-core-file-l1-2-1.dll", "ReadFile", ReadFileHook, &ReadFileReal))
			Hook_IAT_x64_IAT((LPBYTE)hUser, "api-ms-win-core-file-l1-1-0.dll", "ReadFile", ReadFileHook, &ReadFileReal);
	}
	return hUser != NULL;
}
#endif
