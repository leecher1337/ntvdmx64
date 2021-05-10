@echo off
echo ----------------------------------------------------
echo Preparing MINNT repository, please wait...
echo ----------------------------------------------------
echo.

if exist "%ProgramFiles%\7-Zip" set PATH=%PATH:)=^)%;"%ProgramFiles%\7-Zip"
7z >nul 2>&1
if errorlevel 255 (
for /F "skip=2 tokens=3*" %%r in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\7-zip" /v Path') do echo set PATH=%PATH:)=^)%;%%r
7z >nul 2>&1
if errorlevel 255 (
echo Please install 7zip first, then run again
start http://www.7-zip.de
pause
goto fini
)
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
md %minntfix%\minnt\public\internal\shell\inc
md %minntfix%\minnt\public\sdk\lib\x86
md %minntfix%\minnt\public\sdk\inc
md %minntfix%\minnt\base\mvdm\softpc.new\host\inc\alpha
md %minntfix%\minnt\base\mvdm\softpc.new\host\inc\mips
md %minntfix%\minnt\base\mvdm\softpc.new\host\inc\ppc
md %workdir%

rem minnt SDK Update to Win XP/2003 SDK
if exist ..\..\NTOSBE-master\src\public\sdk\inc\commdlg.h xcopy /y ..\..\NTOSBE-master\src\public\sdk\inc %minntfix%\minnt\public\sdk\inc\
for %%a in (libcmt.lib msvcrt.lib ntdll.lib) do if exist ..\..\NTOSBE-master\src\public\sdk\lib\x86\%%~a xcopy /y ..\..\NTOSBE-master\src\public\sdk\lib\x86\%%~a %minntfix%\minnt\public\sdk\lib\x86\

if not exist %minntfix%\minnt\public\ddk\lib\x86\umpdddi.lib goto doddk
if not exist %minntfix%\minnt\public\sdk\lib\x86\winspool.lib goto doddk
if not exist %minntfix%\minnt\public\sdk\lib\x86\userenv.lib goto doddk
for %%a in (compstdui.h winddi.h winddiui.h) do if not exist "%minntfix%\minnt\public\oak\inc\%%~a" goto doddk
if not exist %minntfix%\minnt\public\sdk\inc\commctrl.h goto doddk
goto ddkok
:doddk
echo Need to extract DDK files.
:redoddk
if exist %workdir%\WDK\wxplibs_x86fre_cab001.cab (
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _umpdddi.lib_00315 %minntfix%\minnt\public\ddk\lib\x86\umpdddi.lib
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _winspool.lib_00345 %minntfix%\minnt\public\sdk\lib\x86\winspool.lib
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _userenv.lib_00322 %minntfix%\minnt\public\sdk\lib\x86\userenv.lib
call :expandf %workdir%\WDK\wxplibs_x86fre_cab001.cab _shell32.lib_00284 %minntfix%\minnt\public\sdk\lib\x86\shell32.lib
) else goto ddkisoex
if exist %workdir%\WDK\libs_x86fre_cab001.cab (
rem For RegisterConsoleVDM with fewer parameters starting with Win7
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _kernl32p.lib_00256 %minntfix%\minnt\public\sdk\lib\x86\kernl32p.lib
rem For GetStringBitmapA
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _gdi32.lib_00234 %minntfix%\minnt\public\sdk\lib\x86\gdi32.lib
) else goto ddkisoex

if exist %workdir%\WDK\headers_cab001.cab (
call :expandf %workdir%\WDK\headers_cab001.cab _COMMCTRL.H_00634 %minntfix%\minnt\public\sdk\inc\commctrl.h
rem
rem Current DDK headers not really working with build system/older headers, so we take the headers from
rem old-src and adapt them
rem
rem call :expandf %workdir%\WDK\headers_cab001.cab _compstui.h_00602 %minntfix%\minnt\public\oak\inc\compstui.h
rem call :expandf %workdir%\WDK\headers_cab001.cab _winddi.h_00629 %minntfix%\minnt\public\oak\inc\winddi.h
rem call :expandf %workdir%\WDK\headers_cab001.cab _winddiui.h_00630 %minntfix%\minnt\public\oak\inc\winddiui.h
) else goto ddkisoex
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
call :expandiso GRMWDK_EN_7600_1.ISO WDK\libs_x86fre_cab001.cab
call :expandiso GRMWDK_EN_7600_1.ISO WDK\headers_cab001.cab
goto redoddk

:ddkok
for %%a in (dbgeng.dll dbghelp.dll) do if not exist "%minntfix%\minnt\public\internal\base\inc\%%~a" goto dosdk
goto sdkok
:dosdk
echo Need to extract SDK files.
:redosdk
if exist %workdir%\Setup\WinSDKDebuggingTools\dbg_x86.msi (
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi DbgengDLL
move /y %workdir%\DbgengDLL %minntfix%\NTOSBE-master\tools\x86\idw\dbgeng.dll
call :expandiso Setup\WinSDKDebuggingTools\dbg_x86.msi _DbghelpDLL
move /y %workdir%\_DbghelpDLL %minntfix%\NTOSBE-master\tools\x86\idw\dbghelp.dll
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
rem In case that WinXP/2k3 src is present, this would use original WOW16 code instead of our 
rem reconstruction. However, orignal WOW16 code is incompatible with CCPU, therefore this 
rem code branch will never become active. It's just here for experimental purposes, ignore it.
set WOW16=old-src\nt\private\mvdm\wow16
if exist %minntfix%\minnt\base\mvdm\wow16\makefile.inc goto winxpok
if exist "%workdir%\XPSP1.7z" 7z x -y %workdir%\XPSP1.7z xpsrc1.cab -o%workdir%
if exist %workdir%\xpsrc1.cab 7z x -y %workdir%\xpsrc1.cab base\mvdm\wow16 -o%workdir%\
if not exist %workdir%\base\mvdm\wow16 (
  if exist "%workdir%\Win2K3.7z"  7z x -y %workdir%\Win2K3.7z 3790src1.cab -o%workdir%
  if exist %workdir%\3790src1.cab 7z x -y %workdir%\3790src1.cab base\mvdm\wow16 -o%workdir%\
)
if exist %workdir%\base\mvdm\wow16 (
  rd /q /s %minntfix%\minnt\base\mvdm\wow16
  move /y %workdir%\base\mvdm\wow16 %minntfix%\minnt\base\mvdm\
  set WOW16=
)

:winxpok
for %%a in (gdispool.h splapip.h) do if not exist "%minntfix%\minnt\public\internal\base\inc\%%~a" goto dooldsrc
goto oldsrcok
:dooldsrc
echo Need to copy missing files from old-src
:redooldsrc
set OLDSRC=old-src.trunk.r687.20150728.7z
if exist %workdir%\old-src-sr687.7z set OLDSRC=old-src-sr687.7z
if not exist %workdir%\%OLDSRC% (
echo In order to get the missing header files, download %OLDSRC%
echo Place it in %workdir% and then try again.
echo You have to find this yourself on the Internet
pause
goto fini
)
7z x -y %workdir%\%OLDSRC% old-src\nt\private\windows\media\avi\mciavi32\vfw16 old-src\nt\private\windows\inc\comctrlp.h old-src\nt\private\windows\inc\commdlgp.h old-src\nt\private\windows\inc\shlapip.h old-src\nt\private\windows\inc\prshtp.h old-src\nt\private\windows\inc\shlwapip.h old-src\nt\private\windows\inc\gdispool.h old-src\nt\private\windows\spooler\inc\splapip.h old-src\nt\public\oak\inc\winddiui.h old-src\nt\private\sdktools\jetadmin\inc\winioctl.h old-src\nt\private\sdktools\jetadmin\inc\dsound.h old-src\nt\public\oak\inc\compstui.h %WOW16% old-src\nt\private\mvdm\softpc.new\host\inc\alpha old-src\nt\private\mvdm\softpc.new\host\inc\mips old-src\nt\private\mvdm\softpc.new\host\inc\ppc old-src\nt\private\mvdm\dpmi old-src\nt\private\mvdm\dpmi32 old-src\nt\private\mvdm\inc\intmac.inc old-src\nt\private\mvdm\inc\dpmi.h old-src\nt\private\mvdm\tools16\implib.exe old-src\nt\private\mvdm\tools16\rc16.exe old-src\nt\private\mvdm\tools16\rcpp.exe old-src\tools\x86\idw\sednew.exe old-src\nt\private\sdktools\upd old-src\nt\private\sdktools\qgrep -o%workdir%
if not exist %workdir%\old-src\nt\private\windows\inc\gdispool.h (
echo Cannot expand %workdir%\old-src\nt\private\windows\inc\gdispool.h from %workdir%\%OLDSRC%
echo Cannot continue.
pause
goto fini
)
move /y %workdir%\old-src\nt\private\windows\inc\gdispool.h %minntfix%\minnt\public\internal\base\inc\
move /y %workdir%\old-src\nt\private\windows\spooler\inc\splapip.h %minntfix%\minnt\public\internal\base\inc\
move /y %workdir%\old-src\nt\public\oak\inc\compstui.h %minntfix%\minnt\public\oak\inc\
move /y %workdir%\old-src\nt\private\sdktools\jetadmin\inc\winioctl.h %minntfix%\minnt\public\sdk\inc\
move /y %workdir%\old-src\nt\private\sdktools\jetadmin\inc\dsound.h %minntfix%\minnt\public\sdk\inc\
move /y %workdir%\old-src\nt\public\oak\inc\winddiui.h %minntfix%\minnt\public\oak\inc\
for %%a in (comctrlp.h commdlgp.h prshtp.h shlapip.h shlwapip.h) do move /y %workdir%\old-src\nt\private\windows\inc\%%~a %minntfix%\minnt\public\internal\shell\inc\
echo #include "winddiui_xp.h" >>%minntfix%\minnt\public\oak\inc\winddiui.h


if exist %workdir%\old-src\nt\private\mvdm\wow16\makefile (
  xcopy /e /y %workdir%\old-src\nt\private\mvdm\wow16 %minntfix%\minnt\base\mvdm\wow16\
  del %minntfix%\minnt\base\mvdm\wow16\inc\ime.h
)
xcopy /e /y %workdir%\old-src\nt\private\mvdm\softpc.new\host\inc\alpha %minntfix%\minnt\base\mvdm\softpc.new\host\inc\alpha\
xcopy /e /y %workdir%\old-src\nt\private\mvdm\softpc.new\host\inc\mips %minntfix%\minnt\base\mvdm\softpc.new\host\inc\mips\
xcopy /e /y %workdir%\old-src\nt\private\mvdm\softpc.new\host\inc\ppc %minntfix%\minnt\base\mvdm\softpc.new\host\inc\ppc\
xcopy /e /y %workdir%\old-src\nt\private\mvdm\dpmi %minntfix%\minnt\base\mvdm\dpmi.old\
xcopy /e /y %workdir%\old-src\nt\private\mvdm\dpmi32 %minntfix%\minnt\base\mvdm\dpmi32.old\
xcopy /Y %workdir%\old-src\nt\private\mvdm\inc\intmac.inc %minntfix%\minnt\base\mvdm\dpmi.old\
md %minntfix%\minnt\base\mvdm\inc
copy /Y %workdir%\old-src\nt\private\mvdm\inc\dpmi.h %minntfix%\minnt\base\mvdm\inc\dpmi.h.old
xcopy /e /y %workdir%\old-src\nt\private\mvdm\tools16\implib.exe %minntfix%\NTOSBE-master\tools\x86\tools16\
xcopy /e /y %workdir%\old-src\nt\private\mvdm\tools16\rcpp.exe %minntfix%\NTOSBE-master\tools\x86\tools16\
copy /y %workdir%\old-src\nt\private\mvdm\tools16\rc16.exe %minntfix%\NTOSBE-master\tools\x86\tools16\rc16dos.exe
copy /Y %workdir%\old-src\tools\x86\idw\sednew.exe %minntfix%\NTOSBE-master\tools\x86\idw\sed.exe
xcopy /e /y %workdir%\old-src\nt\private\sdktools\upd %minntfix%\NTOSBE-master\src\sdktools\upd\
rem Windows 7 and above has UAC. Its heuristic assumes that if some executable starts with name upd or setup or patch, etc.
rem that it needs to elevate its privileges. This is not the case for upd.exe and would interfere with build process
rem Therefore we need to embed a manifest in upd
echo 1 24 upd.man >>%minntfix%\NTOSBE-master\src\sdktools\upd\upd.rc
xcopy /e /y %workdir%\old-src\nt\private\sdktools\qgrep %minntfix%\NTOSBE-master\src\sdktools\qgrep\
xcopy /Y %minntfix%\minnt\base\mvdm\dos\v86\cmd\append\dirs %minntfix%\minnt\base\mvdm\dpmi.old\

for %%I in (command debug edlin exe2bin graphics keyb loadfix mem nlsfunc setver) do (
  md %minntfix%\minnt\base\mvdm\dos\v86\cmd\%%I
  xcopy /Y %minntfix%\minnt\base\mvdm\dos\v86\cmd\append\dirs %minntfix%\minnt\base\mvdm\dos\v86\cmd\%%I\
)

rem Copy Video for Windows to release template folder
xcopy /e /y %workdir%\old-src\nt\private\windows\media\avi\mciavi32\vfw16 ..\release\vfw16\

:oldsrcok
echo The patch directory is now prepared. You may delete the contents of the 
echo of the %workdir% directory now.
if not "%1"=="batch" pause
goto fini

:expandf
%systemroot%\system32\expand %1 -F:%2 %workdir%
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
