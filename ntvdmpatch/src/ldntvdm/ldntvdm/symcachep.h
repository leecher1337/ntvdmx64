#pragma once
typedef struct {
	char *pszFunction;
	LPWSTR lpKeyName;
} REGKEY_PAIR;

typedef struct {
	char *pszDLL;
	LPWSTR lpDLLKey;
	REGKEY_PAIR *keys;
	BOOL fx64Only;
} REGKEY_SYMS;
