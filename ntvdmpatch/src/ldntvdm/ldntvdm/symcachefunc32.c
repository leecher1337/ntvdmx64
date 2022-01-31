/*Project: ldntvdm
* Module : symcache
* Author : leecher@dose.0wnz.at
* Descr. : Symbol caching routines for 32bit Symbols so that 64bit loader can cache them too
* Changes: 31.01.2022  - Created
*/

#ifdef _WIN64
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "symcache.h"
#include "symcachep.h"
#undef _WIN64
#include "config.h"
#include "symcachefunc.h"
const int g_aSyms32Size = sizeof(g_aSyms32) / sizeof(g_aSyms32[0]);
#endif
