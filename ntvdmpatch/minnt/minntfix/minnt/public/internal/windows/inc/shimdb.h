// shimdb.h for NTVDM App compatibility support

#define COMPATLAYERMAXLEN 256

typedef DWORD           TAGREF;
typedef WORD            TAG;
typedef PVOID           HSDB;
typedef DWORD           TAGID;

#define SDBAPI          STDAPICALLTYPE

#define SDB_MAX_LAYERS  8
#define SDB_MAX_EXES    16
#define SDB_MAX_SDBS    16

#define APPHELP_HARDBLOCK     0x00000002

typedef struct tagSDBQUERYRESULT {
  TAGREF atrExes[SDB_MAX_EXES];
  DWORD  adwExeFlags[SDB_MAX_EXES];
  TAGREF atrLayers[SDB_MAX_LAYERS];
  DWORD  dwLayerFlags;
  TAGREF trApphelp;
  DWORD  dwExeCount;
  DWORD  dwLayerCount;
  GUID   guidID;
  DWORD  dwFlags;
  DWORD  dwCustomSDBMap;
  GUID   rgGuidDB[SDB_MAX_SDBS];
} SDBQUERYRESULT, *PSDBQUERYRESULT;

/* Windows XP has a slightly different struct, a bit smapper */
#define SDB_MAX_EXES_XP 4

typedef struct tagSDBQUERYRESULTxp {
    TAGREF atrExes[SDB_MAX_EXES_XP];
    TAGREF atrLayers[SDB_MAX_LAYERS];
    DWORD  dwLayerFlags;
    TAGREF trAppHelp;
    DWORD  dwExeCount;
    DWORD  dwLayerCount;
    GUID   guidID;
    DWORD  dwFlags;
    DWORD  dwCustomSDBMap;
    GUID   rgGuidDB[SDB_MAX_SDBS];
} SDBQUERYRESULTxp, *PSDBQUERYRESULTxp;

typedef struct _NTVDM_FLAGS {
    DWORD   dwWOWCompatFlags;
    DWORD   dwWOWCompatFlagsEx;
    DWORD   dwUserWOWCompatFlags;
    DWORD   dwWOWCompatFlags2;
    DWORD   dwWOWCompatFlagsFE;
    DWORD   dwFlagsInfoSize;
    PVOID   pFlagsInfo;
} NTVDM_FLAGS, *PNTVDM_FLAGS;

#define MAKEQWORD(dwLow, dwHigh) \
    ( ((ULONGLONG)(dwLow)) | ( ((ULONGLONG)(dwHigh)) << 32) )

#define GET_WOWCOMPATFLAGS_CMDLINE(pFlagInfo, dwFlag, ppCmdLine) \
    SdbQueryFlagInfo(pFlagInfo, TAG_FLAGS_NTVDM1, MAKEQWORD(dwFlag, 0), ppCmdLine)

#define GET_WOWCOMPATFLAGSEX_CMDLINE(pFlagInfo, dwFlag, ppCmdLine) \
    SdbQueryFlagInfo(pFlagInfo, TAG_FLAGS_NTVDM1, MAKEQWORD(0, dwFlag), ppCmdLine)

#define GET_USERWOWCOMPATFLAGS_CMDLINE(pFlagInfo, dwFlag, ppCmdLine) \
    SdbQueryFlagInfo(pFlagInfo, TAG_FLAGS_NTVDM2, MAKEQWORD(dwFlag, 0), ppCmdLine)

#define GET_WOWCOMPATFLAGS2_CMDLINE(pFlagInfo, dwFlag, ppCmdLine) \
    SdbQueryFlagInfo(pFlagInfo, TAG_FLAGS_NTVDM2, MAKEQWORD(0, dwFlag), ppCmdLine)

#define GET_WOWCOMPATFLAGSFE_CMDLINE(pFlagInfo, dwFlag, ppCmdLine) \
    SdbQueryFlagInfo(pFlagInfo, TAG_FLAGS_NTVDM3, MAKEQWORD(dwFlag, 0), ppCmdLine)

typedef struct _APPHELP_INFO {
    DWORD   dwHtmlHelpID;
    DWORD   dwSeverity;
    LPCTSTR lpszAppName;
    GUID    guidID;
    TAGID   tiExe;
    GUID    guidDB;
    BOOL    bOfflineContent;
    BOOL    bUseHTMLHelp;
    LPCTSTR lpszChmFile;
    LPCTSTR lpszDetailsFile;
    BOOL    bPreserveChoice;
    BOOL    bMSI;
} APPHELP_INFO, *PAPPHELP_INFO;

BOOL
SDBAPI
SdbQueryFlagInfo(
    IN PVOID pvFlagInfo,
    IN TAG tFlagType,
    IN ULONGLONG ullFlagMask,
    OUT LPCWSTR * ppCmdLine
    );

BOOL
SDBAPI
SdbFreeFlagInfo(
    IN PVOID pvFlagInfo
    );

VOID
SDBAPI
SdbReleaseDatabase(
    IN HSDB hSDB
    );


BOOL
ApphelpGetNTVDMInfo(
    IN  LPCWSTR pwszPath,
    IN  LPCWSTR pwszModule,
    IN  LPCWSTR pEnvironment,
    OUT LPWSTR pszCompatLayer,
    OUT PNTVDM_FLAGS pFlags,
    OUT PAPPHELP_INFO pAHInfo,
    OUT HSDB*  phSDB,
    OUT PSDBQUERYRESULT pQueryResult
    );

#define TAG_TYPE_QWORD          0x5000

#define TAG_FLAGS_NTVDM1             (0x9 | TAG_TYPE_QWORD)
#define TAG_FLAGS_NTVDM2             (0xA | TAG_TYPE_QWORD)
#define TAG_FLAGS_NTVDM3             (0xB | TAG_TYPE_QWORD)
