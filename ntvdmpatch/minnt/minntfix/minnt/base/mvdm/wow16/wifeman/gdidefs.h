/**[f******************************************************************
 * gdi.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************
 * 12Jun87	sjp		Added CC_ROUNDRECT.
 ***************************************************************************/


typedef struct
    {
    short int dfType;
    short int dfPoints;
    short int dfVertRes;
    short int dfHorizRes;
    short int dfAscent;
    short int dfInternalLeading;
    short int dfExternalLeading;
    BYTE dfItalic;
    BYTE dfUnderline;
    BYTE dfStrikeOut;
    short int	dfWeight;
    BYTE dfCharSet;
    short int dfPixWidth;
    short int dfPixHeight;
    BYTE dfPitchAndFamily;
    short int dfAvgWidth;
    short int dfMaxWidth;
    BYTE dfFirstChar;
    BYTE dfLastChar;
    BYTE dfDefaultChar;
    BYTE dfBreakChar;
    short int	dfWidthBytes;
    unsigned long int	dfDevice;
    unsigned long int	dfFace;
    unsigned long int	dfBitsPointer;
    unsigned long int	dfBitsOffset;
    BYTE dfReservedByte;
    BYTE dfMaps;

    } FONTINFO;
typedef FONTINFO FAR *LPFONTINFO;

typedef     struct
    {
    short int	      Rop2;
    short int	      bkMode;
    unsigned long int bkColor;
    unsigned long int TextColor;
    short int	      TBreakExtra;
    short int	      BreakExtra;
    short int	      BreakErr;
    short int	      BreakRem;
    short int	      BreakCount;
    short int	      CharExtra;
    unsigned long int LbkColor;
    unsigned long int LTextColor;
    } DRAWMODE;
typedef DRAWMODE FAR *LPDRAWMODE;

typedef     struct
    {
    short int ftHeight;
    short int ftWidth;
    short int ftEscapement;
    short int ftOrientation;
    short int ftWeight;
    BYTE ftItalic;
    BYTE ftUnderline;
    BYTE ftStrikeOut;
    BYTE ftOutPrecision;
    BYTE ftClipPrecision;
    unsigned short int ftAccelerator;
    short int ftOverhang;
    } TEXTXFORM;
typedef TEXTXFORM FAR *LPTEXTXFORM;

