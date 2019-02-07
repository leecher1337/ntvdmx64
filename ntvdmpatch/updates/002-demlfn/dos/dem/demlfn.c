/*
 * DEM Long Filename support module
 *
 * leecher@dose.0wnz.at    02/2019
 */
#include "dem.h"
#include "demlfn.h"

#include <softpc.h>
#include <mvdm.h>
#include <vdmapi.h>
#include <nt_vdd.h>

/* Not present in NT4: */
typedef struct _RTLP_CURDIR_REF {
    LONG RefCount;
    HANDLE Handle;
} RTLP_CURDIR_REF, *PRTLP_CURDIR_REF;
typedef struct RTL_RELATIVE_NAME_U {
    UNICODE_STRING RelativeName;
    HANDLE ContainingDirectory;
    PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;

// --- Defines
#define FLAG_INUSE	0x4000	// No idea what it really is called...

#ifndef GetRModeVDMPointer
#define GetRModeVDMPointer(Address) GetVDMAddr(Address>>16, LOWORD(Address))
#endif

// --- Structures
typedef struct tag_HTBL {
    PBYTE pMem;
    LONG  nAllocated;
    LONG  lNextFree;
    LONG  nEntries;
} HTBL;

typedef struct tag_HTBLENTRY {
    HANDLE hFind;
    USHORT RequiredAttributes;
    USHORT AllowedAttributes;
    UNICODE_STRING FileName;
    USHORT PDB;
} HTBLENTRY;

typedef struct tag_LFNVOLINFO {
    DWORD nFileSystemNameSize;
    LPSTR lpFileSystemNameBuffer;
    DWORD nMaxComponentLen;
    DWORD nMaxPathLen;
    DWORD FileSystemFlags;
} LFNVOLINFO;

typedef union tag_DOSATTR {
    DOSTIME t;
    WORD Attributes;
    DWORD FileSize;
} DOSATTR;

typedef const UNICODE_STRING *PCUNICODE_STRING;

// --- Globals
UNICODE_STRING SlashSlashDotSlash;
PDOSWOWGETTDBDIR DosWowGetTDBDir = NULL;
UNICODE_STRING DosDevicePrefix;
PDOSWOWDODIRECTHDPOPUP DosWowDoDirectHDPopup = NULL;
UNICODE_STRING DosDeviceUNCPrefix;
ULARGE_INTEGER gFileTimeDos0;
BOOL gdwLog = FALSE;
DOSWOWDATA DosWowData;
UNICODE_STRING ColonSlashSlash;
HTBL gSearchHandleTable = {0};
PDOSWOWUPDATETDBDIR DosWowUpdateTDBDir = NULL;

NTSTATUS (*pfnUnicodeStringToDestString)(
    POEM_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString, BOOLEAN Counted
);
NTSTATUS (*pfnSrcStringToUnicodeString)(
    PUNICODE_STRING DestinationString,
    POEM_STRING    SourceString,
    BOOLEAN         AllocateDestinationString
);

LPSTR rgCurDirs[26];	// A-Z
BOOL gfInitCDSPtr = FALSE;

// Imports
extern VOID cmdCheckTempInit(VOID);
extern LPSTR cmdCheckTemp(LPSTR lpszzEnv);

// Local prototypes
NTSTATUS DemUnicodeStringToOemString(POEM_STRING DestinationString, PUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString, BOOLEAN Counted);
NTSTATUS DemUnicodeStringToAnsiString(PANSI_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString, BOOLEAN Counted);
VOID demSetLFNApiTranslation(BOOL bAnsi);
VOID dempStringInitZeroUnicode(UNICODE_STRING *String, WCHAR *Buffer, WORD MaximumLength);
BOOL dempStringPrefixUnicode(UNICODE_STRING *Prefix, UNICODE_STRING *String, BOOL bIgnoreCase);
BOOL dempStringDeleteCharsUnicode(UNICODE_STRING *String, WORD nOffset, int nChars);
int dempStringFindLastChar(UNICODE_STRING *FileName, WCHAR SourceCharacter, BOOL bIgnoreCase);
VOID dempLFNNormalizePath(UNICODE_STRING *Path);
HRESULT dempQuerySubst(WCHAR Drive, UNICODE_STRING *String);
HRESULT dempExpandSubst(UNICODE_STRING *Path, BOOL bShort);
HRESULT dempGetFullPathName(UNICODE_STRING *Path, UNICODE_STRING *CanonicalizedPath, BOOL bExpandSubst);
HRESULT dempGetShortPathName(UNICODE_STRING *Path, UNICODE_STRING *CanicalizedPath, BOOL bExpandSubst);
BOOL dempIsShortNameW(LPWSTR Name, WORD Length, BOOL bAllowWildcards);
LPWSTR dempSkipPathTypeIndicatorW(LPWSTR Path);
HRESULT dempGetLongPathName(UNICODE_STRING *ShortPath, UNICODE_STRING *LongPath, BOOL bExpandSubst);
HRESULT demLFNGetPathName(LPSTR lpPath, LPSTR lpResult, int SubFunction, BOOL bExpandSubst);
HRESULT dempLFNCheckDirectory(UNICODE_STRING *Directory);
HRESULT dempLFNCreateSubst(DWORD DriveNo, UNICODE_STRING *Directory);
HRESULT dempLFNRemoveSubst(WORD DriveNo);
HRESULT dempLFNQuerySubst(WORD DriveNo, UNICODE_STRING *Path);
HRESULT demLFNSubstControl(WORD SubFunction, WORD DriveNo, LPSTR lpPath);
BOOL dempLFNMatchFile(LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes);
HRESULT dempLFNFindFirstFile(HANDLE *phFind, UNICODE_STRING *FileName, LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes);
HRESULT dempLFNFindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes);
HRESULT dempLFNAllocateHandleEntry(PUSHORT pDosHandle, HTBLENTRY **ppEntry);
int dempLFNFreeHandleEntry(HTBLENTRY *pEntry);
HTBLENTRY * dempLFNGetHandleEntry(USHORT DosHandle);
VOID dempLFNCloseSearchHandles();
HRESULT dempLFNConvertFileTime(LPFILETIME lpLocalFileTime, LPFILETIME lpFileTime, USHORT DateTimeFormat);
HRESULT dempLFNConvertFindDataUnicodeToOem(LPWIN32_FIND_DATAA FindDataOem, LPWIN32_FIND_DATAW FindDataUnicode, USHORT usDateTimeFormat, PUSHORT pusUnicodeConversionFlags, LPSTR lpFullFilename, LPSTR lpShortFilename);
int demLFNFindFirstFile(LPSTR lpFileName, LPWIN32_FIND_DATAA DosFindFileData, USHORT usDateTimeFormat, BYTE RequiredAttributes, BYTE AllowedAttributes, PUSHORT pusUnicodeConversionFlags, PUSHORT pDosHandle, LPSTR lpFullFilename, LPSTR lpShortFilename);
HRESULT demLFNFindNextFile(USHORT DosHandle, LPWIN32_FIND_DATAA DosFindFileData, USHORT usDateTimeFormat, PUSHORT pusUnicodeConversionFlags, LPSTR lpFullFilename, LPCSTR lpShortFilename);
HRESULT demLFNFindClose(USHORT hFind);
BOOL dempValidateDirectory(PCDS pcds, UCHAR Drive);
PCDS dempGetCDSPtr(UCHAR Drive);
BOOL dempUpdateCDS(UCHAR Drive, PCDS pcds);
HRESULT dempGetCurrentDirectoryTDB(UCHAR Drive, LPSTR pszDir);
VOID dempSetCurrentDirectoryTDB(UCHAR Drive, LPSTR pszDir);
HRESULT dempGetCurrentDirectoryCDS(UCHAR Drive, LPSTR pszDir);
HRESULT dempSetCurrentDirectoryCDS(UCHAR Drive, LPSTR pszDir);
BOOL dempGetCurrentDirectoryWin32(UCHAR Drive, LPSTR pPath);
HRESULT dempSetCurrentDirectoryWin32(UCHAR Drive, LPSTR pszDir);
HRESULT demGetCurrentDirectoryLong(UCHAR Drive, LPSTR lpResult, ULONG Type);
HRESULT demSetCurrentDirectoryLong(UCHAR Drive, LPSTR lpPath, ULONG Type);
HRESULT dempLFNSetCurrentDirectory(UNICODE_STRING *Path, PUINT pDriveNum);
HRESULT demLFNDirectoryControl(USHORT SubFunction, LPSTR pszDir);
HRESULT demLFNGetCurrentDirectory(UCHAR Drive, LPSTR pszDir);
HRESULT demLFNMoveFile(LPSTR lpszPathSrc, LPSTR lpszPathDest);
HRESULT demLFNGetVolumeInformation(LPSTR lpRootPathName, LFNVOLINFO *VolumeInfo);
HRESULT dempLFNSetFileTime(DWORD Action, UNICODE_STRING *FileName, DOSTIME *DosTime);
BOOL dempUseUTCTimeByHandle(HANDLE FileHandle);
BOOL dempUseUTCTimeByName(UNICODE_STRING *FileName);
HRESULT demLFNOpenFile(PCSZ lpFileName, USHORT regbx, USHORT regcx, USHORT regdx, USHORT regdi, PUSHORT pregAX, PUSHORT pregCX);
HRESULT demLFNDeleteFile(LPSTR pszFile, USHORT RequiredAttributes, USHORT AllowedAttributes, BOOL bAllowWildcards);
HRESULT demLFNGetFileInformationByHandle(SHORT hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation);
HRESULT demLFNGenerateShortFileName(LPSTR lpShortFileName, LPSTR lpLongFileName, WORD Format, WORD Charset);
ULONG dempWOWLFNReturn(NTSTATUS Status);
VOID demInitCDSPtr();
VOID dempCheckTempEnvironmentVariables();
HRESULT dempLFNGetFileTime(DWORD Action, UNICODE_STRING *FileName, DOSTIME *DosTime);
HRESULT demLFNGetSetFileAttributes(DWORD Action, PCSZ SourceString, DOSATTR *DosAttr);
HRESULT demLFNDispatch(PVOID pUserFrame, BOOL ProtectedMode, PUSHORT pResult);

#if DBG
VOID dempLFNLog(LPCSTR pszFormat, ...)
{
    CHAR SourceString[512];
    va_list arglist;

    va_start(arglist, pszFormat);
    if (gdwLog) {
        wvsprintfA(SourceString, pszFormat, arglist);
        OutputDebugStringOem(SourceString);
    }
}
#define DBG_LOG(x) dempLFNLog x
#else
#define DBG_LOG(x)
#endif


NTSTATUS DemUnicodeStringToOemString(POEM_STRING DestinationString, PUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString, BOOLEAN Counted)
{
    PCHAR pBuffer;
    NTSTATUS result;

    if (Counted) {
        pBuffer = NULL;
        if (!AllocateDestinationString && DestinationString->MaximumLength > 0)
            pBuffer = DestinationString->Buffer;
        result = RtlUnicodeStringToCountedOemString(DestinationString, SourceString, AllocateDestinationString);
        if (result >= 0) {
            if (DestinationString->Length >= DestinationString->MaximumLength) {
                if (DestinationString->Buffer) {
                    result = STATUS_BUFFER_OVERFLOW;
                } else if ( pBuffer ) {
                    *pBuffer = 0;
                }
            } else {
                DestinationString->Buffer[DestinationString->Length] = 0;
            }
        }
    } else {
        result = RtlUnicodeStringToOemString(DestinationString, SourceString, AllocateDestinationString);
    }
    return result;
}

NTSTATUS DemUnicodeStringToAnsiString(PANSI_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString, BOOLEAN Counted)
{
    return RtlUnicodeStringToAnsiString(DestinationString, SourceString, AllocateDestinationString);
}

VOID demSetLFNApiTranslation(BOOL bAnsi)
{
    if ( bAnsi ) {
        pfnUnicodeStringToDestString = DemUnicodeStringToAnsiString;
        pfnSrcStringToUnicodeString = RtlAnsiStringToUnicodeString;
    } else {
        pfnUnicodeStringToDestString = DemUnicodeStringToOemString;
        pfnSrcStringToUnicodeString = RtlOemStringToUnicodeString;
    }
}

DOSENV *dempGetDosUserEnvironment()
{
    PDOSPDB pPDB = (PDOSPDB)GetVDMAddr(*pusCurrentPDB, 0);
    return (DOSENV*)GetRModeVDMPointer(pPDB->PDB_User_stack);
}

