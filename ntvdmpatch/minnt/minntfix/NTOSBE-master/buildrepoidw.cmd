@echo off

REM ///////////////////////////////////////////////////////////////////////////
REM  buildrepoidw.cmd
REM ///////////////////////////////////////////////////////////////////////////

pushd %~dp0
setlocal ENABLEDELAYEDEXPANSION

REM //
REM // Verify that the script is running under the sizzle environment.
REM //

if [%NTROOT%] equ [] (
    echo You must run this script under the sizzle environment.
    goto End
)

REM //
REM // Display build information.
REM //

echo Building repository-specific IDW tools from the source tree SDKTOOLS project.
echo.

echo Repository Name            = %NTREPO%
echo SDKTOOLS Project Path      = %NTROOT%\sdktools
echo Repository IDW Path        = %BEREPOIDW%
echo.

REM //
REM // Check for existing IDW path.
REM //

if exist "%BEREPOIDW%" (
    REM Check for the -y switch before attempting to delete the repository IDW directory.
    if not [%1] equ [-y] (
        echo Repository IDW path already exists.
        echo Please specify -y switch if you would like to rebuild the repository-specific IDW.
        
        goto End
    )
    
    REM Delete the existing repository IDW directory.
    rmdir /s /q "%BEREPOIDW%"
)

REM //
REM // Create the repository IDW directory.
REM //

mkdir "%BEREPOIDW%"

REM //
REM // Store the native IDW path.
REM //

set NATIVEBEREPOIDW=%BEREPOIDW%

REM //
REM // Switch the build environment if necessary.
REM //

set PROCESSOR_ARCHITECTURE_LOCASE=%PROCESSOR_ARCHITECTURE%
call :LoCase PROCESSOR_ARCHITECTURE_LOCASE

if [%PROCESSOR_ARCHITECTURE_LOCASE%] equ [amd64] (
    set IDWARCH=x86
)

if not [%IDWARCH%] equ [%NTARCH%] (
    echo Temporarily switching to %IDWARCH% build environment.
    call ntswitch %IDWARCH% chk
)

REM //
REM // Build repository IDW tools.
REM //

call :Build ztools sdktools\ztools
if errorlevel 1 goto Error

call :Build generr sdktools\generr idw\generr.exe generr.exe
if errorlevel 1 goto Error

call :Build genlvl sdktools\genlvl bldtools\genlvl.exe genlvl.exe
if errorlevel 1 goto Error

call :Build gensrv sdktools\gensrv idw\gensrv.exe gensrv.exe
if errorlevel 1 goto Error

call :Build genxx base\tools\genxx idw\genxx.exe genxx.exe
if errorlevel 1 goto Error

call :Build hdivide sdktools\hdivide bldtools\hdivide.exe hdivide.exe
if errorlevel 1 goto Error

call :Build hextract sdktools\hextract bldtools\hextract.exe hextract.exe
if errorlevel 1 goto Error

call :Build hsplit sdktools\hsplit bldtools\hsplit.exe hsplit.exe
if errorlevel 1 goto Error

call :Build munge sdktools\munge idw\munge.exe munge.exe
if errorlevel 1 goto Error

call :Build wcshdr sdktools\wcshdr bldtools\wcshdr.exe wcshdr.exe
if errorlevel 1 goto Error

call :Build wmimofck sdktools\wmimofck mstools\wmimofck.exe wmimofck.exe
if errorlevel 1 goto Error

call :Build zwapi sdktools\zwapi bldtools\zwapi.exe zwapi.exe
if errorlevel 1 goto Error

call :Build zwapi sdktools\zwapi bldtools\zwapi.exe zwapi.exe
if errorlevel 1 goto Error


REM //
REM // leecher1337: Build mvdm tools necessary for build
REM // Also update broken build.exe
REM //
cd %BEROOT%\src\sdktools\build
build -c
if errorlevel 1 (
    echo Build command failed for build.
    goto Error
)
copy /y "%NTTREE%\idw\build.exe" "%NATIVEBEREPOIDW%\build.exe"

cd %NTROOT%\base\mvdm\dos\v86\tools\src\buildidx
nmake
md %BEROOT%\tools\tools16 2>nul
copy /Y ..\..\bin\x86\buildidx.exe %BEROOT%\tools\tools16\


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

endlocal
popd

exit /b 0

REM //
REM // ** Build Function **
REM //
REM // %~1 = Tool name
REM // %~2 = Relative source directory path under %NTROOT%
REM // %~3 = Relative binary path under %NTTREE%
REM // %~4 = Relative binary path under %BEREPOIDW%
REM //

:Build
echo.
echo [%~1]

REM //
REM // Verify that the source directory for the specified tool exists.
REM //

if not exist "%NTROOT%\%~2" (
    echo %~1 does not exist in the local source tree. Skipping %~1 build.
    exit /b
)

cd "%NTROOT%\%~2"

REM //
REM // Perform build.
REM //

build -c
if errorlevel 1 (
    echo Build command failed for %~1.
    exit /b 1
)

REM //
REM // Copy the built tool to the repository idw directory.
REM //

if not [%~3] equ [] (
    copy /y "%NTTREE%\%~3" "%NATIVEBEREPOIDW%\%~4"
    if errorlevel 1 (
        echo Failed to copy %~1 to tools directory.
        exit /b 2
    )
)

exit /b 0

REM //
REM // ** LoCase Function **
REM //
REM // %~1 = Variable name
REM //

:LoCase
set %~1=!%1:A=a!
set %~1=!%1:B=b!
set %~1=!%1:C=c!
set %~1=!%1:D=d!
set %~1=!%1:E=e!
set %~1=!%1:F=f!
set %~1=!%1:G=g!
set %~1=!%1:H=h!
set %~1=!%1:I=i!
set %~1=!%1:J=j!
set %~1=!%1:K=k!
set %~1=!%1:L=l!
set %~1=!%1:M=m!
set %~1=!%1:N=n!
set %~1=!%1:O=o!
set %~1=!%1:P=p!
set %~1=!%1:Q=q!
set %~1=!%1:R=r!
set %~1=!%1:S=s!
set %~1=!%1:T=t!
set %~1=!%1:U=u!
set %~1=!%1:V=v!
set %~1=!%1:W=w!
set %~1=!%1:X=x!
set %~1=!%1:Y=y!
set %~1=!%1:Z=z!

exit /b 0
