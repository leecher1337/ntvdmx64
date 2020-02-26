REM //
REM // ** Build Function **
REM //
REM // %~1 = Tool name
REM // %~2 = Relative source directory path under %NTROOT%
REM // %~3 = Relative binary path under %NTTREE%
REM // %~4 = Relative binary path under %BETOOLS%
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
REM // Copy the built tool to the tools directory.
REM //

if not [%~3] equ [] (
    copy /y "%NTTREE%\%~3" "%BETOOLS%\%~4"
    if errorlevel 1 (
        echo Failed to copy %~1 to tools directory.
        exit /b 2
    )
)

exit /b 0
