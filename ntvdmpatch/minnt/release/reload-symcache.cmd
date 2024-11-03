@echo off
rem Ensure that we are run from 64bit prompt
if "%ProgramFiles(x86)%" == "" goto StartExec
if not exist %SystemRoot%\Sysnative\cmd.exe goto StartExec
%SystemRoot%\Sysnative\cmd.exe /C "%~f0" %*
goto :fini

:StartExec
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

echo Forcing rebuild of NTVDMx64 cache....
regedit /s reg\reloadsym.reg
rundll32 %systemroot%\system32\ldntvdm.dll,RebuildSymbolCache
