@echo on
call basepath.cmd
if not exist "%BASEPATH%" (
  echo Invalid NT Base path %BASEPATH%
  goto fini
)
SETLOCAL ENABLEDELAYEDEXPANSION
if "%SIZ_NTBLD%"=="" (
rem Probe, if checked or free build
if exist %BASEPATH%\..\..\Binaries\x86chk set SIZ_NTBLD=chk
if exist %BASEPATH%\..\..\Binaries\x86fre set SIZ_NTBLD=fre
)
set BINDIR=%BASEPATH%\..\..\Binaries\x86%SIZ_NTBLD%
rem
rem The WOW16 modules hold no localized code, only localized resources, so the
rem build links them once in usa and they are localized here instead, with the
rem original international mechanism: nmake with LANG set copies the usa binary
rem out of INTL_EXE and stamps the localized resources into it with rc16 -t.
rem INTL_SRC is the international source tree, recovered from the shipped
rem localized binaries by src\wow16res.  Stamping needs the 16-bit tools, so it
rem is skipped when they are not on the PATH and the usa binaries are shipped.
rem
if "%INTL_SRC%"=="" set INTL_SRC=%PATCHROOT%src\wow16res\intl
set INTL_EXE=%BINDIR%
rem rc16 is either the plain 16-bit RC 3.11 or the rc16.bat wrapper that runs
rem the MS-DOS Player build of it (see tools16\rc16.bat), so accept both.
set WOW16STAMP=
for %%I in (rc16.exe rc16.bat) do if not "%%~$PATH:I"=="" set WOW16STAMP=1
if "%WOW16STAMP%"=="" echo rc16 is not on the PATH, shipping usa WOW16 resources
set LANG=br CHS CHT cs el es fr GER hu it JPN KOR nl no pl pt ru sv tr usa
if not "%1"=="" set LANG=%1
md releases >nul
echo Copying DOS files
util\settsaware %BINDIR%\ntvdm.exe
for %%L in (!LANG!) do (
  if "%%L"=="usa" (
    set LANGDIR=!BINDIR!
  ) else (
    set LANGDIR=!BINDIR!\%%L
  )
  if not exist "!LANGDIR!\command.com" (
    echo Language %%L is invalid, aborting
    goto fini
  )
  echo.
  echo --- %%L ----
  echo.
  md releases\%%L
  xcopy /Y /S release\*.* releases\%%L\
  if exist ..\readme.txt xcopy /Y ..\readme.txt releases\%%L\
  xcopy /Y minnt\release\*.* releases\%%L\
  if exist minnt\release\%%L\*.*  xcopy /Y minnt\release\%%L\*.* releases\%%L\
  md releases\%%L\dos >nul
  for %%I in (_default.pif ANSI.SYS autoexec.nt backup.exe bios1.rom bios4.rom config.nt country.sys edit.com edit.hlp fastopen.exe forcedos.exe gorilla.bas graftabl.com HIMEM.SYS KEYBOARD.SYS key01.sys money.bas mscdexnt.exe nibbles.bas NTDOS.SYS ntdos404.sys ntdos411.sys ntdos412.sys ntdos804.sys NTIO.SYS ntio404.sys ntio411.sys ntio412.sys ntio804.sys ntvdm.exe ntvdmd.dll qbasic.exe qbasic.hlp redir.exe remline.bas share.exe v7vga.rom append.exe COMMAND.COM exe2bin.exe mem.exe debug.exe dosx.exe edlin.exe graphics.com graphics.pro kb16.com loadfix.com nlsfunc.exe setver.exe vdmdbg.dll vdmredir.dll) do (
    if exist !LANGDIR!\%%I (
      xcopy /Y !LANGDIR!\%%I releases\%%L\dos\
    ) else (
      xcopy /Y !BINDIR!\%%I releases\%%L\dos\
    )
  )
  for %%I in (NetRap.dll xactsrv.dll) do if exist !BINDIR!\%%I xcopy /Y !BINDIR!\%%I releases\%%L\dos\
  if exist !BINDIR!\xactsrv.dll xcopy /Y vdmredir\release\*.* releases\%%L\
  for %%I in (disp.sys ias.sys prnescp.sys) do if exist !LANGDIR!\_%%I copy /Y !LANGDIR!\_%%I releases\%%L\dos\$%%I
  for %%I in ($disp.sys $ias.sys $prnescp.sys adddrv.exe config.us deldrv.exe disp_win.sys font_win.sys jp.bat key02.sys keyax.sys kkcfunc.sys msime.dic msime.sys msimed.sys msimei.sys msimek.sys msimekey.exe msimelst.exe msimer.dic msimergn.exe msimeset.exe ntfont.sys us.bat edit.exe edit2.hlp hbios.sys hfont.sys qbasic.com qbasic2.exe qbasic2.hlp) do if exist !LANGDIR!\%%I xcopy /Y !LANGDIR!\%%I releases\%%L\dos\
  if exist !LANGDIR!\system32\msherc.com (
    xcopy /Y !LANGDIR!\system32\msherc.com releases\%%L\dos\
  ) else (
    xcopy /Y !BINDIR!\system32\msherc.com releases\%%L\dos\
  )
  md releases\%%L\wow32 >nul
  for %%I in (comm.drv commdlg.dll ctl3dv2.dll ddeml.dll drwatson.exe gdi.exe keyboard.drv krnl386.exe lanman.drv lzexpand.dll mciole16.dll mmsystem.dll mmtask.tsk mouse.drv netapi.dll olecli.dll olesvr.dll pmspl.dll rasapi16.dll regedt16.exe shell.dll sound.drv sysedit.exe system.drv timer.drv toolhelp.dll user.exe ver.dll wfwnet.drv vga.drv wifeman.dll win.com win87em.dll winhelp.exe winnls.dll winoldap.mod winsock.dll winspool.exe wow32.dll wowdeb.exe wowexec.exe wowfax.dll wowfaxui.dll) do if exist !BINDIR!\%%I xcopy /Y !BINDIR!\%%I releases\%%L\wow32\

  rem
  rem Localized WOW16 resources.  Only the modules INTL_SRC actually has
  rem resources for are stamped, so a language or a module that has not been
  rem recovered simply keeps the usa binary copied in above.
  rem
  call :stampwow16 %%L commdlg  commdlg.dll  sdk\commdlg
  call :stampwow16 %%L timer    timer.drv    sdk\timer
  call :stampwow16 %%L ddeml    ddeml.dll    sdk\ddeml
  call :stampwow16 %%L sherlock drwatson.exe sdk\sherlock
  call :stampwow16 %%L mmsystem mmsystem.dll mmsystem\mmsystem
  call :stampwow16 %%L ole\client olecli.dll sdk\ole\client retail
  call :stampwow16 %%L wifeman  wifeman.dll  core\wifeman

  rem
  rem Debug symbols and stuff
  rem
  xcopy /Y !BINDIR!\system32\*.sym releases\%%L\Symbols\
  if not "!LANGDIR!"=="!BINDIR!" xcopy /Y !LANGDIR!\system32\*.sym releases\%%L\Symbols\
  if "%SIZ_NTBLD%"=="chk" for /R %BINDIR%\Symbols %%I in (*.pdb) do xcopy /Y %%I releases\%%L\Symbols\
  if "%SIZ_NTBLD%"=="fre" xcopy /Y !BINDIR!\*.pdb releases\%%L\Symbols\
  for /R !LANGDIR! %%I in (*.pdb) do (
    set FN=%%I
    set FN=!FN:_%%J=!
    xcopy /Y %%I releases\Symbols\!FN!
  )
  xcopy /Y !BINDIR!\dbg\files\bin\winxp\*.* releases\%%L\Symbols\
  rem Doesn't get binplaced by default, so take it from compilation dir
  xcopy /Y %BASEPATH%\mvdm\softpc.new\debugger\obj%SIZ_NTBLD%\x86\vdmdebug.exe releases\%%L\Symbols\
)
:fini
goto :eof

