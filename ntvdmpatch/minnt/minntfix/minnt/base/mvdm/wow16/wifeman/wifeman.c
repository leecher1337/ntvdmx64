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
#include "wife.h"

#define WIFE_VERSION	0x0109


/****************************************************************************
    global data
****************************************************************************/
HINSTANCE hInst;                     // our module handle 


ULONG FAR PASCAL MiscGetVersion( VOID )
{
	return WIFE_VERSION;
}

BOOL FAR PASCAL MiscIsDBCSLeadByte( CHAR_SET CharSet, USHORT ch )
{
    return IsDBCSLeadByte((UCHAR)ch);
}

int NEAR PASCAL
LibMain(
    HINSTANCE hInstance,
    UINT cbHeap,
    LPSTR lpCmdLine
    )
{ 
	hInst = hInstance; 
	return TRUE;
}