VOID dempInitLFNSupport()
{
    LARGE_INTEGER Time;
    TIME_FIELDS TimeFields;

    RtlInitUnicodeString(&DosDevicePrefix, L"\\??\\");
    RtlInitUnicodeString(&DosDeviceUNCPrefix, L"\\??\\UNC\\");
    RtlInitUnicodeString(&SlashSlashDotSlash, L"\\\\.\\");
    RtlInitUnicodeString(&ColonSlashSlash, L":\\\\");
    demSetLFNApiTranslation(FALSE);
    TimeFields.Day = 0;
    TimeFields.Minute = 0;
    TimeFields.Second = 0;
    TimeFields.Milliseconds = 0;
    TimeFields.Weekday = 0;
    TimeFields.Year = 1980;
    TimeFields.Month = 1;
    TimeFields.Day = 1;
    RtlTimeFieldsToTime(&TimeFields, &Time);
    gFileTimeDos0.QuadPart = Time.QuadPart;
}

VOID dempStringInitZeroUnicode(UNICODE_STRING *String, WCHAR *Buffer, WORD MaximumLength)
{
    String->Length = 0;
    String->MaximumLength = MaximumLength;
    String->Buffer = Buffer;
    if (Buffer)
        *Buffer = 0;
}

BOOL dempStringPrefixUnicode(UNICODE_STRING *Prefix, UNICODE_STRING *String, BOOL bIgnoreCase)
{
    ULONG i;

    if ( String->Length < Prefix->Length )
        return FALSE;
    if (bIgnoreCase) {
        for (i = 0; i<Prefix->Length / sizeof(WCHAR); i++) {
            if (Prefix->Buffer[i] != String->Buffer[i]) {
                if (RtlUpcaseUnicodeChar(Prefix->Buffer[i]) != RtlUpcaseUnicodeChar(String->Buffer[i]))
                    return FALSE;
            }
        }
        return TRUE;
    }
    if ((Prefix->Length / sizeof(WCHAR)) == 0)
        return TRUE;
    for (i = 0; i<Prefix->Length / sizeof(WCHAR); i++)
        if (Prefix->Buffer[i] != String->Buffer[i]) return FALSE;
    return TRUE;
}

BOOL dempStringDeleteCharsUnicode(UNICODE_STRING *String, WORD nOffset, int nChars)
{
    if ( nOffset <= String->Length ) {
        if (nChars < String->Length - nOffset) {
            String->Length -= nChars;
            memmove(String->Buffer + nOffset, String->Buffer + nOffset + nChars, String->Length - nOffset);
            String->Buffer[String->Length] = 0;
        } else {
            String->Length = nOffset;
        }
        String->Buffer[String->Length] = 0;
        return TRUE;
    }
    return FALSE;
}

int dempStringFindLastChar(UNICODE_STRING *FileName, WCHAR SourceCharacter, BOOL bIgnoreCase)
{
    int Length;
    WCHAR SourceCharactera;

    if (bIgnoreCase) {
        SourceCharactera = RtlUpcaseUnicodeChar(SourceCharacter);
        for (Length = FileName->Length / sizeof(WCHAR); Length >= 0; Length--) {
            if (SourceCharactera == RtlUpcaseUnicodeChar(FileName->Buffer[Length]))
                return Length * sizeof(WCHAR);
        }
    } else {
        for (Length = FileName->Length / sizeof(WCHAR); Length >= 0; Length--) {
            if (SourceCharacter == FileName->Buffer[Length])
                return Length * sizeof(WCHAR);
        }
    }
    return -1;
}

VOID dempLFNNormalizePath(UNICODE_STRING *Path)
{
    UNICODE_STRING DestinationString;

    if (Path->Length > 8) {
        RtlInitUnicodeString(&DestinationString, Path->Buffer + 1);
        if ( dempStringPrefixUnicode(&ColonSlashSlash, &DestinationString, 1) )
            dempStringDeleteCharsUnicode(Path, 0, 4);
    }
}

HRESULT dempQuerySubst(WCHAR Drive, UNICODE_STRING *String)
{
    DWORD nChars;
    WCHAR DeviceName[3];

    DeviceName[0] = Drive;
    DeviceName[1] = ':';
    DeviceName[2] = 0;
    nChars = QueryDosDeviceW(DeviceName, String->Buffer, String->MaximumLength / sizeof(WCHAR));
    if (nChars) {
        String->Length = sizeof(WCHAR) * nChars - 2 * sizeof(WCHAR);
        if (dempStringPrefixUnicode(&DosDeviceUNCPrefix, String, TRUE)) {
            dempStringDeleteCharsUnicode(String, 0, *&DosDeviceUNCPrefix.Length - 4);
            *String->Buffer = '\\';
            return S_OK;
        }
        if (dempStringPrefixUnicode(&DosDevicePrefix, String, TRUE)) {
            dempStringDeleteCharsUnicode(String, 0, DosDevicePrefix.Length);
            return S_OK;
        }
        return HRESULT_FROM_WIN32(ERROR_NOT_SUBSTED);
    }
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT dempExpandSubst(UNICODE_STRING *Path, BOOL bShort)
{
    HRESULT result;
    DWORD PathLen;
    WCHAR szLongPath[MAX_PATH];
    UNICODE_STRING LongPath;
    WORD CharType;
    WORD Length;

    if ( !GetStringTypeW(CT_CTYPE1, Path->Buffer, 1, &CharType) )
        return HRESULT_FROM_WIN32(GetLastError());
    if ( !(CharType & C1_ALPHA) || Path->Buffer[1] != ':' )
        return HRESULT_FROM_WIN32(ERROR_NOT_SUBSTED);
    dempStringInitZeroUnicode(&LongPath, szLongPath, sizeof(szLongPath));
    result = dempQuerySubst(Path->Buffer[0], &LongPath);
    if ( !SUCCEEDED(result) )
        return result;
    Length = LongPath.Length;
    if ( bShort ) {
        PathLen = GetShortPathNameW(szLongPath, szLongPath, sizeof(szLongPath)/sizeof(szLongPath[0]));
        if (!PathLen)
            return HRESULT_FROM_WIN32(GetLastError());
        Length = (sizeof(WCHAR) * PathLen);
        if (Length >= sizeof(szLongPath)/sizeof(szLongPath[0]))
            return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }
    if (szLongPath[Length - 1] == '\\')
        Length -= 2;
    if (Path->Length + Length - 4 <= Path->MaximumLength) {
        memmove(Path->Buffer + Length - 4, Path->Buffer, Path->Length);
        memcpy(Path->Buffer, szLongPath, Length);
        Path->Length += Length - 4;
        return S_OK;
    }
    return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
}

HRESULT dempGetFullPathName(UNICODE_STRING *Path, UNICODE_STRING *CanonicalizedPath, BOOL bExpandSubst)
{
    ULONG Length;
    HRESULT result;

    Length = RtlGetFullPathName_U(Path->Buffer, CanonicalizedPath->MaximumLength, CanonicalizedPath->Buffer, FALSE);
    if ( !Length )
        return HRESULT_FROM_WIN32(GetLastError());
    if (CanonicalizedPath->MaximumLength <= Length) {
        CanonicalizedPath->Length = 0;
        return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }
    CanonicalizedPath->Length = Length;
    if (!SUCCEEDED(dempStringPrefixUnicode(&SlashSlashDotSlash, CanonicalizedPath, 1)))
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    if (bExpandSubst && !SUCCEEDED(result = dempExpandSubst(CanonicalizedPath, FALSE)) &&
            RtlNtStatusToDosError(result) != ERROR_NOT_SUBSTED) {
        return result;
    }
    return S_OK;
}

HRESULT dempGetShortPathName(UNICODE_STRING *Path, UNICODE_STRING *CanicalizedPath, BOOL bExpandSubst)
{
    HRESULT result;
    DWORD Length;

    result = dempGetFullPathName(Path, CanicalizedPath, bExpandSubst);
    if (SUCCEEDED(result)) {
        if (Length = GetShortPathNameW(CanicalizedPath->Buffer, CanicalizedPath->Buffer, CanicalizedPath->MaximumLength / sizeof(WCHAR))) {
            Length *= sizeof(WCHAR);
            if (CanicalizedPath->MaximumLength <= Length) {
                CanicalizedPath->Length = 0;
                return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            } else {
                CanicalizedPath->Length = Length;
                return S_OK;
            }
        } else {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }
    return result;
}

BOOL dempIsShortNameW(LPWSTR Name, WORD Length, BOOL bAllowWildcards)
{
    static DWORD rgdwIllegalMask[] = {0xFFFFFFFF, 0xFC009C05, 0x38000000, 10000000};
    USHORT pos = 0;
    BOOL result;
    BYTE pChr;
    char Buffer[MAX_PATH];
    UNICODE_STRING usName;
    OEM_STRING sName;
    USHORT remain;
    BOOL bExt;

    ASSERT(Name);
    if ( Length > 12 ) return FALSE;
    if ( !Length ) return TRUE;
    if ( *Name != '.' ) {
        usName.Length = Length * sizeof(WCHAR);
        usName.MaximumLength = usName.Length;
        usName.Buffer = Name;
        sName.Length = 0;
        sName.MaximumLength = sizeof(Buffer);
        sName.Buffer = Buffer;
        if (!NT_SUCCESS(pfnUnicodeStringToDestString(&sName, &usName, FALSE, FALSE)))
            return FALSE;

        bExt = FALSE;
        if ( sName.Length ) {
            for (remain = Length - 1; pos < sName.Length; pos++, remain--) {
                if ( IsDBCSLeadByte(pChr) ) {
                    if ( !bExt && pos >= 7 || pos == sName.Length - 1 )
                        return FALSE;
                } else {
                    if ( pChr < 0x80
                            && (1 << pChr % 32) & rgdwIllegalMask[pChr >> 5]
                            && (!bAllowWildcards || pChr != '?' && pChr != '*') ) {
                        return FALSE;
                    }
                    if ( pChr == '.' ) {
                        if ( bExt || remain > 3 )
                            return FALSE;
                        bExt = TRUE;
                    }
                    if ( pos >= 8 && !bExt )
                        return FALSE;
                }
            }
        }
        return TRUE;
    }
    if ( Length != 1 && (Length != 2 || Name[1] != '.') )
        return FALSE;
    return TRUE;
}

LPWSTR dempSkipPathTypeIndicatorW(LPWSTR Path)
{
    int nSlashes;

    switch(RtlDetermineDosPathNameType_U(Path)) {
    case RtlPathTypeUnknown:
        for (Path+=2, nSlashes = 2; *Path && nSlashes > 0; Path++)
            if (IS_ASCII_PATH_SEPARATOR(*Path)) nSlashes--;
        return Path;
    case RtlPathTypeUncAbsolute:
        return Path + 3;
    case RtlPathTypeDriveAbsolute:
        return Path + 2;
    case RtlPathTypeDriveRelative:
        return Path + 1;
    case RtlPathTypeRooted:
        return Path;
    case RtlPathTypeRelative:
        return Path + 4;
    default:
        return NULL;
    }
}

BOOL demIsShortPathName(PCSZ PathName, BOOL bAllowWildcards)
{
    LPWSTR pChr, pNext;
    STRING DestinationString;

    RtlInitString(&DestinationString, PathName);
    if (NT_SUCCESS(pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, FALSE))) {
        if (pChr = dempSkipPathTypeIndicatorW(NtCurrentTeb()->StaticUnicodeString.Buffer)) {
            do {
                while (IS_ASCII_PATH_SEPARATOR(*pChr)) pChr++;
                if ( !*pChr )
                    break;
                for ( pNext = pChr + 1; IS_ASCII_PATH_SEPARATOR(*pNext) ; pNext++ );
                if ( !dempIsShortNameW(pChr, pNext - pChr, bAllowWildcards && !*pNext) )
                    return FALSE;
                pChr = pNext + 1;
            } while (*pNext);
        }
    }
    return TRUE;
}

HRESULT dempGetLongPathName(UNICODE_STRING *ShortPath, UNICODE_STRING *LongPath, BOOL bExpandSubst)
{
    WCHAR *pszPath;
    HRESULT result;
    LPWSTR pPath;
    DWORD Count;
    PWSTR pLast;
    WORD Length;
    BYTE FileInfoBuf[sizeof(FILE_DIRECTORY_INFORMATION)+MAX_PATH*sizeof(WCHAR)];
    FILE_DIRECTORY_INFORMATION *pFileInformation = (FILE_DIRECTORY_INFORMATION*)&FileInfoBuf;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    UNICODE_STRING FileName;
    BOOL bEndOfPath;
    UNICODE_STRING Path;
    HANDLE FileHandle;
    WORD wchTotal;
    UNICODE_STRING UStringNtName;
    WCHAR *pDst;
    WCHAR wch;

    wchTotal = 0;
    bEndOfPath = FALSE;
    RtlInitUnicodeString(&UStringNtName, 0);
    pszPath = RtlAllocateHeap(RtlProcessHeap(), 0, MAX_PATH * sizeof(WCHAR));
    if (!pszPath)
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    dempStringInitZeroUnicode(&Path, pszPath, MAX_PATH * sizeof(WCHAR));
    Path.Length = RtlGetFullPathName_U(ShortPath->Buffer, Path.MaximumLength, Path.Buffer, FALSE);
    if (Path.Length) {
        if (Path.MaximumLength > Path.Length) {
            if (!bExpandSubst || SUCCEEDED(result = dempExpandSubst(&Path, 0)) || RtlNtStatusToDosError(result) == ERROR_NOT_SUBSTED) {
                switch (RtlDetermineDosPathNameType_U(Path.Buffer)) {
                // form: "\\server_name\share_name\rest_of_the_path"
                case RtlPathTypeUncAbsolute:
                    pPath = (LPWSTR)Path.Buffer + 2;
                    Count = 2;
                    while (Count && *pPath != UNICODE_NULL) {
                        if (IS_ASCII_PATH_SEPARATOR(*pPath))
                            Count--;
                        pPath++;
                    }
                    break;
                // form: "D:\rest_of_the_path"
                case RtlPathTypeDriveAbsolute:
                    pPath = (LPWSTR)Path.Buffer + 3;
                    break;
                default:
                    pPath = NULL;
                    result = HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME);
                    break;
                }
                if (pPath) {
                    pDst = LongPath->Buffer;
                    pLast = Path.Buffer;
                    result = S_OK;
                    while (TRUE) {
                        Length = (DWORD)((char *)pPath - (char *)pLast);
                        if (Length > 0) {
                            wchTotal += Length;
                            if (wchTotal >= LongPath->MaximumLength) {
                                result = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                                break;
                            }
                            RtlMoveMemory(pDst, pLast, Length);
                            pDst += Length / sizeof(WCHAR);
                        }
                        if (*pPath != UNICODE_NULL) {
                            while (*pPath != UNICODE_NULL) {
                                if (IS_ASCII_PATH_SEPARATOR(*pPath))
                                    break;
                                pPath++;
                            }
                        } else {
                            bEndOfPath = TRUE;
                        }
                        wch = *pPath;
                        *pPath = UNICODE_NULL;
                        if ( !RtlDosPathNameToNtPathName_U(Path.Buffer, &UStringNtName, &FileName.Buffer, NULL) ) {
                            result = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                            break;
                        }
                        if (bEndOfPath || !FileName.Buffer) {
                            FileName.Length = FileName.MaximumLength = 0;
                            bEndOfPath = TRUE;
                        } else {
                            // \??\C:\Temp
                            //        ^______FileName
                            FileName.Length = FileName.MaximumLength = UStringNtName.Length - ((char*)FileName.Buffer - (char*)UStringNtName.Buffer);
                            UStringNtName.Length = (char*)FileName.Buffer - (char*)UStringNtName.Buffer;
                            if ( UStringNtName.Buffer[UStringNtName.Length/sizeof(WCHAR)-2]  != L':' )
                                UStringNtName.Length-=sizeof(WCHAR);
                        }
                        UStringNtName.MaximumLength = UStringNtName.Length;
                        *pPath = wch;
                        InitializeObjectAttributes(&ObjectAttributes, &UStringNtName, OBJ_CASE_INSENSITIVE, NULL, NULL);
                        if (!NT_SUCCESS(result = NtOpenFile(&FileHandle, FILE_READ_DATA | SYNCHRONIZE, &ObjectAttributes,
                                                            &IoStatusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_FOR_BACKUP_INTENT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_WORD_ALIGNMENT)))
                            break;
                        result = NtQueryDirectoryFile(
                                     FileHandle,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &IoStatusBlock,
                                     pFileInformation,
                                     sizeof(FileInfoBuf),
                                     FileDirectoryInformation,
                                     TRUE,
                                     &FileName,
                                     FALSE);
                        NtClose(FileHandle);
                        RtlFreeUnicodeString(&UStringNtName);
                        UStringNtName.Buffer = 0;
                        if (!NT_SUCCESS(result)) break;
                        if (bEndOfPath) break;
                        wchTotal += pFileInformation->FileNameLength;
                        if (wchTotal >= LongPath->MaximumLength) {
                            result = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                            break;
                        }
                        RtlMoveMemory(pDst, pFileInformation->FileName, pFileInformation->FileNameLength);
                        pDst += pFileInformation->FileNameLength / sizeof(WCHAR);
                        if (*pPath == UNICODE_NULL)
                            break;
                        pLast = pPath;
                        pPath++;
                    }
                    if (SUCCEEDED(result)) {
                        *pDst = 0;
                        LongPath->Length = wchTotal;
                    }
                }
            }
        } else {
            result = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            Path.Length = 0;
        }
    } else {
        result = HRESULT_FROM_WIN32(GetLastError());
    }
    if (Path.Buffer)
        RtlFreeHeap(RtlProcessHeap(), 0, Path.Buffer);
    if (UStringNtName.Buffer)
        RtlFreeUnicodeString(&UStringNtName);
    return result;
}

