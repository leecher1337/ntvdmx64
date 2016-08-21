if "%_NTDRIVE%"=="" set _NTDRIVE=W:
if "%_NTROOT%"=="" set _NTROOT=\nt

pushd %_NTDRIVE%%_NTROOT%\private
setlocal

call %_NTDRIVE%%_NTROOT%\public\tools\chksizzle.cmd
if errorlevel 1    goto End

cd %_NTROOT%\private

if exist zUPDNTVERP.cmd (
    call zUPDNTVERP.cmd
    if errorlevel 1 goto Error
)

cd %_NTROOT%\private
if exist zUPDRES.cmd (
    call zUPDRES.cmd
    if errorlevel 1 goto Error
)
%_NTDRIVE%

echo Building ntos / first pass
cd %_NTROOT%\private\ntos
build %BldFlags%
cd %_NTROOT%\private
echo.

echo Now building prereq for Win10 kernel32.lib
cd %_NTROOT%\private\lsa
build %BldFlags%
echo.
cd %_NTROOT%\private\windows\winnls
build %BldFlags%
echo.
cd %_NTROOT%\private\windows\screg
build %BldFlags%
echo.
echo Building W10 kernel32.lib
cd %_NTROOT%\private\windows\base
build  %BldFlags%
cd %_NTROOT%\private
echo.

:Error
echo.
echo Build error. Process aborted.
echo.
goto End

:End

endlocal
popd
