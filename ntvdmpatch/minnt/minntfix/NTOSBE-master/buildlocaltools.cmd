@echo off

REM ///////////////////////////////////////////////////////////////////////////
REM  buildlocaltools.cmd
REM ///////////////////////////////////////////////////////////////////////////

pushd %~dp0
setlocal

REM //
REM // Verify that the script is not running under the sizzle environment.
REM //

if not [%NTROOT%] equ [] (
    echo You cannot run this script under the sizzle environment.
    echo Please run this script directly from the NT command line.
    goto End
)

REM //
REM // Identify the host processor architecture.
REM //

if not [%1] equ [] (
    set HostProcArch=%1
) else if [%PROCESSOR_ARCHITECTURE%] equ [x86] (
    set HostProcArch=x86
) else if [%PROCESSOR_ARCHITECTURE%] equ [AMD64] (
    set HostProcArch=amd64
) else (
    echo Host processor architecture %PROCESSOR_ARCHITECTURE% is not supported by buildlocaltools.cmd.
    goto Error
)

REM //
REM // Initialise the sizzle environment.
REM //

echo == Initialising the sizzle environment for building NTOSBE tools ... ==
set _NT_TARGET_VERSION=0x0502

call ntenv.cmd NTOSBE %~dp0src %~dp0src.bin %HostProcArch% chk

REM //
REM // Build local tools.
REM //
REM // NOTE: We build tools here one by one in order to help people with little knowledge about
REM //       the Razzle build environment identify potential build issues.
REM //
REM //       If you would like to build the whole project tree as fast (and multithreaded) as
REM //       possible, simply run the "build" command from \src directory.
REM //

echo.
echo == Building local tools ... ==

call bldatool mkmsg sdktools\mkmsg bldtools\mkmsg.exe idw\mkmsg.exe
if errorlevel 1 goto Error

call bldatool nmake sdktools\nmake bldtools\nmake.exe mstools\nmake.exe
if errorlevel 1 goto Error

call bldatool build sdktools\build idw\build.exe idw\build.exe
if errorlevel 1 goto Error

call bldatool debuggers#dbg-common sdktools\debuggers\dbg-common
if errorlevel 1 goto Error

call bldatool debuggers#minidump sdktools\debuggers\minidump
if errorlevel 1 goto Error

call bldatool debuggers#imagehlp#tools sdktools\debuggers\imagehlp\tools
if errorlevel 1 goto Error

call bldatool debuggers#imagehlp#dbghelp sdktools\debuggers\imagehlp\dbghelp
if errorlevel 1 goto Error

call bldatool debuggers#imagehlp#imagehlp sdktools\debuggers\imagehlp\imagehlp
if errorlevel 1 goto Error

call bldatool debuggers#imagehlp#binplace sdktools\debuggers\imagehlp\binplace idw\binplace.exe idw\binplace.exe
if errorlevel 1 goto Error

call bldatool debuggers#symchk#symcommon sdktools\debuggers\symchk\symcommon
if errorlevel 1 goto Error

call bldatool debuggers#symchk#symchk#dll sdktools\debuggers\symchk\symchk\dll dbg\files\bin\symbolcheck.dll idw\symbolcheck.dll
if errorlevel 1 goto Error

call bldatool debuggers#symchk#symchk#exe sdktools\debuggers\symchk\symchk\exe dbg\files\bin\symchk.exe idw\symchk.exe
if errorlevel 1 goto Error

call bldatool ztools sdktools\ztools
if errorlevel 1 goto Error

call bldatool alias sdktools\alias bldtools\alias.exe idw\alias.exe
if errorlevel 1 goto Error

call bldatool pcopy sdktools\pcopy bldtools\pcopy.exe idw\pcopy.exe
if errorlevel 1 goto Error

call bldatool rcdll sdktools\rcdll bldtools\rcdll.dll mstools\rcdll.dll
if errorlevel 1 goto Error

call bldatool rc sdktools\rc bldtools\rc.exe mstools\rc.exe
if errorlevel 1 goto Error

call bldatool mc sdktools\mc bldtools\mc.exe mstools\mc.exe
if errorlevel 1 goto Error

call bldatool touch sdktools\touch bldtools\touch.exe idw\touch.exe
if errorlevel 1 goto Error

call bldatool m4 sdktools\m4 bldtools\m4.exe idw\m4.exe
if errorlevel 1 goto Error

call bldatool tracewpp sdktools\trace\tracewpp bldtools\tracewpp.exe idw\tracewpp.exe
if errorlevel 1 goto Error

call bldatool hextract sdktools\hextract idw\hextract.exe idw\hextract.exe
if errorlevel 1 goto Error

call bldatool hsplit sdktools\hsplit idw\hsplit.exe idw\hsplit.exe
if errorlevel 1 goto Error

call bldatool wcshdr sdktools\wcshdr idw\wcshdr.exe idw\wcshdr.exe
if errorlevel 1 goto Error

call bldatool reloc sdktools\reloc bldtools\reloc.exe tools16\reloc.exe
if errorlevel 1 goto Error

REM //
REM // Clean up temporary binary directory.
REM //

echo.
echo == Cleaning up ... ==

rmdir /s /q "%~dp0src.bin"

REM //
REM // Done.
REM //

echo.
echo Build successfully completed! Done.
goto End

REM //
REM // Error.
REM //

:Error
echo.
echo Error! Build aborted.

REM //
REM // End.
REM //

:End
pause

endlocal
popd

exit /b 0

