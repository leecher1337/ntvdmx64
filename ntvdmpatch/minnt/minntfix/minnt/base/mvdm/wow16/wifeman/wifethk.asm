    TITLE   WIFETHK.ASM
    PAGE    ,132
;
; WOW v1.0
;
; Copyright (c) 1991-1992, Microsoft Corporation
;
; WIFETHK.ASM
; Thunks in 16-bit space to route Windows API calls to WOW32
;
; History:
;
;   01-May-2020 leecher1337
;   Created.
;

ifndef WINDEBUG
    KDEBUG = 0
    WDEBUG = 0
else
    KDEBUG = 1
    WDEBUG = 1
endif


    .286p

    .xlist
    include cmacros.inc
    include wow.inc
    include wowwife.inc
    .list

sBegin  CODE
assumes CS,CODE
assumes DS,NOTHING
assumes ES,NOTHING

; Kernel API thunks

    WifeManThunk MiscGetEUDCLeadByteRange


sEnd    CODE

end