HRESULT demLFNGetPathName(LPSTR lpPath, LPSTR lpResult, int SubFunction, BOOL bExpandSubst)
{
    HRESULT result;
    WCHAR Buffer[MAX_PATH];
    UNICODE_STRING UnicodeString;
    UNICODE_STRING String;
    STRING DestinationString;

    if (lpPath && lpResult) {
        RtlInitString(&DestinationString, lpPath);
        result = pfnSrcStringToUnicodeString(&UnicodeString, &DestinationString, TRUE);
        if (!NT_SUCCESS(result))
            return result;
        dempStringInitZeroUnicode(&String, Buffer, sizeof(Buffer));
        switch (SubFunction) {
        case 0:
            result = dempGetFullPathName(&UnicodeString, &String, bExpandSubst);
            break;
        case 1:
            result = dempGetShortPathName(&UnicodeString, &String, bExpandSubst);
            break;
        case 2:
            result = dempGetLongPathName(&UnicodeString, &String, bExpandSubst);
            break;
        default:
            result = HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
            break;
        }
        RtlFreeUnicodeString(&UnicodeString);

        if (SUCCEEDED(result)) {
            DestinationString.Buffer = lpResult;
            DestinationString.Length = 0;
            DestinationString.MaximumLength = MAX_PATH;
            result = pfnUnicodeStringToDestString(&DestinationString, &String, FALSE, FALSE);
        }
        return result;
    }
    return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
}

HRESULT dempLFNCheckDirectory(UNICODE_STRING *Directory)
{
    DWORD dwAttrib;

    dwAttrib = GetFileAttributesW(Directory->Buffer);
    if ( dwAttrib == -1 )
        return HRESULT_FROM_WIN32(GetLastError());
    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)?S_OK:HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
}

HRESULT dempLFNCreateSubst(DWORD DriveNo, UNICODE_STRING *Directory)
{
    HRESULT result;
    WCHAR TargetPath[MAX_PATH];
    WCHAR DeviceName[3];

    DeviceName[0] = DriveNo + 'A' - 1;
    DeviceName[1] = ':';
    DeviceName[2] = 0;
    if (QueryDosDeviceW(DeviceName, TargetPath, sizeof(TargetPath)/sizeof(WCHAR)))
        return HRESULT_FROM_WIN32(DriveNo); // Why..?
    else {
        if (GetLastError() == 2) {
            result = dempLFNCheckDirectory(Directory);
            if (!SUCCEEDED(result))
                return result;
            if (DefineDosDeviceW(0, DeviceName, Directory->Buffer))
                return S_OK;
        }
    }
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT dempLFNRemoveSubst(WORD DriveNo)
{
    HRESULT result;
    WCHAR DeviceName[3];

    DeviceName[0] = DriveNo + 'A' - 1;
    DeviceName[1] = ':';
    DeviceName[2] = 0;
    result = dempQuerySubst(DriveNo + 'A' - 1, &NtCurrentTeb()->StaticUnicodeString);
    if (result >= 0) {
        if (DefineDosDeviceW(DDD_REMOVE_DEFINITION, DeviceName, NtCurrentTeb()->StaticUnicodeString.Buffer))
            return S_OK;
        else
            return HRESULT_FROM_WIN32(GetLastError());
    }
    return result;
}

HRESULT dempLFNQuerySubst(WORD DriveNo, UNICODE_STRING *Path)
{
    return dempQuerySubst(DriveNo + 'A' - 1, Path);
}

HRESULT demLFNSubstControl(WORD SubFunction, WORD DriveNo, LPSTR lpPath)
{
    NTSTATUS Status;
    STRING DestinationString;

    switch (SubFunction) {
    case 0:
        RtlInitString(&DestinationString, lpPath);
        Status = pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, 0);
        if (NT_SUCCESS(Status))
            Status = dempLFNCreateSubst(DriveNo, &NtCurrentTeb()->StaticUnicodeString);
        return Status;
    case 1:
        return dempLFNRemoveSubst(DriveNo);
    case 2:
        Status = dempLFNQuerySubst(DriveNo, &NtCurrentTeb()->StaticUnicodeString);
        if (NT_SUCCESS(Status)) {
            DestinationString.Buffer = lpPath;
            DestinationString.Length = 0;
            DestinationString.MaximumLength = 260;
            Status = pfnUnicodeStringToDestString(&DestinationString, &NtCurrentTeb()->StaticUnicodeString, FALSE, FALSE);
        }
        return Status;
    default:
        return HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
    }
}

BOOL dempLFNMatchFile(LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes)
{
    return ( (BYTE)lpFindFileData->dwFileAttributes & (~ATTR_VOLUME_ID) & RequiredAttributes) == RequiredAttributes
           && !((BYTE)lpFindFileData->dwFileAttributes & ~AllowedAttributes & ATTR_ALL);
}

HRESULT dempLFNFindFirstFile(HANDLE *phFind, UNICODE_STRING *FileName, LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes)
{
    HANDLE hFind;
    HRESULT result;

    hFind = FindFirstFileW(FileName->Buffer, lpFindFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (dempLFNMatchFile(lpFindFileData, RequiredAttributes, AllowedAttributes)) {
                *phFind = hFind;
                return S_OK;
            }
        } while (FindNextFileW(hFind, lpFindFileData));
        SetLastError(ERROR_FILE_NOT_FOUND);
    }
    result = HRESULT_FROM_WIN32(GetLastError());
    if (hFind != INVALID_HANDLE_VALUE)
        FindClose(hFind);
    return result;
}

