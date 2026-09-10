@echo off
rem ===========================================================================
rem console-chooser.cmd - picks which console host NTVDM (and the rest of the
rem system) runs under, and remembers enough about the pre-install state to put
rem things back on uninstall.
rem
rem INVOCATION
rem   console-chooser.cmd prepare
rem       Back up the pre-install ForceV2 state. MUST run BEFORE ntvdmx64.inf,
rem       which unconditionally sets ForceV2=0 - once that has happened the
rem       original value is gone. Records nothing else and shows no menu, so an
rem       install aborted between prepare and apply leaves a working (V1) NTVDM
rem       and no bookkeeping claiming a choice that was never applied.
rem
rem   console-chooser.cmd apply
rem       Show the menu and apply the choice. MUST run AFTER ntvdmx64.inf,
rem       otherwise the INF's own ForceV2=0 would stomp a V2/Custom choice.
rem
rem   console-chooser.cmd uninstall
rem       Restore the backed-up ForceV2 value and remove OpenConsoleNTVDM if it
rem       was installed. MUST run BEFORE install.bat deletes
rem       HKCU\Software\ldntvdm, which is where the backup lives.
rem
rem   console-chooser.cmd            (no arguments)
rem       Interactive: prepare (if not already done) + apply. Safe to re-run at
rem       any time to change the choice or reset to the original console.
rem
rem PRIVILEGES
rem   Which console a user gets is a PER-USER setting (HKCU), so switching
rem   between them - including opting in to an already-installed
rem   OpenConsoleNTVDM - needs no administrator rights at all. On a shared
rem   machine every user can have their own preference.
rem
rem   Only the machine-wide part of OpenConsoleNTVDM needs admin: the .exe in
rem   System32, this package's INF in %windir%\inf, and the Programs and
rem   Features entry.
rem
rem   So this script never prompts for elevation. It checks whether it already
rem   has it and behaves accordingly:
rem     elevated   - full install/uninstall available (this is how the ntvdmx64
rem                  installer runs it).
rem     unelevated - switches the console for the current user only. Switching
rem                  away from OpenConsoleNTVDM opts this user out but leaves it
rem                  installed for everyone else; the user is told it is still
rem                  there and can be removed from Control Panel.
rem
rem STATE
rem   HKCU\Software\ldntvdm\ConsoleChooser
rem       Choice              REG_SZ    V1 | V2 | Custom  (absent = none applied)
rem       OrigForceV2Present  REG_DWORD 1 if HKCU\Console\ForceV2 existed before
rem                                     we ever touched it, 0 if it did not
rem       OrigForceV2Value    REG_DWORD the original value (if Present=1)
rem ===========================================================================

setlocal EnableDelayedExpansion

rem --- Run 64-bit, and note whether we are elevated ---------------------------
rem 64-bit matters because we drive INF sections that touch
rem HKCU\Software\Classes, which is redirected for 32-bit processes.
if "%ProgramFiles(x86)%" == "" goto StartExec
if not exist %SystemRoot%\Sysnative\cmd.exe goto StartExec
%SystemRoot%\Sysnative\cmd.exe /C "%~f0" %*
goto :fini

:StartExec
rem Two operational modes, decided by whether we already have admin rights - we
rem deliberately do NOT prompt for elevation:
rem
rem   elevated   (run by the ntvdmx64 installer, which is already elevated):
rem              full install/uninstall, including adding and removing
rem              OpenConsoleNTVDM machine-wide.
rem   unelevated (someone ran this .cmd themselves): switches the console for
rem              the current user only. Which console a user gets is a per-user
rem              setting, so this needs no admin rights at all - and on a shared
rem              machine each user can have their own preference.
set "ISADMIN=1"
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' set "ISADMIN=0"

