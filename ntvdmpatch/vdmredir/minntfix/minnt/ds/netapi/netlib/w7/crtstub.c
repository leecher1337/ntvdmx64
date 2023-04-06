// HACKHACK: Stubs for the Win7 safe CRT so that we can link against netlibw7.lib
#include <windows.h>


void __cdecl
_invalid_parameter(
    WCHAR const* const expression,
    WCHAR const* const function_name,
    WCHAR const* const file_name,
    UINT   const line_number,
    UINT_PTR const reserved)
{
}

#if 0  // Already in BufferOverflowU.lib
void __fastcall __security_check_cookie(UINT cookie)
{
}
#endif