HRESULT dempLFNFindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData, BYTE RequiredAttributes, BYTE AllowedAttributes)
{
    while (FindNextFileW(hFindFile, lpFindFileData)) {
        if (dempLFNMatchFile(lpFindFileData, RequiredAttributes, AllowedAttributes))
            return S_OK;
    }
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT dempLFNAllocateHandleEntry(PUSHORT pDosHandle, HTBLENTRY **ppEntry)
{
    if (!gSearchHandleTable.pMem) {
        gSearchHandleTable.pMem = RtlAllocateHeap(RtlProcessHeap(), 0, 32*sizeof(HTBLENTRY));
        if (!gSearchHandleTable.pMem) return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
        gSearchHandleTable.nEntries = 0;
        gSearchHandleTable.nAllocated = 32;
        gSearchHandleTable.lNextFree = -1;
    }
    if (gSearchHandleTable.lNextFree == -1) {
        if (gSearchHandleTable.nEntries >= gSearchHandleTable.nAllocated) {
            PVOID pMem;

            if (gSearchHandleTable.nAllocated + 16 >= 0x3FFF) return STATUS_UNSUCCESSFUL;
            if (!(pMem = RtlReAllocateHeap(RtlProcessHeap(), 0, gSearchHandleTable.pMem,
                                           (gSearchHandleTable.nAllocated + 16) * sizeof(HTBLENTRY)))) return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
            gSearchHandleTable.pMem = pMem;
            gSearchHandleTable.nAllocated += 16;
        }
        *ppEntry = (HTBLENTRY*)(gSearchHandleTable.pMem + gSearchHandleTable.nEntries * sizeof(HTBLENTRY));
        gSearchHandleTable.nEntries++;
    } else {
        *ppEntry = (HTBLENTRY*)(gSearchHandleTable.pMem + gSearchHandleTable.lNextFree * sizeof(HTBLENTRY));
        gSearchHandleTable.lNextFree = *((LONG*)*ppEntry);
    }
    *pDosHandle = (*ppEntry - gSearchHandleTable.pMem)/sizeof(HTBLENTRY) | FLAG_INUSE;
    return S_OK;
}

int dempLFNFreeHandleEntry(HTBLENTRY *pEntry)
{
    LONG lEntry = ((PBYTE)pEntry - gSearchHandleTable.pMem)/sizeof(HTBLENTRY);

    if (lEntry == gSearchHandleTable.nEntries - 1) {
        for (--gSearchHandleTable.nEntries; gSearchHandleTable.lNextFree != -1;
                gSearchHandleTable.lNextFree = *((PLONG)(gSearchHandleTable.pMem +
                                               gSearchHandleTable.lNextFree * sizeof(HTBLENTRY))), gSearchHandleTable.nEntries--) {
            if (gSearchHandleTable.lNextFree != gSearchHandleTable.nEntries-1) break;
        }
        return gSearchHandleTable.lNextFree;
    } else {
        LONG lPrevFree = -1, lNextFree;

        for (lNextFree = gSearchHandleTable.lNextFree; lNextFree > lEntry;
                lNextFree = *((PLONG)(gSearchHandleTable.pMem + lNextFree * sizeof(HTBLENTRY)))) {
            lPrevFree = lNextFree;
        }
        if (lPrevFree == -1) {
            *((PLONG)pEntry) = gSearchHandleTable.lNextFree;
            gSearchHandleTable.lNextFree = lEntry;
        } else {
            PLONG plPrev = (PLONG)(gSearchHandleTable.pMem + lPrevFree * sizeof(HTBLENTRY));

            *((PLONG)pEntry) = *plPrev;
            *plPrev = lEntry;
        }
        pEntry->PDB = 0;
        return 0;
    }
}

HTBLENTRY * dempLFNGetHandleEntry(USHORT DosHandle)
{
    HTBLENTRY *pEntry;

    if ((DosHandle & FLAG_INUSE) && gSearchHandleTable.pMem) {
        DosHandle &= ~FLAG_INUSE;
        if (DosHandle < gSearchHandleTable.nEntries &&
                (pEntry = (HTBLENTRY*)(gSearchHandleTable.pMem + DosHandle * sizeof(HTBLENTRY))) &&
                pEntry->PDB == *pusCurrentPDB) {
            return pEntry;
        }
    }
    return NULL;
}

VOID dempLFNCloseSearchHandles()
{
    LONG i;
    HTBLENTRY *pEntry;

    for (i=gSearchHandleTable.nEntries-1; i>=0; i--) {
        if (pEntry = dempLFNGetHandleEntry(i | FLAG_INUSE)) {
            if (pEntry->hFind != INVALID_HANDLE_VALUE) FindClose(pEntry->hFind);
            dempLFNFreeHandleEntry(pEntry);
        }
    }
}

HRESULT dempLFNConvertFileTime(LPFILETIME lpLocalFileTime, LPFILETIME lpFileTime, USHORT DateTimeFormat)
{
    USHORT wDate, wTime;

    switch (DateTimeFormat) {
    case 0:
        *lpLocalFileTime = *lpFileTime;
        return S_OK;
    case 1:
        if (((ULARGE_INTEGER*)lpFileTime)->QuadPart > gFileTimeDos0.QuadPart) {
            if (!FileTimeToLocalFileTime(lpFileTime, lpLocalFileTime))
                return HRESULT_FROM_WIN32(GetLastError());
        } else {
            *lpLocalFileTime = *((LPFILETIME)&gFileTimeDos0.QuadPart);
        }
        if (FileTimeToDosDateTime(lpLocalFileTime, &wDate, &wTime)) {
            lpLocalFileTime->dwLowDateTime = (wTime | ((DWORD) wDate << 16));
            lpLocalFileTime->dwHighDateTime = 0;
            return S_OK;
        }
        return HRESULT_FROM_WIN32(GetLastError());
    default:
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }
}

HRESULT dempLFNConvertFindDataUnicodeToOem(LPWIN32_FIND_DATAA FindDataOem, LPWIN32_FIND_DATAW FindDataUnicode, USHORT usDateTimeFormat, PUSHORT pusUnicodeConversionFlags, LPSTR lpFullFilename, LPSTR lpShortFilename)
{
    UNICODE_STRING DestinationString;
    STRING FullFilename;
    HRESULT result;

    *pusUnicodeConversionFlags = 0;
    result = dempLFNConvertFileTime(&FindDataOem->ftLastWriteTime, &FindDataUnicode->ftLastWriteTime, usDateTimeFormat);
    if (SUCCEEDED(result)) {
        if ( !FindDataUnicode->ftCreationTime.dwLowDateTime && !FindDataUnicode->ftCreationTime.dwHighDateTime ) {
            FindDataUnicode->ftCreationTime.dwLowDateTime = FindDataUnicode->ftLastWriteTime.dwLowDateTime;
            FindDataUnicode->ftCreationTime.dwHighDateTime = FindDataUnicode->ftLastWriteTime.dwHighDateTime;
        }
        result = dempLFNConvertFileTime(&FindDataOem->ftCreationTime, &FindDataUnicode->ftCreationTime, usDateTimeFormat);
        if (SUCCEEDED(result)) {
            if ( !FindDataUnicode->ftLastAccessTime.dwLowDateTime && !FindDataUnicode->ftLastAccessTime.dwHighDateTime ) {
                FindDataUnicode->ftLastAccessTime.dwLowDateTime = FindDataUnicode->ftLastWriteTime.dwLowDateTime;
                FindDataUnicode->ftLastAccessTime.dwHighDateTime = FindDataUnicode->ftLastWriteTime.dwHighDateTime;
            }
            result = dempLFNConvertFileTime(
                         &FindDataOem->ftLastAccessTime,
                         &FindDataUnicode->ftLastAccessTime,
                         usDateTimeFormat);
            if (SUCCEEDED(result)) {
                FullFilename.Length = 0;
                FullFilename.Buffer = lpFullFilename?lpFullFilename:FindDataOem->cFileName;
                FullFilename.MaximumLength = sizeof(FindDataOem->cFileName);
                RtlInitUnicodeString(&DestinationString, FindDataUnicode->cFileName);
                result = pfnUnicodeStringToDestString(&FullFilename, &DestinationString, FALSE, TRUE);
                if (!NT_SUCCESS(result)) {
                    if (result != STATUS_UNMAPPABLE_CHARACTER)
                        return result;
                    *pusUnicodeConversionFlags = 1;
                }
                FullFilename.Length = 0;
                FullFilename.Buffer = lpShortFilename?lpShortFilename:FindDataOem->cAlternateFileName;
                FullFilename.MaximumLength = sizeof(FindDataOem->cAlternateFileName);
                RtlInitUnicodeString(&DestinationString, FindDataUnicode->cAlternateFileName);
                result = pfnUnicodeStringToDestString(&FullFilename, &DestinationString, FALSE, TRUE);
                if (!NT_SUCCESS(result)) {
                    if (result != STATUS_UNMAPPABLE_CHARACTER)
                        return result;
                    *pusUnicodeConversionFlags |= 2;
                }
                FindDataOem->dwFileAttributes = FindDataUnicode->dwFileAttributes;
                FindDataOem->nFileSizeHigh = FindDataUnicode->nFileSizeHigh;
                FindDataOem->nFileSizeLow = FindDataUnicode->nFileSizeLow;
                result = S_OK;
            }
        }
    }
    return result;
}

int demLFNFindFirstFile(LPSTR lpFileName, LPWIN32_FIND_DATAA DosFindFileData, USHORT usDateTimeFormat, BYTE RequiredAttributes, BYTE AllowedAttributes, PUSHORT pusUnicodeConversionFlags, PUSHORT pDosHandle, LPSTR lpFullFilename, LPSTR lpShortFilename)
{
    HANDLE hFind;
    HTBLENTRY *pEntry;
    WIN32_FIND_DATAW FindFileData;
    STRING FileName;
    NTSTATUS result;

    RtlInitString(&FileName, lpFileName);
    result = pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &FileName, 0);
    if (NT_SUCCESS(result)) {
        if (RequiredAttributes == ATTR_VOLUME_ID && AllowedAttributes == ATTR_VOLUME_ID)
            return S_OK;
        dempLFNNormalizePath(&NtCurrentTeb()->StaticUnicodeString);
        result = dempLFNFindFirstFile(&hFind, &NtCurrentTeb()->StaticUnicodeString, &FindFileData, RequiredAttributes, AllowedAttributes);
        if (SUCCEEDED(result)) {
            result = dempLFNConvertFindDataUnicodeToOem(
                         DosFindFileData,
                         &FindFileData,
                         usDateTimeFormat,
                         pusUnicodeConversionFlags,
                         lpFullFilename,
                         lpShortFilename);
            if (SUCCEEDED(result)) {
                result = dempLFNAllocateHandleEntry(pDosHandle, &pEntry);
                if (SUCCEEDED(result)) {
                    pEntry->hFind = hFind;
                    pEntry->RequiredAttributes = RequiredAttributes;
                    pEntry->AllowedAttributes = AllowedAttributes;
                    pEntry->PDB = *pusCurrentPDB;
                    return result;
                }
            }
            if (hFind)
                FindClose(hFind);
        }
    }
    return result;
}

VOID demLFNCleanup()
{
    dempLFNCloseSearchHandles();
}

HRESULT demLFNFindNextFile(USHORT DosHandle, LPWIN32_FIND_DATAA DosFindFileData, USHORT usDateTimeFormat, PUSHORT pusUnicodeConversionFlags, LPSTR lpFullFilename, LPCSTR lpShortFilename)
{
    HTBLENTRY *pEntry;
    HRESULT result;
    WIN32_FIND_DATAW FindFileData;

    pEntry = dempLFNGetHandleEntry(DosHandle);
    if ( !pEntry )
        return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
    if (pEntry->hFind == INVALID_HANDLE_VALUE) {
        result = dempLFNFindFirstFile(
                     &pEntry->hFind,
                     &pEntry->FileName,
                     &FindFileData,
                     pEntry->RequiredAttributes,
                     pEntry->AllowedAttributes);
        RtlFreeUnicodeString(&pEntry->FileName);
    } else {
        result = dempLFNFindNextFile(pEntry->hFind, &FindFileData, pEntry->RequiredAttributes, pEntry->AllowedAttributes);
    }
    if (SUCCEEDED(result))
        result = dempLFNConvertFindDataUnicodeToOem(
                     DosFindFileData,
                     &FindFileData,
                     usDateTimeFormat,
                     pusUnicodeConversionFlags,
                     lpFullFilename,
                     lpShortFilename);
    return result;
}

HRESULT demLFNFindClose(USHORT hFind)
{
    HTBLENTRY *pEntry;

    pEntry = dempLFNGetHandleEntry(hFind);
    if ( pEntry ) {
        if (pEntry->hFind != INVALID_HANDLE_VALUE)
            FindClose(pEntry->hFind);
        dempLFNFreeHandleEntry(pEntry);
        return S_OK;
    }
    return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
}

BOOL dempValidateDirectory(PCDS pcds, UCHAR Drive)
{
    DWORD dw;
    CHAR  chDrive;
    static CHAR  pPath[]="?:\\";
    static CHAR  EnvVar[] = "=?:";

    // validate media
    chDrive = Drive + 'A';
    pPath[0] = chDrive;
    dw = GetFileAttributesOem(pPath);
    if (dw == 0xFFFFFFFF || !(dw & FILE_ATTRIBUTE_DIRECTORY))
        return (FALSE);

    // if invalid path, set path to the root
    // reset CDS, and win32 env for win32
    dw = GetFileAttributesOem(pcds->CurDir_Text);
    if (dw == 0xFFFFFFFF || !(dw & FILE_ATTRIBUTE_DIRECTORY)) {
        strcpy(pcds->CurDir_Text, pPath);
        pcds->CurDir_End = 2;
        EnvVar[1] = chDrive;
        SetEnvironmentVariableOem(EnvVar,pPath);
    }

    return (TRUE);
}

