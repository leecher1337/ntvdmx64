@echo off
mkdir W:\nt\private\mvdm\softpc.new\base\cvidcnew
copy /Y src W:\nt\private\mvdm\softpc.new\base\cvidcnew
for %%I in (access_c.h ccpudefs.inc cpuint_c.h ev_glue.c evid_c.h evidfunc.c evidfunc.h evidgen.h fm_c.h fpu_c.h gdebug.h gdpvar.h gldc_c.h page_c.h prot_c.h segreg_c.h trace_c.h vglfunc.c makefile) do copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\%%I W:\nt\private\mvdm\softpc.new\base\cvidcnew
pushd w:\nt
..\ntvdmpatch\util\patch.exe -N -p1 -i ..\ntvdmpatch\experimental\cvidc-new\cvidc-new.patch
popd
