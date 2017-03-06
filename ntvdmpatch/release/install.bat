@echo off

>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
  echo Requesting administrative privileges...
  goto UACPrompt
) else ( goto gotAdmin )
:UACPrompt
echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"
"%temp%\getadmin.vbs"
exit /B
:gotAdmin
if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
pushd "%CD%"
CD /D "%~dp0"

echo ---------------------------------------------
echo Checking machine
echo ---------------------------------------------
reg query HKLM\Hardware\Description\System\CentralProcessor\0 | Find /i "x86" >nul
if not errorlevel 1 (
  echo You appear to be running this installation on a 32bit machine.
  echo This NTVDMx64 is only meant to be used on an x64 machine, please use 
  echo NTVDM shipped with your windows installation instead.
  echo Installation aborted
  goto fini
)

for /F "skip=2 tokens=3" %%r in ('reg query HKLM\SYSTEM\CurrentControlSet\Control\SecureBoot\State /v UEFISecureBootEnabled') do if "%%r"=="0x1" (
  echo It seems that your machine has secure boot feature enabled.
  echo This prevents our AppInit_DLL loader from working properly and 
  echo therefore prevents start of NTVDM.
  echo Please disabe secure boot in BIOS, reboot and try again
  start https://msdn.microsoft.com/en-us/windows/hardware/commercialize/manufacture/desktop/disabling-secure-boot
  goto fini
)

cls

echo ---------------------------------------------
echo Installing symbol server
echo ---------------------------------------------
xcopy /s /y /g /k /d /i symbols %SystemRoot%\symbols
if errorlevel 1 goto errcpy
util\symfetch %systemRoot%\system32\kernel32.dll %systemRoot%\syswow64\kernel32.dll 
if errorlevel 1 (
  echo Downloading required Symbols from Microsoft Symbol server failed.
  echo It may be that you are not connected to the Internet or there is
  echo a problem with the symbol server DLLs.
  echo Please note that you HAVE to fetch the symbols for the loader code to
  echo work properly. This is not a severe installation error, as the loader
  echo tries to download the Symbols too, if not available, so ensure that
  echo you are conneted to the Internet when launching your first DOS 
  echo Application. You may continue now, or abort and try to install again
  echo to ensure proper symbol download.
  pause
)

echo ---------------------------------------------
echo Installing ldntvdm.dll loader code
echo ---------------------------------------------
move /Y %SystemRoot%\System32\ldntvdm.dll %SystemRoot%\System32\ldntvdm.dll.bak >nul
move /Y %SystemRoot%\System32\ldntvdm.dll %SystemRoot%\SysWow64\ldntvdm.dll.bak >nul
xcopy /s /y /g /k /d ldntvdm %SystemRoot%
if errorlevel 1 goto errcpy
reg import reg\appinit.reg
set AppInit=
for /F "skip=2 tokens=2*" %%r in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows" /v AppInit_DLLs') do set AppInit=%AppInit%%%s
echo %AppInit% | findstr /I /C:ldntvdm.dll >nul
if not "%AppInit%"=="" set AppInit=%AppInit% 
if errorlevel 1 reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows" /v AppInit_DLLs /f /d "%AppInit%ldntvdm.dll"
set AppInit=

echo ---------------------------------------------
echo Installing registry patches
echo ---------------------------------------------
reg query HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\VirtualDeviceDrivers /v VDD >nul
if errorlevel 1 reg import reg\vdd.reg
reg import reg\conhost.reg
reg import reg\wow.reg

echo ---------------------------------------------
echo Installing DOS and NTVDM
echo ---------------------------------------------
xcopy /s /y /g /k DOS %SystemRoot%\system32
if errorlevel 1 goto errcpy
xcopy /s /y /g /k DOS %SystemRoot%\SysWow64
if errorlevel 1 goto errcpy

echo.
echo =============================================
echo.
echo Installation successful.
echo You now may want to reboot now to get loader code working.
echo.
goto fini

:errcpy
echo Copying to Windows directory failed. Please ensure that you are
echo running installation procedure as Administrator.
echo INSTALLATION FAILED

:fini
pause