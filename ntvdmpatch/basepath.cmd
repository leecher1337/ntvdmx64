@echo off
rem
rem Determine base path of build root for minnt or old-src
rem
set PATCHROOT=%~dp0
if "%NT_ROOT%"=="" (
  if "%NTROOT%"=="" (
    if exist %~dp0..\nt (
      set BASEPATH=%~dp0..\nt\private
    ) else (
      if exist %~dp0..\minnt (
        set BASEPATH=%~dp0..\minnt\base
      ) else (
        if exist W:\nt (
          set BASEPATH=w:\nt\private
        ) else (
          if exist w:\minnt (
            set BASEPATH=w:\minnt\base
          ) else (
            echo NT BE path not found
            goto fini
          )
        )
      )
    )
  ) else ( 
    set BASEPATH=%NTROOT%\base
  )
) else ( 
  set BASEPATH=%NT_ROOT%\private
)
rem Determine the repository we use
if exist %BASEPATH%\mvdm\softpc.new\obj.vdm\sources.inc (
  set REPO=minnt
) else (
  set REPO=old-src
)
:fini
