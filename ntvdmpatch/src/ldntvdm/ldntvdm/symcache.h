#ifdef _WIN64
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage64"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication64"
#else
#define REGKEY_BasepProcessInvalidImage L"BasepProcessInvalidImage32"
#define REGKEY_BaseIsDosApplication L"BaseIsDosApplication32"
#endif

BOOL UpdateSymbolCache();