PCDS dempGetCDSPtr(UCHAR Drive)
{
    PCDS pCDS = NULL;
    static CHAR RootPathName[] = "?:\\";

    if ( Drive < *(PUCHAR)DosWowData.lpCDSCount ) {
        RootPathName[0] = Drive + 'A';
        if ((Drive != 1) || (DRIVE_REMOVABLE == GetDriveTypeA(RootPathName))) {
            //
            // Drive defined in fixed table
            //
            pCDS = (PCDS) DosWowData.lpCDSFixedTable;
            if ( GetSystemDefaultLangID() == MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT))
                pCDS = (PCDS) ((ULONG)pCDS + (Drive*sizeof(CDS_JPN)));
            else
                pCDS = (PCDS) ((ULONG)pCDS + (Drive*sizeof(CDS)));
        }
    } else if (Drive <= 25) {
        RootPathName[0] = Drive + 'A';
        if ((*(PUCHAR)DosWowData.lpCurDrv == Drive) || GetDriveTypeA(RootPathName) > 1) {
            //
            // Network drive
            //
            pCDS = (PCDS)DosWowData.lpCDSBuffer;
        }
    }
    return pCDS;
}

BOOL dempUpdateCDS(UCHAR Drive, PCDS pcds)
{
    static CHAR  EnvVar[] = "=?:";
    DWORD EnvVarLen;
    BOOL bStatus = TRUE;
    UCHAR FixedCount;
    int i;
    PCDS pcdstemp;

    FixedCount = *(PUCHAR) DosWowData.lpCDSCount;

    if (*(PUCHAR)DosWowData.lpSCS_ToSync) {

        if (GetSystemDefaultLangID() == MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT)) {
            PCDS_JPN pcdstemp_jpn;

            pcdstemp_jpn = (PCDS_JPN) DosWowData.lpCDSFixedTable;
            for (i=0; i < (int)FixedCount; i++, pcdstemp_jpn++)
                pcdstemp_jpn->CurDirJPN_Flags |= CURDIR_TOSYNC;
        } else {
            pcdstemp = (PCDS) DosWowData.lpCDSFixedTable;
            for (i=0; i < (int)FixedCount; i++, pcdstemp++)
                pcdstemp->CurDir_Flags |= CURDIR_TOSYNC;
        }

        // Mark tosync in network drive as well
        pcdstemp = (PCDS)DosWowData.lpCDSBuffer;
        pcdstemp->CurDir_Flags |= CURDIR_TOSYNC;

        *(PUCHAR)DosWowData.lpSCS_ToSync = 0;
    }

    // If CDS needs to be synched or if the requested drive is different
    // then the the drive being used by NetCDS go refresh the CDS.
    if ((pcds->CurDir_Flags & CURDIR_TOSYNC) ||
            ((Drive >= FixedCount) && (pcds->CurDir_Text[0] != (Drive + 'A') &&
                                       pcds->CurDir_Text[0] != (Drive + 'a')))) {
        // validate media
        EnvVar[1] = Drive + 'A';
        if((EnvVarLen = GetEnvironmentVariableOem (EnvVar, (LPSTR)pcds,
                        MAXIMUM_VDM_CURRENT_DIR+3)) == 0) {

            // if its not in env then and drive exist then we have'nt
            // yet touched it.

            pcds->CurDir_Text[0] = EnvVar[1];
            pcds->CurDir_Text[1] = ':';
            pcds->CurDir_Text[2] = '\\';
            pcds->CurDir_Text[3] = 0;
            SetEnvironmentVariableOem ((LPSTR)EnvVar,(LPSTR)pcds);
        }

        if (EnvVarLen > MAXIMUM_VDM_CURRENT_DIR+3) {
            //
            // The current directory on this drive is too long to fit in the
            // cds. That's ok for a win16 app in general, since it won't be
            // using the cds in this case. But just to be more robust, put
            // a valid directory in the cds instead of just truncating it on
            // the off chance that it gets used.
            //
            pcds->CurDir_Text[0] = EnvVar[1];
            pcds->CurDir_Text[1] = ':';
            pcds->CurDir_Text[2] = '\\';
            pcds->CurDir_Text[3] = 0;
        }

        pcds->CurDir_Flags &= 0xFFFF - CURDIR_TOSYNC;
        pcds->CurDir_End = 2;

    }

    if (!bStatus) {

        *(PUCHAR)DosWowData.lpDrvErr = ERROR_INVALID_DRIVE;
    }

    return (bStatus);
}

HRESULT dempGetCurrentDirectoryTDB(UCHAR Drive, LPSTR pszDir)
{
    if ( !DosWowGetTDBDir || !DosWowGetTDBDir(Drive, pszDir + 3) )
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    pszDir[0] = Drive + 'A';
    pszDir[1] = ':';
    pszDir[2] = '\\';
    return S_OK;
}

VOID dempSetCurrentDirectoryTDB(UCHAR Drive, LPSTR pszDir)
{
    if ( DosWowUpdateTDBDir )
        DosWowUpdateTDBDir(Drive, pszDir);
}

HRESULT dempGetCurrentDirectoryCDS(UCHAR Drive, LPSTR pszDir)
{
    PCDS pCDS;
    HRESULT result = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);       // assume error

    if (NULL != (pCDS = dempGetCDSPtr(Drive))) {

        if (dempUpdateCDS(Drive, pCDS)) {
            // for removable media we need to check that media is present.
            // fixed disks, network drives and CDROM drives are fixed drives in
            // DOS.
            if (!(pCDS->CurDir_Flags & CURDIR_NT_FIX)) {
                if(dempValidateDirectory(pCDS, Drive) == FALSE)
                    return (result);         // fail
            }
            strcpy(pszDir, pCDS);
            result = S_OK;
        }
    }
    return (result);
}

HRESULT dempSetCurrentDirectoryCDS(UCHAR Drive, LPSTR pszDir)
{
    PCDS pCDS;

    if (NULL != (pCDS = dempGetCDSPtr(Drive))) {
        if (strlen(pszDir) <= DIRSTRLEN) {
            strcpy(pCDS->CurDir_Text, pszDir);
        } else {
            strncpy(pCDS->CurDir_Text, pszDir, 3);
            pCDS->CurDir_Text[3] = 0;
        }
        return S_OK;
    }
    return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
}

HRESULT demSetCurrentDirectoryLCDS(UCHAR Drive, LPSTR pszDir)
{
    if (pszDir) {
        if (!rgCurDirs[Drive]) {
            rgCurDirs[Drive] = (LPSTR)RtlAllocateHeap(RtlProcessHeap(), 0, MAX_PATH+4);
            if (!rgCurDirs[Drive])
                return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
        }
        strcpy(rgCurDirs[Drive], pszDir);
    } else {
        RtlFreeHeap(RtlProcessHeap(), 0, rgCurDirs[Drive]);
        rgCurDirs[Drive] = NULL;
    }
    return S_OK;
}

HRESULT demGetCurrentDirectoryLCDS(UCHAR Drive, LPSTR pszDir)
{

    if (!rgCurDirs[Drive])
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    if (pszDir)
        strcpy(pszDir, rgCurDirs[Drive]);
    return S_OK;
}

BOOL dempGetCurrentDirectoryWin32(UCHAR Drive, LPSTR pPath)
{
    DWORD dw;
    static CHAR  EnvVar[] = "=?:";

    EnvVar[1] = Drive + 'A';
    if (dw = GetEnvironmentVariableOem(EnvVar, pPath, MAX_PATH)) {
        if (dw > MAX_PATH) return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
        dw = GetFileAttributesOem(pPath);
        if (dw == 0xFFFFFFFF) return HRESULT_FROM_WIN32(GetLastError());
        if (!(dw & FILE_ATTRIBUTE_DIRECTORY)) return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    } else {
        pPath[0] = Drive + 'A';
        pPath[1] = ':';
        pPath[2] = '\\';
        pPath[3] = 0;
        SetEnvironmentVariableOem(EnvVar, pPath);
    }

    return (S_OK);
}

HRESULT dempSetCurrentDirectoryWin32(UCHAR Drive, LPSTR pszDir)
{
    DWORD dw;
    static CHAR  EnvVar[] = "=?:";

    if (*(PUCHAR)DosWowData.lpCurDrv == Drive) {
        if ( !SetCurrentDirectoryOem(pszDir) )
            return HRESULT_FROM_WIN32(GetLastError());
    } else {
        dw = GetFileAttributesOem(pszDir);
        if (dw == 0xFFFFFFFF || !(dw & FILE_ATTRIBUTE_DIRECTORY))
            return STATUS_INVALID_HANDLE;
    }
    EnvVar[1] = Drive + 'A';
    if (!SetEnvironmentVariableOem(EnvVar, pszDir))
        return HRESULT_FROM_WIN32(GetLastError());
    return S_OK;
}

HRESULT demGetCurrentDirectoryLong(UCHAR Drive, LPSTR lpResult, ULONG Type)
{
    HRESULT result;
    CHAR lpDirectory[MAX_PATH];

    if ( Type & 0x10000 || !SUCCEEDED(result = dempGetCurrentDirectoryTDB(Drive, lpDirectory)) ) {
        result = demGetCurrentDirectoryLCDS(Drive, lpDirectory);
        if (!SUCCEEDED(result)) {
            if ( !(Type & 0x20000) )
                result = dempGetCurrentDirectoryCDS(Drive, lpDirectory);
            if (!SUCCEEDED(result)) {
                result = dempGetCurrentDirectoryWin32(Drive, lpDirectory);
                if (!SUCCEEDED(result))
                    return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
            }
        }
    }
    switch ((USHORT)Type) {
    case 3:
        if (strlen(lpDirectory) > DIRSTRLEN)
            return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    case 1:
        strcpy(lpResult, lpDirectory);
        return result;
    case 2:
        return demLFNGetPathName(lpDirectory, lpResult, 2, FALSE);
    default:
        return result;
    }
}

HRESULT demSetCurrentDirectoryLong(UCHAR Drive, LPSTR lpPath, ULONG Type)
{
    HRESULT result;
    CHAR szPath[260];

    if (SUCCEEDED(result = demLFNGetPathName(lpPath, szPath, 1, FALSE)) &&
            SUCCEEDED(result = dempSetCurrentDirectoryWin32(Drive, szPath)) &&
            SUCCEEDED(result = demSetCurrentDirectoryLCDS(Drive, szPath))) {
        if ( Type & 0x20000 || SUCCEEDED(result = dempSetCurrentDirectoryCDS(Drive, szPath)) ) {
            if ( !(Type & 0x10000) )
                dempSetCurrentDirectoryTDB(Drive, szPath);
        }
    }
    return result;
}

HRESULT dempLFNSetCurrentDirectory(UNICODE_STRING *Path, PUINT pDriveNum)
{
    WCHAR *pszDir;
    UCHAR DriveNum;
    HRESULT result;
    WCHAR wszPath[MAX_PATH];
    char szPath[MAX_PATH];
    OEM_STRING strPath;
    UNICODE_STRING ucsPath;

    pszDir = Path->Buffer && *Path->Buffer?Path->Buffer:L"\\";
    switch (RtlDetermineDosPathNameType_U(pszDir)) {
    case RtlPathTypeDriveAbsolute:
        CharUpperBuffW(pszDir, 1);
        DriveNum = *pszDir - 'A';
        break;
    case RtlPathTypeDriveRelative:
    case RtlPathTypeRooted:
    case RtlPathTypeRelative:
        DriveNum = *((BYTE*)DosWowData.lpCurDrv);
        break;
    default:
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    ucsPath.Buffer = wszPath;
    ucsPath.MaximumLength = sizeof(wszPath);
    ucsPath.Length = RtlGetFullPathName_U(pszDir, sizeof(wszPath), wszPath, FALSE);
    if (!ucsPath.Length)
        return HRESULT_FROM_WIN32(GetLastError());
    if ( ucsPath.MaximumLength <= ucsPath.Length )
        return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    strPath.Buffer = szPath;
    strPath.MaximumLength = sizeof(szPath);
    result = pfnUnicodeStringToDestString(&strPath, &ucsPath, 0, 0);
    if (NT_SUCCESS(result)) {
        result = demSetCurrentDirectoryLong(DriveNum, strPath.Buffer, 0);
        if ( pDriveNum )
            *pDriveNum = DriveNum;
    }
    return result;
}

HRESULT demSetCurrentDirectoryGetDrive(LPSTR lpDirectoryName, PUINT pDrive)
{
    STRING DestinationString;

    RtlInitString(&DestinationString, lpDirectoryName);
    pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, FALSE);
    return dempLFNSetCurrentDirectory(&NtCurrentTeb()->StaticUnicodeString, pDrive);
}

