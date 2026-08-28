@echo off
setlocal EnableExtensions

rem ============================================================
rem  Setup-HyperV-User.cmd
rem
rem  Adds the current user to the local "Hyper-V Administrators"
rem  group so NTVDMx64 can create Windows Hypervisor Platform
rem  partitions without UAC elevation on every 16-bit app launch.
rem
rem  The group name is LOCALIZED -- different on German, French,
rem  Japanese, etc. Windows installs -- so we look it up by its
rem  fixed well-known SID  S-1-5-32-578  via PowerShell, and pass
rem  the resolved name to net.exe.
rem
rem  Run this script ONCE per user account, then log out and back
rem  in.  After that, NTVDMx64 works without further setup.
rem ============================================================

rem -- Self-elevate if not already running with an admin token.
net session >nul 2>&1
if errorlevel 1 (
    echo This script needs administrator rights to modify a local group.
    echo Requesting elevation . . .
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b 0
)

rem -- Resolve the localized group name from the well-known SID.
set "HVGROUP="
for /f "usebackq delims=" %%G in (`powershell -NoProfile -Command "(Get-LocalGroup -SID 'S-1-5-32-578').Name"`) do (
    set "HVGROUP=%%G"
)

if not defined HVGROUP (
    echo.
    echo ERROR: Could not find the local "Hyper-V Administrators" group
    echo        ^(well-known SID S-1-5-32-578^) on this system.
    echo        Make sure the "Hyper-V Hypervisor" and "Windows Hypervisor
    echo        Platform" optional Windows features are installed, then
    echo        reboot and re-run this script.
    echo.
    pause
    exit /b 1
)

echo.
echo Adding user "%USERNAME%" to local group "%HVGROUP%" . . .
net localgroup "%HVGROUP%" "%USERNAME%" /add
set "RC=%ERRORLEVEL%"

echo.
if "%RC%"=="0" (
    echo Done.
    echo.
    echo IMPORTANT: log out of Windows and log back in for the new
    echo group membership to take effect.  After that, NTVDMx64
    echo will use Hyper-V acceleration without UAC prompts.
) else (
    rem  net localgroup returns 2 when the user is already a member,
    rem  with a stderr message; that case is benign.
    echo The "net localgroup" command above returned exit code %RC%.
    echo If the message reads something like "member already exists" /
    echo "Member ist bereits Mitglied" / "membre est deja membre", the
    echo user is already in the group and no further action is needed.
    echo Any other error means the operation actually failed -- check
    echo the message above.
)

echo.
pause
endlocal