rem
rem Stamp one module's localized resources into its usa binary.
rem   %1 language  %2 module directory  %3 binary  %4 sub-path under
rem   INTL_SRC\<lang>  %5 where the makefile leaves the stamped file, when it
rem   is not the module directory
rem
:stampwow16
if "%WOW16STAMP%"=="" goto :eof
if not exist "%INTL_SRC%\%1\%4\." goto :eof
if not exist "%BASEPATH%\mvdm\wow16\%2\makefile" goto :eof
if not exist "%INTL_EXE%\%3" goto :eof
echo Stamping %1 resources into %3
set WOW16DST=%CD%\releases\%1\wow32
set WOW16SUB=%5
if not "%WOW16SUB%"=="" set WOW16SUB=%WOW16SUB%\
pushd %BASEPATH%\mvdm\wow16\%2
call :iclean "%INTL_SRC%\%1\%4"
nmake /nologo LANG=%1
rem a half stamped file must not reach the release: without this the usa
rem binary nmake copied in before rc16 failed would be shipped as localized
if errorlevel 1 (
  echo WARNING: nmake LANG=%1 failed in wow16\%2
  if exist %WOW16SUB%%~n3.%1 del /F /Q %WOW16SUB%%~n3.%1
)
rem rc16 -t takes an empty .res without complaining and returns 0, stripping
rem every resource from the image it stamps, so check the compile produced one
for %%F in (*.res) do if %%~zF EQU 0 (
  echo WARNING: %%F is empty, discarding the stamp of %3 for %1
  if exist %WOW16SUB%%~n3.%1 del /F /Q %WOW16SUB%%~n3.%1
)
if exist %WOW16SUB%%~n3.%1 (
  copy /Y %WOW16SUB%%~n3.%1 "%WOW16DST%\%3"
  del %WOW16SUB%%~n3.%1
) else (
  echo WARNING: could not stamp %3 for %1, keeping the usa binary
)
rem leave no localized copies behind for the next usa build to pick up
call :iclean "%INTL_SRC%\%1\%4"
popd
goto :eof

rem
rem Remove the localized scripts the makefile copied out of RES_DIR into the
rem module directory, and the .res compiled from them, so a later usa build
rem cannot pick them up.  That is what the original iclean targets are for,
rem but only regedit, ole\client and kernel31 ever had one, so the list of
rem names is taken from RES_DIR (%1) instead - which is exactly what was
rem copied in.
rem Doing it before nmake as well means a leftover .rc or .res from an
rem interrupted run cannot make nmake believe the resources are up to date.
rem   %1 the module's RES_DIR under INTL_SRC
rem
:iclean
rem /F because a file checked out of the source tree can be read only
for /f "delims=" %%F in ('dir /b /a-d "%~1" 2^>nul') do if exist "%%F" del /F /Q "%%F"
if exist *.res del /F /Q *.res
goto :eof
