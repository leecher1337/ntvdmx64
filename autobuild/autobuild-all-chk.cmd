@echo off
if "%SIZ_NTBLD%"=="" set SIZ_NTBLD=chk
set NOPAUSE=1
set KEEPWD=1
call autobuild-ccpu-%SIZ_NTBLD%.cmd 
if not exist releases\nul exit /b
md releases-all\ccpu 2>bul
rmdir /s /q releases-all\ccpu\%SIZ_NTBLD% 2>nul
move /y releases releases-all\ccpu\%SIZ_NTBLD%
set KEEPPAT=1
call autobuild-haxm-%SIZ_NTBLD%.cmd 
if not exist releases\nul exit /b
md releases-all\haxm 2>nul
rmdir /s /q releases-all\haxm\%SIZ_NTBLD% 2>nul
move /y releases releases-all\haxm\%SIZ_NTBLD%
