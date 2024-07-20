/* Project: pificon
 * Module : dllmain
 * Author : leecher@dose.0wnz.at
 * Descr. : We need to reconstruct PIF file parser for icons on x64
 *          explorer. This adds a custom COM component that parses the .PIF
 *          files as a shell extension to show the correct icon in explorer.
 *
 *          Main module
 * Changes: 17.07.2024  - Created
 */
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

// Define the CLSID for your COM object
// D4C4511B-A392-498F-806E-02312ED07CC8
const CLSID CLSID_PifFile = { 0xD4C4511B,0xA392,0x498F,0x80,0x6E,0x02,0x31,0x2E,0xD0,0x7C,0xC8 };

HINSTANCE g_hInstance = NULL;

#if _WIN32_WINNT < 0x0600
LONG RegDeleteTreeW(HKEY hKey, WCHAR * szSubKey)
{
    HKEY hKeyNew;
    WCHAR szName[256];
    LONG lResult = RegOpenKeyW(hKey, szSubKey, &hKeyNew);

    if (lResult != ERROR_SUCCESS)
    {
        return lResult;
    }
	szName[0] = 0;
    while (ERROR_SUCCESS == RegEnumKeyW(hKeyNew, 0, szName, 256))
    {
        RegDeleteTreeW(hKeyNew, szName);
    }
    RegCloseKey(hKeyNew);
    return RegDeleteKeyW(hKey, szSubKey);
}

#endif

// Helper function to create registry key and set value
LONG CreateRegistryKey(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName, LPCWSTR data) {
    HKEY hSubKey;
    LONG hr = RegCreateKeyExW(hKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubKey, NULL);

    if (hr == ERROR_SUCCESS) {
        hr = RegSetValueExW(hSubKey, valueName, 0, REG_SZ, (const BYTE*)data, (lstrlenW(data) + 1) * sizeof(WCHAR));
        RegCloseKey(hSubKey);
    }
    return hr;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void) {
    WCHAR szModule[MAX_PATH], szCLSID[MAX_PATH], szSubKey[MAX_PATH];
	DWORD cbData;
	LONG hr;
	HKEY hSubKey;

    if (!GetModuleFileNameW(g_hInstance, szModule, sizeof(szModule)/sizeof(szModule[0]))) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

	StringFromGUID2(&CLSID_PifFile, szCLSID, sizeof(szCLSID)/sizeof(szCLSID[0]));

    // On NT/2K, put our extension in the "approved" list.
    if ( 0 == (GetVersion() & 0x80000000) ) {
        wsprintfW(szSubKey, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved");
        hr = CreateRegistryKey(HKEY_LOCAL_MACHINE, szSubKey, szCLSID, L"PIF shortcut");
		if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);
    }
    
    wsprintfW(szSubKey, L"CLSID\\%s", szCLSID);
    hr = CreateRegistryKey(HKEY_CLASSES_ROOT, szSubKey, NULL, L"PIF shortcut");
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);

    wsprintfW(szSubKey, L"CLSID\\%s\\InprocServer32", szCLSID);
    hr = CreateRegistryKey(HKEY_CLASSES_ROOT, szSubKey, NULL, szModule);
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);

    hr = CreateRegistryKey(HKEY_CLASSES_ROOT, szSubKey, L"ThreadingModel", L"Apartment");
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);

	// Backup old IconHandler for .PIF
	hr = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"piffile\\shellex\\IconHandler", 0, KEY_READ | KEY_WRITE, &hSubKey);
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);
	cbData = sizeof(szSubKey);
	hr = RegQueryValueExW(hSubKey, NULL, NULL, NULL, (LPBYTE)szSubKey, &cbData);
    if (hr != ERROR_SUCCESS && RegQueryValueExW(hSubKey, L"OldHandler", NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
		hr = RegSetValueExW(hSubKey, L"OldHandler", 0, REG_SZ, (LPBYTE)szSubKey, cbData);
	}

	// Set new IconHandler to be us
	hr = RegSetValueExW(hSubKey, NULL, 0, REG_SZ, (LPBYTE)szCLSID, (lstrlenW(szCLSID)+1)*sizeof(WCHAR));
	RegCloseKey(hSubKey);
	
    return HRESULT_FROM_WIN32(hr);
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
    WCHAR szCLSID[MAX_PATH], szSubKey[MAX_PATH];
	DWORD cbData;
	LONG hr;
	HKEY hSubKey;

    StringFromGUID2(&CLSID_PifFile, szCLSID, sizeof(szCLSID)/sizeof(szCLSID[0]));


    // On NT/2K, remove our extension from the "approved" list.
    if ( 0 == (GetVersion() & 0x80000000) ) {
        wsprintfW(szSubKey, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved");
		hr = RegOpenKeyExW(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_SET_VALUE, &hSubKey);
        if (hr == ERROR_SUCCESS) {
			hr = RegDeleteValueW(hSubKey, szCLSID);
			RegCloseKey(hSubKey);
		}
    }

    // Restore the old DefaultIcon value from the backup we made during
    // registration.
	hr = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"piffile\\shellex\\IconHandler", 0, KEY_READ | KEY_WRITE, &hSubKey);
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);
	cbData = sizeof(szSubKey);
	hr = RegQueryValueExW(hSubKey, L"OldHandler", NULL, NULL, (LPBYTE)szSubKey, &cbData);
    if (hr == ERROR_SUCCESS) {
		hr = RegSetValueExW(hSubKey, NULL, 0, REG_SZ, (LPBYTE)szSubKey, cbData);
        if (hr == ERROR_SUCCESS) RegDeleteValueW(hSubKey, L"OldHandler");
	} else {
		hr = RegSetValueExW(hSubKey, NULL, 0, REG_SZ, (LPBYTE)L"{00021401-0000-0000-C000-000000000046}", 39*sizeof(WCHAR));
	}
    if (hr != ERROR_SUCCESS) return HRESULT_FROM_WIN32(hr);

	wsprintfW(szSubKey, L"CLSID\\%s", szCLSID);
    hr = RegDeleteTreeW(HKEY_CLASSES_ROOT, szSubKey);
    if (hr == ERROR_FILE_NOT_FOUND) hr = ERROR_SUCCESS;

    return HRESULT_FROM_WIN32(hr);
}


BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_hInstance = hInstance;
        DisableThreadLibraryCalls(hInstance);
    }
    return TRUE;
}