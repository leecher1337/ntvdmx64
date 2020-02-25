@echo off
call ..\..\basepath.cmd
mkdir %BASEPATH%\mvdm\softpc.new\base\cvidcnew
xcopy /Y /S src %BASEPATH%\mvdm\softpc.new\base\cvidcnew
for %%I in (access_c.h ccpudefs.inc cpuint_c.h ev_glue.c evid_c.h evidfunc.c evidfunc.h evidgen.h fm_c.h fpu_c.h gdebug.h gdpvar.h gldc_c.h page_c.h prot_c.h segreg_c.h trace_c.h vglfunc.c makefile) do copy /Y %BASEPATH%\mvdm\softpc.new\base\cvidc\%%I %BASEPATH%\mvdm\softpc.new\base\cvidcnew
pushd %BASEPATH%
%PATCHROOT%\util\patch.exe -N -p2 -i %PATCHROOT%\experimental\cvidc-new\cvidc-new.patch
for %%I IN (sources sources.inc) do if exist %BASEPATH%\mvdm\softpc.new\obj.vdm\%%I %PATCHROOT%\util\sed.exe -i "s/\\cvidc\\/\\cvidcnew\\/g" %BASEPATH%\mvdm\softpc.new\obj.vdm\%%I
if "%REPO%"=="minnt" (
  %PATCHROOT%\util\sed.exe -i "s/\$(BASEDIR)\\private/\$(PROJECT_ROOT)/g; s/386_STDCALL/X86_STDCALL/g" %BASEPATH%\mvdm\softpc.new\base\cvidcnew\sources %BASEPATH%\mvdm\softpc.new\base\cvidcnew\test\sources
  %PATCHROOT%\util\patch.exe -N -p2 -i %PATCHROOT%\experimental\cvidc-new\cvidc-minnt.patch
)
popd
