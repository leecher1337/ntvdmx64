@echo off
call basepath.cmd
if not exist "%BASEPATH%" (
  echo Invalid NT Base path %BASEPATH%
  goto fini
)
SETLOCAL ENABLEDELAYEDEXPANSION
set BINDIR=%BASEPATH%\..\..\Binaries\x86chk
set LANG=br CHS CHT cs es fr GER hu it JPN KOR nl pl pt ru sv usa
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
  xcopy /Y minnt\release\*.* releases\%%L\
  if exist minnt\release\%%L\*.*  xcopy /Y minnt\release\%%L\*.* releases\%%L\
  md releases\%%L\dos >nul
  for %%I in (_default.pif ANSI.SYS autoexec.nt backup.exe bios1.rom bios4.rom config.nt country.sys edit.com edit.hlp fastopen.exe forcedos.exe gorilla.bas graftabl.com HIMEM.SYS KEYBOARD.SYS key01.sys money.bas mscdexnt.exe nibbles.bas NTDOS.SYS ntdos404.sys ntdos411.sys ntdos412.sys ntdos804.sys NTIO.SYS ntio404.sys ntio411.sys ntio412.sys ntio804.sys ntvdm.exe ntvdmd.dll qbasic.exe qbasic.hlp redir.exe remline.bas share.exe v7vga.rom append.exe COMMAND.COM exe2bin.exe mem.exe debug.exe dosx.exe edlin.exe graphics.com graphics.pro kb16.com loadfix.com nlsfunc.exe setver.exe) do (
    if exist !LANGDIR!\%%I (
      xcopy /Y !LANGDIR!\%%I releases\%%L\dos\
    ) else (
      xcopy /Y !BINDIR!\%%I releases\%%L\dos\
    )
  )
  for %%I in ($disp.sys $ias.sys $prnescp.sys adddrv.exe config.us deldrv.exe disp_win.sys font_win.sys jp.bat key02.sys keyax.sys kkcfunc.sys msime.dic msime.sys msimed.sys msimei.sys msimek.sys msimekey.exe msimelst.exe msimer.dic msimergn.exe msimeset.exe ntfont.sys us.bat edit.exe edit2.hlp hbios.sys hfont.sys qbasic.com qbasic2.exe qbasic2.hlp) do if exist !LANGDIR!\%%I xcopy /Y !LANGDIR!\%%I releases\%%L\dos\
  if exist !LANGDIR!\system32\msherc.com (
    xcopy /Y !LANGDIR!\system32\msherc.com releases\%%L\dos\
  ) else (
    xcopy /Y !BINDIR!\system32\msherc.com releases\%%L\dos\
  )
)
:fini
