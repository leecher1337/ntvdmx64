@echo off
echo ----------------------------------------------------
echo Preparing MINNT repository, please wait...
echo ----------------------------------------------------
echo.

setlocal enableDelayedExpansion
if exist "%ProgramFiles%\7-Zip" set PATH=%PATH:)=^)%;"%ProgramFiles%\7-Zip"
7z >nul 2>&1
if errorlevel 255 (
for /F "tokens=2*" %%r in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\7-zip" /v Path 2^>nul') do set ZPATH=%%s
if "!ZPATH!"=="" for /F "tokens=2*" %%r in ('reg query "HKEY_CURRENT_USER\SOFTWARE\7-zip" /v Path 2^>nul') do set ZPATH=%%s
if not "!ZPATH!"=="" set PATH=%PATH:)=^)%;!ZPATH!
7z >nul 2>&1
if errorlevel 255 (
echo Please install 7zip first, then run again
start http://www.7-zip.de
pause
goto fini
)
)
endlocal & set PATH=%PATH%


rem Directory that contains the fixes to be applied to minnt
set minntfix=minntfix
set workdir=..\minnt\work
if not exist %minntfix% (
echo Directory %minntfix% which should contain minnt fixes does not exist.
echo Cannot continue.
pause
goto fini
)

rem Creating directory structure
md %minntfix%\minnt\public\internal\base\inc 2>nul
md %minntfix%\minnt\public\internal\net\inc 2>nul
md %minntfix%\minnt\public\sdk\lib\x86 2>nul
md %minntfix%\minnt\public\sdk\inc 2>nul

md %workdir% 2>nul

rem minnt SDK Update to Win XP/2003 SDK
if exist ..\..\NTOSBE-master\src\public\sdk\inc\commdlg.h xcopy /d ..\..\NTOSBE-master\src\public\sdk\inc %minntfix%\minnt\public\sdk\inc\
for %%a in (libcmt.lib msvcrt.lib ntdll.lib) do if exist ..\..\NTOSBE-master\src\public\sdk\lib\x86\%%~a xcopy /y ..\..\NTOSBE-master\src\public\sdk\lib\x86\%%~a %minntfix%\minnt\public\sdk\lib\x86\

