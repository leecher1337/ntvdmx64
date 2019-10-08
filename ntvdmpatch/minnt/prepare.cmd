@echo off
echo ----------------------------------------------------
echo Preparing MINNT repository
echo ----------------------------------------------------
echo.

7z >nul 2>&1
if errorlevel 255 (
echo Please install 7zip first, then run again
start http://www.7-zip.de
pause
goto fini
)

rem Directory that contains the fixes to be applied to minnt
set minntfix=minntfix
set workdir=work
if not exist %minntfix% (
echo Directory %minntfix% which should contain minnt fixes does not exist.
echo Cannot continue.
pause
goto fini
)

rem Creating directory structure
md %minntfix%\NTOSBE-master\tools\x86\idw
md %minntfix%\minnt\base\mvdm\wow16
md %minntfix%\minnt\public\ddk\lib\x86
md %minntfix%\minnt\public\internal\base\inc
md %minntfix%\minnt\public\sdk\lib\x86
md %workdir%

if not exist %minntfix%\minnt\public\ddk\lib\x86\umpdddi.lib goto doddk
if not exist %minntfix%\minnt\public\sdk\lib\x86\winspool.lib goto doddk
for %%a in (compstdui.h winddi.h winddiui.h) do if not exist "%minntfix%\minnt\public\oak\inc\%%~a" goto doddk
goto ddkok
:doddk
echo Need to extract DDK files.
:redoddk
if exist %workdir%\WDK\wxplibs_x86fre_cab001.cab (
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _umpdddi.lib_00315 %minntfix%\minnt\public\ddk\lib\x86\umpdddi.lib
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _winspool.lib_00345 %minntfix%\minnt\public\sdk\lib\x86\winspool.lib
) else goto ddkisoex
rem
rem Current DDK headers not really working with build system/older headers, so we take the headers from
rem old-src and adapt them
rem
rem if exist %workdir%\WDK\headers_cab001.cab (
rem call :expandf %workdir%\WDK\headers_cab001.cab _compstui.h_00602 %minntfix%\minnt\public\oak\inc\compstui.h
rem call :expandf %workdir%\WDK\headers_cab001.cab _winddi.h_00629 %minntfix%\minnt\public\oak\inc\winddi.h
rem call :expandf %workdir%\WDK\headers_cab001.cab _winddiui.h_00630 %minntfix%\minnt\public\oak\inc\winddiui.h
rem ) else goto ddkisoex
goto ddkok
:ddkisoex
if not exist %workdir%\GRMWDK_EN_7600_1.ISO (
echo In order to get the required DDK headers, you need to download the NT WDK 7600
echo Place GRMWDK_EN_7600_1.ISO in %workdir% and then try again.
start https://www.microsoft.com/en-us/download/details.aspx?id=11800
pause
goto fini
)
call :expandiso GRMWDK_EN_7600_1.ISO WDK\wxplibs_x86fre_cab001.cab 
rem call :expandiso GRMWDK_EN_7600_1.ISO WDK\headers_cab001.cab
goto redoddk

:ddkok
for %%a in (dbgeng.dll dbghelp.dll symbolcheck.dll symchk.exe) do if not exist "%minntfix%\minnt\public\internal\base\inc\%%~a" goto dosdk
goto sdkok
:dosdk
echo Need to extract SDK files.
:redosdk
if exist %workdir%\Setup\WinSDKDebuggingTools\dbg_x86.msi (
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi DbgengDLL
move /y %workdir%\DbgengDLL %minntfix%\NTOSBE-master\tools\x86\idw\dbgeng.dll
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi _DbghelpDLL
move /y %workdir%\_DbghelpDLL %minntfix%\NTOSBE-master\tools\x86\idw\dbghelp.dll
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi SymbolcheckDLL
move /y %workdir%\SymbolcheckDLL %minntfix%\NTOSBE-master\tools\x86\idw\symbolcheck.dll
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi SymchkEXE
move /y %workdir%\SymchkEXE %minntfix%\NTOSBE-master\tools\x86\idw\symchk.exe
goto sdkok
)
if not exist %workdir%\GRMSDK_EN_DVD.iso  (
echo In order to get the required SDK headers, you need to download the Win 7 SDK
echo Place GRMSDK_EN_DVD.iso in %workdir% and then try again.
start https://www.microsoft.com/en-us/download/details.aspx?id=8442
pause
goto fini
)
call :expandiso GRMSDK_EN_DVD.iso Setup\WinSDKDebuggingTools\dbg_x86.msi
goto redosdk

:sdkok
for %%a in (gdispool.h splapip.h) do if not exist "%minntfix%\minnt\public\internal\base\inc\%%~a" goto dooldsrc
if not exist "%minntfix%\minnt\base\mvdm\wow16\makefile" goto dooldsrc
goto oldsrcok
:dooldsrc
echo Need to copy missing files from old-src
:redooldsrc
if not exist %workdir%\old-src.trunk.r687.20150728.7z (
echo In order to get the missing header files, download old-src.trunk.r687.20150728.7z
echo Place it in %workdir% and then try again.
echo You have to find this yourself on the Internet
pause
goto fini
)
7z x -y %workdir%\old-src.trunk.r687.20150728.7z old-src\nt\private\windows\inc\gdispool.h old-src\nt\private\windows\spooler\inc\splapip.h old-src\nt\public\oak\inc\winddiui.h old-src\nt\public\oak\inc\compstui.h old-src\nt\private\mvdm\wow16 -o%workdir%
if not exist %workdir%\old-src\nt\private\windows\inc\gdispool.h (
echo Cannot expand %workdir%\old-src\nt\private\windows\inc\gdispool.h from %workdir%\old-src.trunk.r687.20150728.7z. 
echo Cannot continue.
pause
goto fini
)

move /y %workdir%\old-src\nt\private\windows\inc\gdispool.h %minntfix%\minnt\public\internal\base\inc\
move /y %workdir%\old-src\nt\private\windows\spooler\inc\splapip.h %minntfix%\minnt\public\internal\base\inc\
move /y %workdir%\old-src\nt\public\oak\inc\compstui.h %minntfix%\minnt\public\oak\inc\
move /y %workdir%\old-src\nt\public\oak\inc\winddiui.h %minntfix%\minnt\public\oak\inc\
echo #include "winddiui_xp.h" >>%minntfix%\minnt\public\oak\inc\winddiui.h


xcopy /s /y %workdir%\old-src\nt\private\mvdm\wow16 %minntfix%\minnt\base\mvdm\wow16\
del %minntfix%\minnt\base\mvdm\wow16\inc\ime.h


:oldsrcok
echo The patch directory is now prepared. You may delete the contents of the 
echo of the %workdir% directory now.
pause
goto fini

:expandf
expand %1 -F:%2 %workdir%
if not exist %workdir%\%2 (
echo Cannot expand %2 from %1. 
echo Cannot continue.
pause
exit
)
move /y %workdir%\%2 %3
exit /B

:expandiso
7z x -y %workdir%\%1 %2 -o%workdir%
if not exist %workdir%\%2 (
echo Cannot expand %2 from %1. 
echo Cannot continue.
pause
exit
)
exit /B

:fini