HRESULT demLFNDirectoryControl(USHORT SubFunction, LPSTR pszDir)
{
    NTSTATUS Status;
    STRING DestinationString;

    RtlInitString(&DestinationString, pszDir);
    Status = pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, 0);
    if (NT_SUCCESS(Status)) {
        switch (SubFunction) {
        case 0x39u:
            if (!CreateDirectoryW(NtCurrentTeb()->StaticUnicodeString.Buffer, NULL)) {
                Status = HRESULT_FROM_WIN32(GetLastError());
                if (Status == HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE) || Status == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS))
                    Status = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
            }
            break;
        case 0x3Au:
            if ( !RemoveDirectoryW(NtCurrentTeb()->StaticUnicodeString.Buffer) )
                Status = HRESULT_FROM_WIN32(GetLastError());
            break;
        case 0x3Bu:
            Status = dempLFNSetCurrentDirectory(&NtCurrentTeb()->StaticUnicodeString, NULL);
            break;
        }
    } else if (Status == STATUS_BUFFER_OVERFLOW) {
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }
    return Status;
}

HRESULT demLFNGetCurrentDirectory(UCHAR Drive, LPSTR pszDir)
{
    HRESULT result;
    char pPath[260];

    result = demGetCurrentDirectoryLong(Drive?Drive-(UCHAR)1:DosWowData.lpCurDrv, pPath, 2);
    if (SUCCEEDED(result))
        strcpy(pszDir, &pPath[3]);
    return result;
}

HRESULT demLFNMoveFile(LPSTR lpszPathSrc, LPSTR lpszPathDest)
{
    HRESULT result;
    UNICODE_STRING Path;
    STRING DestinationString;
    UNICODE_STRING UnicodeString;

    RtlInitString(&DestinationString, lpszPathSrc);
    result = pfnSrcStringToUnicodeString(&UnicodeString, &DestinationString, 1);
    if (NT_SUCCESS(result)) {
        dempLFNNormalizePath(&UnicodeString);
        RtlInitString(&DestinationString, lpszPathDest);
        result = pfnSrcStringToUnicodeString(&Path, &DestinationString, TRUE);
        if (NT_SUCCESS(result)) {
            dempLFNNormalizePath(&Path);
            if (!MoveFileW(UnicodeString.Buffer, Path.Buffer))
                result = HRESULT_FROM_WIN32(GetLastError());
            RtlFreeUnicodeString(&Path);
        }
        RtlFreeUnicodeString(&UnicodeString);
    }
    return result;
}

HRESULT demLFNGetVolumeInformation(LPSTR lpRootPathName, LFNVOLINFO *VolumeInfo)
{
    DWORD FileSystemFlags;

    // From oemuni\file.c
    if ( GetVolumeInformationOem(
                lpRootPathName,
                0,
                0,
                0,
                &VolumeInfo->nMaxComponentLen,
                &FileSystemFlags,
                VolumeInfo->lpFileSystemNameBuffer,
                VolumeInfo->nFileSystemNameSize) ) {
        VolumeInfo->FileSystemFlags = (FileSystemFlags & 0x8007) | 0x4000;
        VolumeInfo->nMaxPathLen = VolumeInfo->nMaxComponentLen + 5;
        return S_OK;
    }
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT demLFNFileTimeControl(WORD SubFunction, LPFILETIME lpFileTime, DOSTIME *DosTime)
{
    TIME_FIELDS TimeFields;
    LARGE_INTEGER Time;
    FILETIME LocalFileTime;

    if ( (SubFunction & 0xF)  == 0) {
        if ( SubFunction & 0x10 ) {
            LocalFileTime.dwLowDateTime = lpFileTime->dwLowDateTime;
            LocalFileTime.dwHighDateTime = lpFileTime->dwHighDateTime;
        } else if ( !FileTimeToLocalFileTime(lpFileTime, &LocalFileTime) ) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        Time.QuadPart = ((ULARGE_INTEGER*)&LocalFileTime)->QuadPart + 19999999;
        RtlTimeToTimeFields(&Time, &TimeFields);
        if ( TimeFields.Year >= 1980 && TimeFields.Year <= 2107 ) {
            DosTime->FatDate = ((TimeFields.Year - 60) << 9) | (TimeFields.Month << 5) | TimeFields.Day;
            DosTime->FatTime = (TimeFields.Hour << 11) | (TimeFields.Minute << 5) | (TimeFields.Second >> 1);
            DosTime->FatHundredths = (TimeFields.Milliseconds + 1000 * (TimeFields.Second & 1)) / 10;
            return S_OK;
        }
        DosTime->FatDate = 1 | (1 << 5);  // 1.1.1980
        DosTime->FatTime = 0;
        DosTime->FatHundredths = 0;
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    } else if ( (SubFunction & 0xF) == 1 ) {
        TimeFields.Year = (DosTime->FatDate >> 9) + 1980;
        TimeFields.Day = DosTime->FatDate & 0x1F;
        TimeFields.Month = (DosTime->FatDate >> 5) & 0xF;
        TimeFields.Hour = DosTime->FatTime >> 11;
        TimeFields.Second = (DosTime->FatTime & 0x1F) << 1;
        TimeFields.Minute = (DosTime->FatTime >> 5) & 0x3F;
        TimeFields.Second += (10 * DosTime->FatHundredths) / 1000;
        TimeFields.Milliseconds = (10 * DosTime->FatHundredths) % 1000;
        if ( RtlTimeFieldsToTime(&TimeFields, &Time) ) {
            LocalFileTime = *((FILETIME*)&Time.QuadPart);
            if ( LocalFileTimeToFileTime(&LocalFileTime, lpFileTime) )
                return S_OK;
            return HRESULT_FROM_WIN32(GetLastError());
        }
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    }
    return HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
}

HRESULT dempLFNSetFileTime(DWORD Action, UNICODE_STRING *FileName, DOSTIME *DosTime)
{
    LPFILETIME lpFileTime;
    HRESULT result;
    PWSTR Buffer;
    FILE_BASIC_INFORMATION FileInformation;
    OBJECT_ATTRIBUTES ObjectAttributes;
    RTL_RELATIVE_NAME_U DirectoryInfo;
    IO_STATUS_BLOCK IoStatusBlock;
    UNICODE_STRING NtPathName;

    ZeroMemory(&FileInformation, sizeof(FileInformation));
    switch (Action) {
    case 3:
        lpFileTime = (LPFILETIME)&FileInformation.LastWriteTime;
        break;
    case 5:
        lpFileTime = (LPFILETIME)&FileInformation.LastAccessTime;
        break;
    case 7:
        lpFileTime = (LPFILETIME)&FileInformation.CreationTime;
        break;
    default:
        lpFileTime = (LPFILETIME)Action;
        break;
    }
    result = demLFNFileTimeControl(1, lpFileTime, DosTime);
    if (SUCCEEDED(result)) {
        if ( RtlDosPathNameToNtPathName_U(FileName->Buffer, &NtPathName, NULL, &DirectoryInfo) ) {
            HANDLE hFile;

            Buffer = NtPathName.Buffer;
            if (DirectoryInfo.RelativeName.Length)
                NtPathName = DirectoryInfo.RelativeName;
            else
                DirectoryInfo.ContainingDirectory = NULL;
            InitializeObjectAttributes(&ObjectAttributes, &NtPathName, OBJ_CASE_INSENSITIVE, DirectoryInfo.ContainingDirectory, NULL);
            result = NtOpenFile(
                         &hFile,
                         FILE_WRITE_ATTRIBUTES | SYNCHRONIZE,
                         &ObjectAttributes,
                         &IoStatusBlock,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         FILE_SYNCHRONOUS_IO_NONALERT  | FILE_OPEN_FOR_BACKUP_INTENT);
            RtlFreeHeap(RtlProcessHeap(), 0, Buffer);
            if (NT_SUCCESS(result)) {
                result = NtSetInformationFile(
                             hFile,
                             &IoStatusBlock,
                             &FileInformation,
                             sizeof(FileInformation),
                             FileBasicInformation);
                NtClose(hFile);
            }
        } else {
            return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
        }
    }
    return result;
}

BOOL dempUseUTCTimeByHandle(HANDLE FileHandle)
{
    struct _IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_DEVICE_INFORMATION FsInformation;

    if (NT_SUCCESS(NtQueryVolumeInformationFile(FileHandle, &IoStatusBlock, &FsInformation, sizeof(FsInformation), FileFsDeviceInformation)))
        return (FsInformation.Characteristics & FILE_REMOVABLE_MEDIA) && (FsInformation.DeviceType == FILE_DEVICE_CD_ROM || FsInformation.DeviceType == FILE_DEVICE_CD_ROM_FILE_SYSTEM);
    return FALSE;
}

BOOL dempUseUTCTimeByName(UNICODE_STRING *FileName)
{
    USHORT Length;
    WCHAR RootPathName[260];
    UNICODE_STRING RootPath;

    dempStringInitZeroUnicode(&RootPath, RootPathName, 260);
    Length = RtlGetFullPathName_U(FileName->Buffer, RootPath.MaximumLength, RootPath.Buffer, FALSE);
    if (Length == 0 || RootPath.MaximumLength <= Length)
        return FALSE;
    if (RtlDetermineDosPathNameType_U(RootPath.Buffer) == RtlPathTypeDriveAbsolute) {
        RootPathName[3] = 0;
        return GetDriveTypeW(RootPathName) == DRIVE_CDROM;
    }
    return FALSE;
}

HRESULT dempGetFileTimeByHandle(WORD SubFunction, HANDLE hFile, DOSTIME *DosTime)
{
    HRESULT result;
    FILETIME FileTime;

    if ( !GetFileTime(hFile, SubFunction==6?&FileTime:NULL, SubFunction==4?&FileTime:NULL, SubFunction==0?&FileTime:NULL) )
        return HRESULT_FROM_WIN32(GetLastError());
    result = demLFNFileTimeControl(dempUseUTCTimeByHandle(hFile)?0x10:0, &FileTime, DosTime);
    if (result == HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && SubFunction == 0)
        return S_OK;
    return result;
}

ULONG demGetFileTimeByHandle_WOW(HANDLE hFile)
{
    HRESULT result;
    FILETIME LastWriteTime;
    DOSTIME DosTime;

    if ( GetFileTime(hFile, 0, 0, &LastWriteTime) ) {
        result = demLFNFileTimeControl(dempUseUTCTimeByHandle(hFile)?0x10:0, &LastWriteTime, &DosTime);
        if (result == HRESULT_FROM_WIN32(ERROR_INVALID_DATA))
            result = S_OK;
    } else {
        result = HRESULT_FROM_WIN32(GetLastError());
    }
    if (SUCCEEDED(result))
        return GETULONG(DosTime.FatDate, DosTime.FatTime);
    return 0xFFFF;
}

HRESULT dempSetFileTimeByHandle(BYTE SubFunction, HANDLE hFile, DOSTIME *DosTime)
{
    HRESULT result;
    BOOL bRet;
    FILETIME FileTime;
    LPFILETIME lpLastAccessTime = NULL, lpLastWriteTime = NULL, lpCreationTime = NULL;

    switch ( SubFunction ) {
    case 1:
        lpLastWriteTime = &FileTime;
        DosTime->FatHundredths = 0;
        break;
    case 5:
        lpLastAccessTime = &FileTime;
        DosTime->FatTime = 0;
        DosTime->FatHundredths = 0;
        break;
    case 7:
        lpCreationTime = &FileTime;
        break;
    }
    result = demLFNFileTimeControl(1, &FileTime, DosTime);
    if (SUCCEEDED(result) && !SetFileTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime))
        return HRESULT_FROM_WIN32(GetLastError());
    return result;
}