if not exist %minntfix%\minnt\public\sdk\lib\x86\netlibw7.lib goto doddk
goto ddkok
:doddk
echo Need to extract DDK files.
:redoddk
if exist %workdir%\WDK\libs_x86fre_cab001.cab (
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _netlib.lib_00290 %minntfix%\minnt\public\sdk\lib\x86\netlibw7.lib
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _samlib.lib_00337 %minntfix%\minnt\public\sdk\lib\x86\samlib.lib
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _safecrtnt.lib_00027 %minntfix%\minnt\public\sdk\lib\x86\safecrtnt.lib
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _rpcutil.lib_00335 %minntfix%\minnt\public\sdk\lib\x86\rpcutil.lib
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _rpcrt4.lib_00334  %minntfix%\minnt\public\sdk\lib\x86\rpcrt4w7.lib
call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _netapi32.lib_00289 %minntfix%\minnt\public\sdk\lib\x86\netapi32w7.lib
if not exist %minntfix%\minnt\public\sdk\lib\x86\winspool.lib  call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _winspool.lib_00389  %minntfix%\minnt\public\sdk\lib\x86\winspool.lib
if not exist %minntfix%\minnt\public\sdk\lib\x86\kernl32p.lib  call :expandf %workdir%\WDK\libs_x86fre_cab001.cab _kernl32p.lib_00256 %minntfix%\minnt\public\sdk\lib\x86\kernl32p.lib
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
call :expandiso GRMWDK_EN_7600_1.ISO WDK\libs_x86fre_cab001.cab
goto redoddk

:ddkok
set XPSRC=
if exist %minntfix%\minnt\public\internal\net\inc\icanon.h goto winxpok
if exist %workdir%\nt5src.7z 7z x -y %workdir%\nt5src.7z Win2k3\3790src2.cab Win2k3\3790src4.cab  -o%workdir%
if exist %workdir%\Win2k3\3790src*.cab move %workdir%\Win2k3\3790src*.cab %workdir%\
if exist %workdir%\Win2K3.7z  7z x -y %workdir%\Win2K3.7z 3790src2.cab 3790src4.cab -o%workdir%
if exist %workdir%\3790src2.cab set XPSRC2=%workdir%\3790src2.cab
if exist %workdir%\3790src4.cab set XPSRC4=%workdir%\3790src4.cab
if "%XPSRC4%"=="" set XPSRC2=
if "%XPSRC2%"=="" (
echo Building a working VDM Redirector for Windows 7 or above requires you to have 
echo the Windows Server 2003 sourcecode available.
echo I can detect multiple flavours of the source.
echo In order to use it, please place ONE of the following files in %workdir%:
echo nt5src.7z
echo 3790src2.cab, 3790src4.cab
echo Win2K3.7z
echo Sorry, I cannot tell you where to find it, but you may find it with your 
echo favourite search engine or torrent tracker
pause
goto fini
)
7z x -y %XPSRC4% public\internal\base\inc\ public\internal\ds\inc\ public\internal\net\inc\ public\sdk\inc\ -o%workdir%
7z x -y %XPSRC2% ds\inc\ ds\netapi -o%workdir%
attrib -R /S %workdir%

:winxpok
for %%I in (smbmacro.h smb.h smbtrans.h status.h srvfsctl.h xactsrv2.h svcs.h srvann.h bowdbg.h dfspriv.h dfsfsctl.h) do copy /y %workdir%\public\internal\base\inc\%%I %minntfix%\minnt\public\internal\base\inc\%%I
for %%I in (netdebug.h netlib.h ssi.h dosprint.h loghours.h rxprint.h rxserver.h secobj.h) do copy /y %workdir%\public\internal\ds\inc\%%I %minntfix%\minnt\public\internal\ds\inc\%%I
for %%I in (hostannc.h icanon.h rpcutil.h netcan.h rap.h) do copy /y %workdir%\public\internal\net\inc\%%I %minntfix%\minnt\public\internal\net\inc\%%I
for %%I in (debugfmt.h ntddnfs.h nettypes.h lmbrowsr.h lmchdev.h ntddbrow.h ntlsa.h lmdfs.h dsgetdc.h winldap.h schnlsp.h) do copy /y %workdir%\public\sdk\inc\%%I %minntfix%\minnt\public\sdk\inc\%%I
xcopy /e /y %workdir%\ds\netapi\inc %minntfix%\minnt\ds\netapi\inc\
xcopy /e /y %workdir%\ds\inc %minntfix%\minnt\ds\inc\
for %%I in (xactsrv dosprint rap svcdlls\srvsvc\client rpcxlate\rxcommon rpcxlate\rxapi) do (
  copy /y %workdir%\ds\netapi\%%I\*.c %minntfix%\minnt\ds\netapi\%%I\
  if exist %workdir%\ds\netapi\%%I\*.h copy /y %workdir%\ds\netapi\%%I\*.h %minntfix%\minnt\ds\netapi\%%I\
)
copy /y %workdir%\ds\netapi\svcdlls\browser\client\*.c  %minntfix%\minnt\ds\netapi\svcdlls\browser2\client\
copy /y %workdir%\ds\netapi\svcdlls\browser\client\*.h  %minntfix%\minnt\ds\netapi\svcdlls\browser2\client\
copy /y %workdir%\ds\netapi\rap\*.def %minntfix%\minnt\ds\netapi\rap\
copy /y %workdir%\ds\netapi\rap\*.rc %minntfix%\minnt\ds\netapi\rap\
copy /y %workdir%\ds\netapi\svcdlls\srvsvc\idl\*.h %minntfix%\minnt\ds\netapi\svcdlls\srvsvc\
copy /y %workdir%\ds\netapi\svcdlls\srvsvc\idl\*.acf %minntfix%\minnt\ds\netapi\svcdlls\srvsvc\
copy /y %workdir%\ds\netapi\svcdlls\srvsvc\idl\*.idl %minntfix%\minnt\ds\netapi\svcdlls\srvsvc\
mkdir %minntfix%\minnt\ds\netapi\netlib\w7 2>nul
copy /y %workdir%\ds\netapi\api\canonapi.c %minntfix%\minnt\ds\netapi\netlib\w7\
copy /y %workdir%\public\internal\ds\inc\brcommon.h %minntfix%\minnt\ds\netapi\svcdlls\browser2\
mkdir  %minntfix%\minnt\public\internal\ds\inc\ntcrypto  2>nul
copy /y %workdir%\public\internal\ds\inc\crypto\rc4.h %minntfix%\minnt\public\internal\ds\inc\ntcrypto\
:prepared


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