set "CHOOSERKEY=HKCU\Software\ldntvdm\ConsoleChooser"
rem Must match __CLSID_CConsoleHandoff in the terminal fork's
rem src/host/exe/CConsoleHandoff.h (and openconsole-ntvdm.inf's CONSOLE_CLSID).
set "CONSOLE_CLSID={838B3ED2-B549-4155-98B5-CAE18FCA8539}"
set "ZIPURL=https://github.com/leecher1337/terminal/releases/latest/download/OpenConsoleNTVDM.zip"
set "CACHEZIP=%~dp0OpenConsoleNTVDM.zip"
set "EXTRACTDIR=%~dp0OpenConsoleNTVDM"
rem Minimum Windows build our OpenConsole build supports (matches the
rem MinVersion in the terminal repo's CascadiaPackage manifests: 10.0.19041).
set "MINBUILD=19041"

rem --- Windows version detection (same approach install.bat uses) ----------
for /f "tokens=4-5 delims=[.XP " %%i in ('ver') do set "VERSION=%%i.%%j"
set "BUILD=0"
if "%VERSION%"=="10.0" for /f "tokens=6 delims=[.XP " %%i in ('ver') do set "BUILD=%%i"

rem Tier 1: does a V2 console (and therefore ForceV2) exist at all? Only the
rem 10.0 family (Windows 10 and 11 both report 10.0 from "ver") has one.
set "HAVEV2=0"
if "%VERSION%"=="10.0" set "HAVEV2=1"

rem Tier 2: is this build new enough to run our patched OpenConsole?
set "HAVECUSTOM=0"
if "%HAVEV2%"=="1" if %BUILD% GEQ %MINBUILD% set "HAVECUSTOM=1"

rem INTERACTIVE marks the paths where a human is watching, so an error message
rem can be held on screen instead of scrolling away. STANDALONE additionally
rem marks a bare double-click from Explorer, where the window closes the moment
rem we exit - there we pause on the way out whatever happened, so the result is
rem readable at all. Neither is set for the prepare/uninstall calls the
rem ntvdmx64 installer makes, which must never block waiting for a keypress.
set "INTERACTIVE=0"
set "STANDALONE=0"
set "ERRORED=0"

if /I "%1"=="prepare"   goto doprepare
if /I "%1"=="apply"     set "INTERACTIVE=1" & goto doapply
if /I "%1"=="uninstall" goto douninstall

rem No arguments: interactive re-run. prepare is idempotent, so this is safe
rem even long after installation.
set "INTERACTIVE=1"
set "STANDALONE=1"
call :doprepare_inner
goto doapply

rem ===========================================================================
:doprepare
call :doprepare_inner
goto fini

:doprepare_inner
rem Nothing to back up on a system with no V2 console - ForceV2 is meaningless
rem there and ntvdmx64.inf setting it is a harmless no-op.
if "%HAVEV2%"=="0" exit /b 0

rem Idempotent: never overwrite an existing backup with an already-modified
rem current value.
reg query "%CHOOSERKEY%" /v OrigForceV2Present >nul 2>&1
if not errorlevel 1 exit /b 0

call :readforcev2
if "%FV2PRESENT%"=="0" (
  echo [*] No previous ForceV2 setting found - will restore to "not set".
  reg add "%CHOOSERKEY%" /v OrigForceV2Present /t REG_DWORD /d 0 /f >nul
  exit /b 0
)

rem Recovery heuristic: ForceV2 is already 0 and NTVDM is already installed,
rem so this is a reinstall (or a first run of this chooser on top of an older
rem ntvdmx64 that forced V1 without ever recording the original). The current 0
rem is ours, not the user's - assume V2 was the original, so a later uninstall
rem can actually hand the modern console back.
if "%FV2VALUE%"=="0" if exist "%SYSTEMROOT%\syswow64\ntvdm.exe" (
  echo [*] Existing NTVDM install detected with ForceV2=0 - assuming the
  echo     original setting was the modern console.
  reg add "%CHOOSERKEY%" /v OrigForceV2Present /t REG_DWORD /d 1 /f >nul
  reg add "%CHOOSERKEY%" /v OrigForceV2Value /t REG_DWORD /d 1 /f >nul
  exit /b 0
)

echo [*] Saving current ForceV2=%FV2VALUE% for restore on uninstall.
reg add "%CHOOSERKEY%" /v OrigForceV2Present /t REG_DWORD /d 1 /f >nul
reg add "%CHOOSERKEY%" /v OrigForceV2Value /t REG_DWORD /d %FV2VALUE% /f >nul
exit /b 0

rem ===========================================================================
:doapply
if "%HAVEV2%"=="0" (
  rem Windows 7/8.x etc: there is no modern console to choose between. Say so
  rem rather than flashing a window that closes again immediately.
  echo.
  echo [*] This version of Windows has only one console host, so there is
  echo     nothing to choose. NTVDM will use it as-is.
  goto fini
)

call :getchoice
echo.
echo ===========================================================================
echo  Console host for NTVDM
echo ===========================================================================
echo.
echo  [1] Legacy console (recommended)
echo      Forces the old console host system-wide. NTVDM integrates properly
echo      and console interaction works as it should.
echo      Drawback: applications that need the modern console - WSL in
echo      particular - will not work while this is active.
echo.
echo  [2] Keep the modern console
echo      Leaves your console settings alone. Least invasive, and WSL and
echo      friends keep working.
echo      Drawback: NTVDM gets its own separate legacy console window opened
echo      by the loader, which is a noticeably worse experience.
echo.
if "%HAVECUSTOM%"=="1" (
  echo  [3] Install our patched console ^(OpenConsoleNTVDM^)
  echo      Downloads a patched build of Microsoft's open-source OpenConsole
  echo      and registers it as your console host. You keep the modern console
  echo      everywhere AND get working NTVDM graphics, because this build
  echo      hosts the session directly instead of forwarding it to Windows
  echo      Terminal over ConPTY ^(which cannot carry pixel data^).
  echo      Drawback: it is a separate, unsigned third-party build, downloaded
  echo      from GitHub and not shipped with Windows.
  echo.
) else (
  echo  [3] Install our patched console - NOT AVAILABLE on this Windows build
  echo      Requires Windows 10 build %MINBUILD% or newer ^(you have %BUILD%^).
  echo.
)
if not "%CURCHOICE%"=="" (
  call :choicelabel "%CURCHOICE%"
  if "%CHOICESOURCE%"=="detected" (
    echo  Current setting: !CHOICELABEL! - detected from your system
  ) else (
    echo  Current setting: !CHOICELABEL!
  )
)
echo  [K] Keep current setting and do nothing
echo.

rem Capture the exact choice index rather than using "if errorlevel N", which
rem means ">= N" and would match several branches at once.
if "%HAVECUSTOM%"=="1" (
  choice /c 123K /n /m "Your choice [1,2,3,K]: "
  set "PICKLEVEL=!errorlevel!"
) else (
  choice /c 12K /n /m "Your choice [1,2,K]: "
  set "PICKLEVEL=!errorlevel!"
  rem With only two real options K comes back as 3; shift it so both menus
  rem agree that 4 means "keep current".
  if "!PICKLEVEL!"=="3" set "PICKLEVEL=4"
)

set "PICK="
if "%PICKLEVEL%"=="1" set "PICK=1"
if "%PICKLEVEL%"=="2" set "PICK=2"
if "%PICKLEVEL%"=="3" set "PICK=3"
if "%PICKLEVEL%"=="4" set "PICK=K"
if not defined PICK (
  echo [*] No selection made - leaving console settings unchanged.
  goto fini
)
if "%PICK%"=="K" (
  echo [*] Leaving console settings unchanged.
  goto fini
)

set "NEWCHOICE=V1"
if "%PICK%"=="2" set "NEWCHOICE=V2"
if "%PICK%"=="3" set "NEWCHOICE=Custom"

rem Switching away from the patched console: opt this user out, but leave it
rem installed on the machine - it is a machine-wide install other users may be
rem using, and it has its own Programs and Features entry.
if not "%NEWCHOICE%"=="Custom" if "%CURCHOICE%"=="Custom" (
  call :removeopenconsole_user
  if exist "%SystemRoot%\System32\OpenConsoleNTVDM.exe" (
    echo.
    echo     Note: OpenConsoleNTVDM is still installed on this machine and is
    echo     no longer used by your account. To remove it for everyone, use
    echo     "OpenConsoleNTVDM" in Control Panel ^> Programs and Features.
    echo.
  )
)

if "%NEWCHOICE%"=="Custom" (
  if exist "%SystemRoot%\System32\OpenConsoleNTVDM.exe" (
    rem Already installed machine-wide - opting this user in is pure HKCU and
    rem needs no admin rights.
    echo [*] Enabling OpenConsoleNTVDM for your account...
    rundll32.exe advpack.dll,LaunchINFSection openconsole-ntvdm.inf,UserInstall
  ) else if "%ISADMIN%"=="1" (
    call :ensureopenconsole
    if errorlevel 1 (
      echo [!] Could not install OpenConsoleNTVDM - console settings unchanged.
      set "ERRORED=1"
      goto fini
    )
  ) else (
    echo.
    echo [!] OpenConsoleNTVDM is not installed on this machine yet, and
    echo     installing it requires administrator rights.
    echo     Run this script as administrator once to install it - after that
    echo     any user can switch to it without elevation.
    echo.
    echo     Console settings unchanged.
    set "ERRORED=1"
    goto fini
  )
)

if "%NEWCHOICE%"=="V1" (
  call :setforcev2 0
) else (
  call :setforcev2 1
)

rem Record the choice LAST, only once the work above actually succeeded, so the
rem bookkeeping can never claim a state that was not applied.
reg add "%CHOOSERKEY%" /v Choice /t REG_SZ /d "%NEWCHOICE%" /f >nul
call :choicelabel "%NEWCHOICE%"
echo [*] Console set to: %CHOICELABEL%
goto fini

rem ===========================================================================
:douninstall
call :getchoice
rem Remove OpenConsoleNTVDM machine-wide only if WE recorded installing it and
rem we actually have the rights to do so. If it is merely detected as active,
rem the user installed it themselves outside this chooser - it has its own
rem Programs and Features entry, and removing ntvdmx64 is no reason to take it
rem away from them.
if "%CHOICESOURCE%"=="recorded" if "%CURCHOICE%"=="Custom" (
  if "%ISADMIN%"=="1" (
    call :removeopenconsole
  ) else (
    rem Unelevated: at least opt this user back out, and say what is left.
    call :removeopenconsole_user
    echo [*] OpenConsoleNTVDM is still installed on this machine - remove it
    echo     via Control Panel ^> Programs and Features if you no longer want it.
  )
)

reg query "%CHOOSERKEY%" /v OrigForceV2Present >nul 2>&1
if errorlevel 1 (
  rem Never prepared - leave ForceV2 alone rather than guessing.
  echo [*] No saved console setting to restore.
) else (
  set "ORIGPRESENT=0"
  set "ORIGVALUE=1"
  for /f "tokens=3" %%a in ('reg query "%CHOOSERKEY%" /v OrigForceV2Present 2^>nul ^| findstr /i "OrigForceV2Present"') do set /a ORIGPRESENT=%%a
  if "!ORIGPRESENT!"=="0" (
    echo [*] Restoring ForceV2 to "not set".
    reg delete "HKCU\Console" /v ForceV2 /f >nul 2>&1
  ) else (
    for /f "tokens=3" %%a in ('reg query "%CHOOSERKEY%" /v OrigForceV2Value 2^>nul ^| findstr /i "OrigForceV2Value"') do set /a ORIGVALUE=%%a
    echo [*] Restoring ForceV2 to !ORIGVALUE!.
    reg add "HKCU\Console" /v ForceV2 /t REG_DWORD /d !ORIGVALUE! /f >nul
  )
)

reg delete "%CHOOSERKEY%" /f >nul 2>&1
goto fini

rem ===========================================================================
rem Helpers
rem ===========================================================================

rem Reads HKCU\Console\ForceV2 into FV2PRESENT (0/1) and FV2VALUE.
:readforcev2
set "FV2PRESENT=0"
set "FV2VALUE="
for /f "tokens=2,3" %%a in ('reg query "HKCU\Console" /v ForceV2 2^>nul ^| findstr /i "ForceV2"') do (
  if /I "%%a"=="REG_DWORD" (
    set "FV2PRESENT=1"
    set /a FV2VALUE=%%b
  )
)
exit /b 0

rem Determines the current state into CURCHOICE, and sets CHOICESOURCE to
rem "recorded" or "detected".
rem
rem Preference is our own recorded value, but an ntvdmx64 installed before this
rem chooser existed (or an OpenConsoleNTVDM installed by hand) has no record -
rem so fall back to working the state out from the system itself, which lets
rem the menu show something truthful either way.
:getchoice
set "CURCHOICE="
set "CHOICESOURCE=recorded"
for /f "tokens=3" %%a in ('reg query "%CHOOSERKEY%" /v Choice 2^>nul ^| findstr /i "Choice"') do set "CURCHOICE=%%a"
if defined CURCHOICE exit /b 0

set "CHOICESOURCE=detected"

rem Is our console actually the active delegation target? That - not merely
rem whether the .exe is present - is what decides the effective behaviour.
rem (Four percent signs to end up with the real "%%Startup" subkey name.)
set "DELCONSOLE="
for /f "tokens=3" %%a in ('reg query "HKCU\Console\%%%%Startup" /v DelegationConsole 2^>nul ^| findstr /i "DelegationConsole"') do set "DELCONSOLE=%%a"
if /I "%DELCONSOLE%"=="%CONSOLE_CLSID%" (
  set "CURCHOICE=Custom"
  exit /b 0
)

rem ForceV2=0 means the legacy console is being forced. Absent means the
rem Windows default, which is the modern console.
call :readforcev2
if "%FV2PRESENT%"=="1" if "%FV2VALUE%"=="0" (
  set "CURCHOICE=V1"
  exit /b 0
)
set "CURCHOICE=V2"
exit /b 0

rem Maps the internal state token (V1/V2/Custom - what we store in the registry)
rem to wording a user can match against the menu options above. Deliberately
rem free of parentheses: a %%var%% containing ")" gets expanded while a
rem parenthesised block is being parsed and would terminate the block early.
:choicelabel
set "CHOICELABEL=%~1"
if "%~1"=="V1" set "CHOICELABEL=[1] Legacy console"
if "%~1"=="V2" set "CHOICELABEL=[2] Modern console"
if "%~1"=="Custom" set "CHOICELABEL=[3] OpenConsoleNTVDM - our patched console"
exit /b 0

:setforcev2
echo [*] Setting ForceV2=%1
reg add "HKCU\Console" /v ForceV2 /t REG_DWORD /d %1 /f >nul
exit /b 0

rem Downloads (if needed), extracts and installs OpenConsoleNTVDM.
rem Returns errorlevel 1 on failure.
:ensureopenconsole
rem curl and tar are both inbox since Windows 10 1803, comfortably below our
rem %MINBUILD% floor, so no download/extract fallbacks are needed here.
set "HTTPCODE="
if exist "%CACHEZIP%" (
  echo [*] Checking for a newer OpenConsoleNTVDM package...
  for /f %%c in ('curl -L -s -o "%CACHEZIP%.new" -w "%%{http_code}" -z "%CACHEZIP%" "%ZIPURL%" 2^>nul') do set "HTTPCODE=%%c"
) else (
  echo [*] Downloading OpenConsoleNTVDM package...
  for /f %%c in ('curl -L -s -o "%CACHEZIP%.new" -w "%%{http_code}" "%ZIPURL%" 2^>nul') do set "HTTPCODE=%%c"
)

if "!HTTPCODE!"=="304" (
  echo [*] Cached package is still current.
  if exist "%CACHEZIP%.new" del "%CACHEZIP%.new" >nul 2>&1
) else if "!HTTPCODE!"=="200" (
  rem Only replace the cached copy once the new one is known good, so a failed
  rem or truncated download cannot destroy a working cached package.
  if not exist "%CACHEZIP%.new" (
    echo [!] Download reported success but produced no file.
    exit /b 1
  )
  move /y "%CACHEZIP%.new" "%CACHEZIP%" >nul
  if exist "%EXTRACTDIR%" rd /s /q "%EXTRACTDIR%" >nul 2>&1
) else (
  if exist "%CACHEZIP%.new" del "%CACHEZIP%.new" >nul 2>&1
  if exist "%CACHEZIP%" (
    echo [!] Download failed ^(HTTP !HTTPCODE!^) - using the cached package.
  ) else (
    echo [!] Download failed ^(HTTP !HTTPCODE!^).
    echo     URL: %ZIPURL%
    exit /b 1
  )
)

if not exist "%CACHEZIP%" (
  echo [!] No package available.
  exit /b 1
)

if not exist "%EXTRACTDIR%\install-openconsole.cmd" (
  echo [*] Extracting package...
  if exist "%EXTRACTDIR%" rd /s /q "%EXTRACTDIR%" >nul 2>&1
  md "%EXTRACTDIR%" >nul 2>&1
  tar -xf "%CACHEZIP%" -C "%EXTRACTDIR%"
  if errorlevel 1 (
    echo [!] Could not extract "%CACHEZIP%".
    exit /b 1
  )
)

if not exist "%EXTRACTDIR%\install-openconsole.cmd" (
  echo [!] Package does not contain install-openconsole.cmd.
  exit /b 1
)

call "%EXTRACTDIR%\install-openconsole.cmd"
exit /b 0

rem Opts the CURRENT USER out of OpenConsoleNTVDM, leaving it installed on the
rem machine for anyone else who wants it. No admin rights needed - this only
rem clears HKCU. Used when switching consoles, where removing a machine-wide
rem install other users may be relying on would be far too heavy-handed.
:removeopenconsole_user
if exist "%SystemRoot%\inf\openconsole-ntvdm.inf" (
  rundll32.exe advpack.dll,LaunchINFSection openconsole-ntvdm.inf,UserUninstall
) else (
  rem INF is gone; clear the delegation values directly so we cannot leave this
  rem user pointed at a console host that may not be there.
  rem NOTE: four percent signs to end up with the real "%%Startup" subkey name.
  reg delete "HKCU\Console\%%%%Startup" /v DelegationConsole /f >nul 2>&1
  reg delete "HKCU\Console\%%%%Startup" /v DelegationTerminal /f >nul 2>&1
)
exit /b 0

rem Removes OpenConsoleNTVDM from the MACHINE via its registered
rem UninstallString. Only used when ntvdmx64 itself is being uninstalled, and
rem only when elevated. Deliberately self-contained - installation copies the
rem INF into %windir%\inf, so removal does not need the downloaded package and
rem still works after the download cache has been cleaned up.
:removeopenconsole
set "OCUNINST="
for /f "skip=2 tokens=2*" %%r in ('reg query "HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenConsoleNTVDM" /v UninstallString 2^>nul') do set "OCUNINST=%%s"

if defined OCUNINST (
  echo [*] Removing OpenConsoleNTVDM...
  %OCUNINST%
  exit /b 0
)

rem No uninstall entry: either it was never installed, or someone removed it by
rem hand. Clear the delegation values defensively so we cannot leave the system
rem pointing at a console host that is not there.
rem NOTE: the real subkey name is "%%Startup" with two literal percent signs,
rem and batch collapses "%%" to one - so four are needed here to end up with
rem two. Verify in regedit if you ever edit this.
echo [*] No OpenConsoleNTVDM uninstall entry found - clearing any stale
echo     delegation settings.
reg delete "HKCU\Console\%%%%Startup" /v DelegationConsole /f >nul 2>&1
reg delete "HKCU\Console\%%%%Startup" /v DelegationTerminal /f >nul 2>&1
exit /b 0

:fini
rem Single pause point, so an error and a standalone run can never pause twice.
rem Pause when a human is there AND either the window is about to close on them
rem (double-clicked from Explorer) or something went wrong they need to read.
rem Never when the ntvdmx64 installer drives us non-interactively, which would
rem stall the install waiting for a keypress.
set "NEEDPAUSE=0"
if "%STANDALONE%"=="1" set "NEEDPAUSE=1"
if "%INTERACTIVE%"=="1" if "%ERRORED%"=="1" set "NEEDPAUSE=1"
if "%NEEDPAUSE%"=="1" (
  echo.
  pause
)
endlocal
exit /b