HRESULT demLFNOpenFile(PCSZ lpFileName, USHORT regbx, USHORT regcx, USHORT regdx, USHORT regdi, PUSHORT pregAX, PUSHORT pregCX)
{
    NTSTATUS result;
    DWORD dwCreationDisposition;
    DWORD dwDesiredAccess;
    HANDLE hFile;
    WORD hDosHandle;
    SECURITY_ATTRIBUTES SecurityAttributes;
    STRING FileName;
    int action;
    int dwFlagsAndAttributes;
    DOSSFT *ppSFT;
    DWORD dwShareMode;

#define DOS_OPEN_EXISTING		1
#define DOS_TRUNCATE_EXISTING	2
#define DOS_CREATE_NEW			16

    dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    action = 1;
    RtlInitString(&FileName, lpFileName);
    result = pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &FileName, FALSE);
    if (!SUCCEEDED(result)) return result;
    if ( regbx & 8 )
        return HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
    if (regdx & (DOS_CREATE_NEW - 1)) {
        if ((regdx & (DOS_CREATE_NEW - 1)) == DOS_OPEN_EXISTING) {
            if (regdx & DOS_CREATE_NEW) {
                dwCreationDisposition = OPEN_ALWAYS;
            } else {
                dwCreationDisposition = OPEN_EXISTING;
            }
        } else if ((regdx & (DOS_CREATE_NEW - 1)) == DOS_TRUNCATE_EXISTING) {
            dwCreationDisposition = TRUNCATE_EXISTING; // (~regdx & DOS_CREATE_NEW | 0x40) >> 4;
        } else {
            return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
        }
    } else if (regdx == DOS_CREATE_NEW) {
        dwCreationDisposition = CREATE_NEW;
    } else {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }
    if (regbx & 0x70) {
        switch (regbx & 0x70) {
        case 0x10:
            dwShareMode = 0;
            break;
        case 0x20:
            dwShareMode = FILE_SHARE_READ;
            break;
        case 0x30:
            dwShareMode = FILE_SHARE_WRITE;
            break;
        case 0x40:
            dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
            break;
        default:
            return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
        }
    } else {
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    }

    switch (regbx & 0xF) {
    case 0:
    case 4:
        dwDesiredAccess = GENERIC_READ;
        break;
    case 1:
        dwDesiredAccess = GENERIC_WRITE;
        break;
    case 2:
        dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }

    if (regbx & 0x9800)
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    if (regbx & 0x100)
        dwFlagsAndAttributes = FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL;
    if (regbx & 0x4000)
        dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
    dempLFNNormalizePath(&NtCurrentTeb()->StaticUnicodeString);
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.nLength = 12;
    SecurityAttributes.bInheritHandle = TRUE;
    hFile = CreateFileW(
                NtCurrentTeb()->StaticUnicodeString.Buffer,
                dwDesiredAccess,
                dwShareMode,
                &SecurityAttributes,
                dwCreationDisposition,
                dwFlagsAndAttributes | regcx & 0x27,
                0);
    if (hFile == INVALID_HANDLE_VALUE)
        return HRESULT_FROM_WIN32(GetLastError());
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (regdx == 0x12) {
            if (GetFileType(hFile) == FILE_TYPE_DISK) {
                if (!SetEndOfFile(hFile)) {
                    CloseHandle(hFile);
                    return HRESULT_FROM_WIN32(GetLastError());
                } else
                    action = 3;
            } else
                action = 2;
        }
    } else if (regdx & 0x10) {
        action = 2;
    }
    hDosHandle = VDDAllocateDosHandle(0, &ppSFT, 0);
    if ((hDosHandle & 0x8000) != 0) {
        CloseHandle(hFile);
        return HRESULT_FROM_WIN32(-hDosHandle);
    }
    ppSFT->SFT_Mode = regbx & 0x7F;
    ppSFT->SFT_Attr = 0;
    ppSFT->SFT_Flags = (regbx & 0x80) << 5;
    ppSFT->SFT_Devptr = (ULONG)-1;
    ppSFT->SFT_NTHandle = (ULONG)hFile;
    *pregCX = action;
    *pregAX = hDosHandle;
    return S_OK;
}

HRESULT demLFNDeleteFile(LPSTR pszFile, USHORT RequiredAttributes, USHORT AllowedAttributes, BOOL bAllowWildcards)
{
    HRESULT result;
    WIN32_FIND_DATAW FindFileData;
    STRING DestinationString;
    UNICODE_STRING Source;

    RtlInitString(&DestinationString, pszFile);
    result = pfnSrcStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, FALSE);
    if (NT_SUCCESS(result)) {
        dempLFNNormalizePath(&NtCurrentTeb()->StaticUnicodeString);
        if (bAllowWildcards) {
            BOOL bDeleted = FALSE;
            HANDLE hFind;
            WORD Pos;

            result = dempLFNFindFirstFile(&hFind, &NtCurrentTeb()->StaticUnicodeString, &FindFileData, RequiredAttributes, AllowedAttributes);
            if (!SUCCEEDED(result))
                return result;
            Pos = dempStringFindLastChar(&NtCurrentTeb()->StaticUnicodeString, L'\\', FALSE) + 1;
            do {
                RtlInitUnicodeString(&Source, FindFileData.cFileName);
                NtCurrentTeb()->StaticUnicodeString.Length = Pos;
                result = RtlAppendUnicodeStringToString(&NtCurrentTeb()->StaticUnicodeString, &Source);
                if (!NT_SUCCESS(result))
                    break;
                if (!DeleteFileW(NtCurrentTeb()->StaticUnicodeString.Buffer)) {
                    result = HRESULT_FROM_WIN32(GetLastError());
                    break;
                }
                bDeleted = TRUE;
            } while (SUCCEEDED(result = dempLFNFindNextFile(hFind, &FindFileData, RequiredAttributes, AllowedAttributes)));
            FindClose(hFind);
            if (bDeleted)
                result = S_OK;
        } else {
            if ( dempStringFindLastChar(&NtCurrentTeb()->StaticUnicodeString, '*', FALSE) >= 0 || dempStringFindLastChar(&NtCurrentTeb()->StaticUnicodeString, '?', FALSE) >= 0 )
                return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
            if ( !DeleteFileW(NtCurrentTeb()->StaticUnicodeString.Buffer) )
                return HRESULT_FROM_WIN32(GetLastError());
            return S_OK;
        }
    }
    return result;
}

HRESULT demLFNGetFileInformationByHandle(SHORT hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation)
{
    HANDLE ntHandle;

    ntHandle = VDDRetrieveNtHandle(0, hFile, NULL, NULL);
    if (ntHandle) {
        if (GetFileInformationByHandle(ntHandle, lpFileInformation))
            return S_OK;
        return HRESULT_FROM_WIN32(GetLastError());
    }
    return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
}

HRESULT demLFNGenerateShortFileName(LPSTR lpShortFileName, LPSTR lpLongFileName, WORD Format, WORD Charset)
{
    NTSTATUS Status;
    int Position;
    GENERATE_NAME_CONTEXT Context;
    char Buffer[26];
    UNICODE_STRING Name8dot3;
    STRING DestinationString;
    WORD Charseta;

    switch (Charset & 0xF) {
    case 0:
        RtlInitAnsiString(&DestinationString, lpLongFileName);
        Status = RtlAnsiStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, 0);
        if (!NT_SUCCESS(Status)) return Status;
        break;
    case 1:
        RtlInitString(&DestinationString, lpLongFileName);
        Status = RtlOemStringToUnicodeString(&NtCurrentTeb()->StaticUnicodeString, &DestinationString, 0);
        if (!NT_SUCCESS(Status)) return Status;
        break;
    case 2:
        RtlInitUnicodeString(&NtCurrentTeb()->StaticUnicodeString, lpLongFileName);
        break;
    default:
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }
    Charseta = Charset & 0xF0;
    dempStringInitZeroUnicode(&Name8dot3, Charseta != 0x20?Buffer:lpShortFileName, 26);
    ZeroMemory(&Context,  sizeof(Context));
    RtlGenerate8dot3Name(&NtCurrentTeb()->StaticUnicodeString, 0, &Context, &Name8dot3);
    Position = dempStringFindLastChar(&Name8dot3, '~', 0);
    if (Position >= 0)
        dempStringDeleteCharsUnicode(&Name8dot3, Position, 4);
    if (Format == 0) {
        Position = dempStringFindLastChar(&Name8dot3, '.', 1);
        if ( Position >= 0 )
            dempStringDeleteCharsUnicode(&Name8dot3, Position, 2);
    }
    if ( Charseta == 0x20 )
        return S_OK;
    DestinationString.Buffer = lpShortFileName;
    DestinationString.Length = 0;
    DestinationString.MaximumLength = 26;
    if (Charseta) {
        if ( Charseta != 0x10 )
            return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
        return RtlUnicodeStringToOemString(&DestinationString, &Name8dot3, 0);
    }
    return RtlUnicodeStringToAnsiString(&DestinationString, &Name8dot3, 0);
}

ULONG dempWOWLFNReturn(NTSTATUS Status)
{
    ULONG result; // eax

    result = RtlNtStatusToDosError(Status);
    switch (HRESULT_CODE(result)) {
    case ERROR_WRITE_PROTECT:
    case ERROR_BAD_UNIT:
    case ERROR_NOT_READY:
    case ERROR_BAD_COMMAND:
    case ERROR_CRC:
    case ERROR_BAD_LENGTH:
    case ERROR_SEEK:
    case ERROR_NOT_DOS_DISK:
    case ERROR_SECTOR_NOT_FOUND:
    case ERROR_OUT_OF_PAPER:
    case ERROR_WRITE_FAULT:
    case ERROR_READ_FAULT:
    case ERROR_GEN_FAILURE:
    case ERROR_WRONG_DISK:
        result |= 0xFFFF0000;
        break;
    }
    return result;
}

VOID demInitCDSPtr()
{
    if (!gfInitCDSPtr) {
        gfInitCDSPtr = TRUE;
        DosWowData.lpCDSFixedTable = (DWORD)GetRModeVDMPointer(*((DWORD*)DosWowData.lpCDSFixedTable));
    }
}

VOID demSetDosVarLocation()
{
    DOSWOWDATA *pData;

    pData = (DOSWOWDATA *)GetVDMAddr(getDS(), getSI());
    DosWowData.lpCDSCount      = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpCDSCount));
    DosWowData.lpCDSFixedTable = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpCDSFixedTable));
    DosWowData.lpCDSBuffer     = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpCDSBuffer));
    DosWowData.lpCurDrv        = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpCurDrv));
    DosWowData.lpCurPDB        = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpCurPDB));
    DosWowData.lpDrvErr        = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpDrvErr));
    DosWowData.lpExterrLocus   = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpExterrLocus));
    DosWowData.lpSCS_ToSync    = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpSCS_ToSync));
    DosWowData.lpSftAddr       = (DWORD)GetRModeVDMPointer(FETCHDWORD(pData->lpSftAddr));
}

VOID dempCheckTempEnvironmentVariables()
{
    DWORD Length;
    PSZ pEnv;
    CHAR Name[MAX_PATH+8];
    CHAR *pszTmps[2];
    ULONG iPos, iTemp;

    pszTmps[0] = "TEMP";
    pszTmps[1] = "TMP";
    cmdCheckTempInit();
    for (iTemp = 0; iTemp<sizeof(pszTmps)/sizeof(pszTmps[0]); iTemp++) {
        strcpy(Name, pszTmps[iTemp]);
        iPos = strlen(Name);
        Length = GetEnvironmentVariableA(Name, &Name[iPos+1], MAX_PATH);
        if (Length && Length < MAX_PATH) {
            Name[iPos] = '=';
            if (pEnv = cmdCheckTemp(Name)) {
                pEnv[iPos] = 0;
                DBG_LOG(("%s: substituted for %s\r\n", pEnv, &pEnv[iPos + 1]));
                SetEnvironmentVariableA(pEnv, &pEnv[iPos + 1]);
            }
        }
    }
}

VOID demWOWLFNInit(PWOWLFNINIT pLFNInit)
{
    DosWowUpdateTDBDir = pLFNInit->pDosWowUpdateTDBDir;
    DosWowGetTDBDir = pLFNInit->pDosWowGetTDBDir;
    DosWowDoDirectHDPopup = pLFNInit->pDosWowDoDirectHDPopup;
    dempCheckTempEnvironmentVariables();
    demInitCDSPtr();
}

DWORD demWOWLFNAllocateSearchHandle(HANDLE hFind)
{
    USHORT DosHandle;
    HTBLENTRY *pEntry;

    if (!SUCCEEDED(dempLFNAllocateHandleEntry(&DosHandle, &pEntry)))
        return (DWORD)-1; // Error
    pEntry->hFind = hFind;
    pEntry->RequiredAttributes = pEntry->AllowedAttributes = 0;
    pEntry->PDB = *pusCurrentPDB;
    return DosHandle;
}

HANDLE demWOWLFNGetSearchHandle(USHORT DosHandle)
{
    HTBLENTRY *pEntry = dempLFNGetHandleEntry(DosHandle);

    return pEntry?pEntry->hFind:INVALID_HANDLE_VALUE;
}


BOOL demWOWLFNCloseSearchHandle(USHORT DosHandle)
{
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HTBLENTRY *pEntry;

    if (pEntry = dempLFNGetHandleEntry(DosHandle)) {
        hFind = pEntry->hFind;
        dempLFNFreeHandleEntry(pEntry);
    }
    return FindClose(hFind);
}

HRESULT dempLFNGetFileTime(DWORD Action, UNICODE_STRING *FileName, DOSTIME *DosTime)
{
    HRESULT result;
    PWSTR Buffer;
    LPFILETIME lpFileTime;
    FILE_NETWORK_OPEN_INFORMATION FileInformation;
    OBJECT_ATTRIBUTES ObjectAttributes;
    RTL_RELATIVE_NAME_U DirectoryInfo;
    UNICODE_STRING NtPathName;

    if ( !RtlDosPathNameToNtPathName_U(FileName->Buffer, &NtPathName, 0, &DirectoryInfo) )
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    Buffer = NtPathName.Buffer;

    if ( DirectoryInfo.RelativeName.Length )
        NtPathName = DirectoryInfo.RelativeName;
    else
        DirectoryInfo.ContainingDirectory = NULL;

    InitializeObjectAttributes(&ObjectAttributes, &NtPathName, OBJ_CASE_INSENSITIVE, DirectoryInfo.ContainingDirectory, NULL);
    result = NtQueryFullAttributesFile(&ObjectAttributes, &FileInformation);
    RtlFreeHeap(RtlProcessHeap(), 0, Buffer);
    if (!NT_SUCCESS(result))
        return result;
    switch ( Action ) {
    case 4:
        lpFileTime = (struct _FILETIME *)&FileInformation.LastWriteTime;
        break;
    case 6:
        lpFileTime = (struct _FILETIME *)&FileInformation.LastAccessTime;
        break;
    case 8:
        lpFileTime = (struct _FILETIME *)&FileInformation.CreationTime;
        break;
    default:
        lpFileTime = (struct _FILETIME *)FileName;
        break;
    }
    result = demLFNFileTimeControl(dempUseUTCTimeByName(FileName)?0x10:0, lpFileTime, DosTime);
    if (result == HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && Action == 4)
        return S_OK;
    return result;
}

HRESULT demLFNGetSetFileAttributes(DWORD Action, PCSZ SourceString, DOSATTR *DosAttr)
{
    UNICODE_STRING *FileName;
    HRESULT result;
    DWORD dwAttrib;
    STRING DestinationString;

    FileName = &NtCurrentTeb()->StaticUnicodeString;
    RtlInitString(&DestinationString, SourceString);
    result = pfnSrcStringToUnicodeString(FileName, &DestinationString, 0);
    if (NT_SUCCESS(result)) {
        dempLFNNormalizePath(FileName);
        switch ( Action ) {
        case 0u:
            dwAttrib = GetFileAttributesW(FileName->Buffer);
            if (dwAttrib == (DWORD)-1)
                return HRESULT_FROM_WIN32(GetLastError());
            DosAttr->Attributes = dwAttrib & 0x3F;
            break;
        case 1u:
            if (DosAttr->Attributes & 0xFFFFFFD0)
                return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
            if ( !SetFileAttributesW(FileName->Buffer, DosAttr->Attributes & 0x27) )
                return HRESULT_FROM_WIN32(GetLastError());
            break;
        case 2u:
            dwAttrib = GetCompressedFileSizeW(FileName->Buffer, NULL);
            if (dwAttrib == INVALID_FILE_SIZE)
                return HRESULT_FROM_WIN32(GetLastError());
            else
                DosAttr->FileSize = dwAttrib;
            break;
        case 3u:
        case 5u:
        case 7u:
            return dempLFNSetFileTime(Action, FileName, &DosAttr->t);
        case 4u:
        case 6u:
        case 8u:
            return dempLFNGetFileTime(Action, FileName, &DosAttr->t);
        default:
            return HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
        }
    }
    return result;
}


HRESULT demLFNDispatch(PVOID pUserFrame, BOOL ProtectedMode, PUSHORT pResult)
{
    HRESULT result;
    DOSENV *DosEnv;
    USHORT UnicodeConversionFlags, DosHandle;
    PWIN32_FIND_DATAA lpFindDataDest;
    LPFILETIME lpFileTime;
    WIN32_FIND_DATA DOSFindData= {0};
    BY_HANDLE_FILE_INFORMATION FileInformation;
    LFNVOLINFO FileSystemFlags;
    DOSATTR DosAttr;

    result = S_OK;
    if ( !gfInitCDSPtr )
        demInitCDSPtr();
    DosEnv = pUserFrame?(DOSENV*)pUserFrame:dempGetDosUserEnvironment();
    *pResult = DosEnv->ax;
    if (HIBYTE(DosEnv->ax) == 0x71) {
        DBG_LOG(("LFN Function: 0x%x \r\n", DosEnv->ax));
        switch (LOBYTE(DosEnv->ax)) {
        case 0x39:
        case 0x3A:
        case 0x3B:
            result = demLFNDirectoryControl(
                         LOBYTE(DosEnv->ax),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode));
            break;
        case 0x41:
            result = demLFNDeleteFile(
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode),
                         HIBYTE(DosEnv->cx),
                         LOBYTE(DosEnv->cx),
                         DosEnv->si);
            break;
        case 0x43:
            memset (&DosAttr, 0, sizeof(DOSATTR));
            switch (LOBYTE(DosEnv->bx)) {
            case 1:
                DosAttr.Attributes = DosEnv->cx;
                break;
            case 3:
            case 5:
                DosAttr.t.FatDate = DosEnv->di;
                DosAttr.t.FatTime = DosEnv->cx;
                break;
            case 7:
                DosAttr.t.FatHundredths = DosEnv->si;
                break;
            }
            result = demLFNGetSetFileAttributes(
                         LOBYTE(DosEnv->bx),
                         (PCSZ)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode),
                         &DosAttr);
            if (!SUCCEEDED(result)) break;
            switch (LOBYTE(DosEnv->bx)) {
            case 0:
                *pResult = DosAttr.Attributes;
                DosEnv->cx = DosAttr.Attributes;
                break;
            case 2:
                DosEnv->dx = DosAttr.t.FatTime;
                *pResult = DosAttr.t.FatDate;
                break;
            case 4:
            case 6:
            case 8:
                DosEnv->si = DosAttr.t.FatHundredths;
                DosEnv->cx = DosAttr.t.FatTime;
                DosEnv->di = DosAttr.t.FatDate;
                break;
            }
            break;
        case 0x47:
            result = demLFNGetCurrentDirectory(
                         LOBYTE(DosEnv->dx),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->si), 0, ProtectedMode));
            break;
        case 0x4E:
            lpFindDataDest = Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode);
            ASSERT(NULL != lpFindDataDest);
            result = demLFNFindFirstFile(
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode),
                         &DOSFindData,
                         DosEnv->si,
                         HIBYTE(DosEnv->cx),
                         LOBYTE(DosEnv->cx),
                         &UnicodeConversionFlags,
                         &DosHandle,
                         lpFindDataDest->cFileName,
                         lpFindDataDest->cAlternateFileName);
            if (SUCCEEDED(result)) {
                memmove(lpFindDataDest, &DOSFindData, 44);
                *pResult = DosHandle;
                DosEnv->cx = UnicodeConversionFlags;
            }
            break;
        case 0x4F:
            lpFindDataDest = Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode);
            ASSERT(NULL != lpFindDataDest);
            result = demLFNFindNextFile(
                         DosEnv->bx,
                         &DOSFindData,
                         DosEnv->si,
                         &UnicodeConversionFlags,
                         &lpFindDataDest->cFileName,
                         &lpFindDataDest->cAlternateFileName);
            if (SUCCEEDED(result)) {
                memmove(lpFindDataDest, &DOSFindData, 44);
                DosEnv->cx = UnicodeConversionFlags;
            }
            break;
        case 0x56:
            result = demLFNMoveFile(
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode));
            break;
        case 0x60:
            result = demLFNGetPathName(
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->si), 0, ProtectedMode),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode),
                         LOBYTE(DosEnv->cx),
                         ~(HIBYTE(DosEnv->cx) >> 7) & 1);
            if (SUCCEEDED(result))
                *pResult = 0;
            break;
        case 0x6C:
            result = demLFNOpenFile(
                         Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->si), 0, ProtectedMode),
                         DosEnv->bx,
                         DosEnv->cx,
                         DosEnv->dx,
                         DosEnv->di,
                         &DosHandle,
                         &UnicodeConversionFlags);
            if (!SUCCEEDED(result))
                break;
            *pResult = DosHandle;
            DosEnv->cx = UnicodeConversionFlags;	// = Action taken
            break;
        case 0xA0:
            FileSystemFlags.nFileSystemNameSize = DosEnv->cx;
            FileSystemFlags.lpFileSystemNameBuffer = (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode);
            result = demLFNGetVolumeInformation((LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode), &FileSystemFlags);
            if (SUCCEEDED(result)) {
                DosEnv->bx = FileSystemFlags.FileSystemFlags;
                DosEnv->cx = FileSystemFlags.nMaxComponentLen;
                DosEnv->dx = FileSystemFlags.nMaxPathLen;
            }
            break;
        case 0xA1:
            result = demLFNFindClose(DosEnv->bx);
            break;
        case 0xA6:
            result = demLFNGetFileInformationByHandle(DosEnv->bx, &FileInformation);
            if (SUCCEEDED(result))
                memmove(Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode), &FileInformation, sizeof(FileInformation));
            break;
        case 0xA7:
            switch (LOBYTE(DosEnv->bx)) {
            case 0:
                result = demLFNFileTimeControl(0, (LPFILETIME)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->si), 0, ProtectedMode), &DosAttr.t);
                if (SUCCEEDED(result)) {
                    DosEnv->dx = DosAttr.t.FatDate;
                    DosEnv->cx = DosAttr.t.FatTime;
                    DosEnv->bx = MAKEWPARAM(LOBYTE(DosEnv->bx), DosAttr.t.FatHundredths);
                }
                break;
            case 1:
                DosAttr.t.FatDate = DosEnv->dx;
                DosAttr.t.FatTime = DosEnv->cx;
                DosAttr.t.FatHundredths = HIBYTE(DosEnv->bx);
                lpFileTime = (LPFILETIME)Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode);
                result = demLFNFileTimeControl(getBL(), lpFileTime, &DosAttr.t);  // BUG? Shouldn't we use LOBYTE(DosEnv->bx) or hardcoded 1 instead?
                break;
            default:
                result = HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
                break;
            }
            break;
        case 0xA8:
            result = demLFNGenerateShortFileName(
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->es, DosEnv->di), 0, ProtectedMode),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->si), 0, ProtectedMode),
                         HIBYTE(DosEnv->dx),
                         LOBYTE(DosEnv->dx));
            break;
        case 0xAA:
            result = demLFNSubstControl(
                         HIBYTE(DosEnv->bx),
                         LOBYTE(DosEnv->bx),
                         (LPSTR)Sim32GetVDMPointer(GETULONG(DosEnv->ds, DosEnv->dx), 0, ProtectedMode));
            break;
        default:
            result = HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
            break;
        }
        if (!SUCCEEDED(result))
            *pResult = RtlNtStatusToDosError(result);
    } else {
        dempLFNCloseSearchHandles();
    }

    DBG_LOG(("LFN returns: 0x%x\r\n", result));
    return result;
}

VOID demLFNEntry()
{
    HRESULT result;
    USHORT ax;

    result = demLFNDispatch(NULL, FALSE, &ax);
    setAX(ax);
    setCF(SUCCEEDED(result)?0:1);
}

ULONG demWOWLFNEntry(LPVOID pUserFrame)
{
    DOSENV *DosEnv;
    ULONG result;
    USHORT ax;

    DosEnv = (DOSENV *)pUserFrame;
    result = demLFNDispatch(pUserFrame, TRUE, &ax);
    DosEnv->flags &= 0xFFBE;                  // Mask zero and carry flag
    if ((result & 0x80000000)) {
        DosEnv->flags |= 1;                     // set carry
        result = dempWOWLFNReturn(result);
        if (result & 0xFFFF0000)
            DosEnv->flags |= 0x40u;               // set zero
    } else {
        DosEnv->ax = ax;
    }
    return result;
}
