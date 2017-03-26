What is it?
===========
NTVDMx64 is a port of the well-known Windows NTVDM, which is used by windows 
to run DOS-Applications, to 64bit Windows, so that you can run your 
DOS-Applications on 64bit Windows too.
This is a proof-of-concept that it is possible to run the NTVDM on 64bit 
Windows and Microsoft was just too lazy to port it over and have disabled it 
for unknown reasons. More specifically, it's a set of patches to the 
sourcecode of original Microsoft NTVDM from NT4 in order to work on 64bit 
Windows.

Motivation
==========
The first question that may come now is: Why porting NTVDM when DosBox is 
working even better and more accurate in x64 Windows.
There are mainly 2 reasons:

1) DosBox doesn't have a good interaction with the windows console. 
Specifically it's not possible to start Windows Applications from within the 
DOS-programs, redirect their output, and other useful interactions that are 
possible with Windows' own NTVDM. 
Over time my DOS-Applications have grown to interact with various Windows 
Applications and Interfaces and that wouldn't work with DosBox for instance. 

2) Simply as a proof-of-concept that it would technically be possible and to 
encourage Microsoft to maybe take my patches and incorporate them into future
Windows releases in order to bring back proper NTVDM Support on x64 
(still x32 is a lot faster because of V86 mode, but on machines with lots of 
RAM, which may be required to run x64, there may be use for this).

How does it work?
=================
I never thought that it would be possible at all, as NTVDM on Win32 uses V86 
mode of the CPU for fast code execution which isn't available in x64 long 
mode.
However I stumbled upon the leaked Windows NT 4 sourcecode and the guys from 
OpenNT not only released the source but also patched it and included all 
required build tools so that it can be compiled without installing anything 
but their installation package.
The code was a pure goldmine and I was curious how the NTVDM works.

It seems that Microsoft bought the SoftPC solution from Insignia, a company 
that specialised in DOS-Emulators for UNIX-Systems. I found out that it also 
existed on MIPS, PPC and ALPHA Builds of Windows NT 4 which obviously don't 
have a V86 mode available like Intel x86 has. It turned out that Insignia 
shipped SoftPC with a complete emulated C-CPU which also got used by Microsoft
for MIPS, PPC and ALPHA-Builds. 
For V86 mode, Microsoft implemented some facility called "Monitor" which is
the piece of Software that interacts with the V86 part in the NT Kernel.
However they assumed that i386 build = built with Monitor, therefore the
sourcecode had to be patched to distinguish between a i386 build with CCPU
and one with MONITOR. 
But there are a lot of other pitfalls too, which are to overcome. One major
problem is that NTVDM uses calls to the Console and CSRSS Subsystem which
are only used by the NTVDM and are therefore not working out of the box on
x64 systems. Also stuff like Page allocation granularity poses a problem to
EMM memory management on x64 systems. 
And last but not least, the way NTVDM is invoked is rather complicated, 
because it is done by the loader code of kernel32/kernelbase which registers
the NTVDM and the requested application with the console server and the
builds a commandline to execute NTVDM with the appropriate parameters
(when I have more time, I may write how this all works in more detail).
But fortunately as there still are 32bit Versions of Windows, Microsoft still
has the code available in their x64 Windows Versions, as they are compiling
the same codebase as on Win32. This enables us to use these facilities also
on 64bit Windows, but structures have to be converted forward and back
between 32bit and 64bit structures as NTVDM is 32bit code and therefore
runs inside WOW64. WOW64.dll generally has transformations for a lot of 
calls by 32bit applications, but Microsoft didn't implement the ones that are
necessary for NTVDM (as they are just used by this one application).
Additionally they broke some functions on x64 builds simply by introducing
bugs or not properly converting them, i.e. for console graphics.

So all these difficulties require the system to be patched in order to
support the calls of NTVDM and let it load properly with the CCPU.
One solution would be binary patches of the Windows system files, but this
is problematic as these files can change with various upgrades and there is
also a nasty Patchguard in Windows which doesn't allow system files to be
modified. Therefore the way to go here is a DLL that gets loaded into every
process and patched the operating system DLLs in memory right after loading
the application. The patcher also has to replicate between 32bit and 64bit
processes which is another challenging task.
The method that has been chosen by me is creating 2 DLLs, one for 32bit and
one for 64bit (as they required different code for replication and 
modification). These DLLs get loaded into every process as AppInit-DLL, which
is a nice mechanism by Windows that allows DLLs to be injected into every
GUI process.
Unfortunately these DLLs nowadays have a signature enforcement in order to
get loaded, but this can be turned off in registry. As far as I know this
only works if "Secure Boot" is off, which therefore is a requirement for
this all to work (however I don't know what this exactly is, I guess it is
some UEFI-stuff). AppInit-DLLs only get injected to GUI processes by the 
CSRSS, but injection is also required into CLI processes as these are even
more likely to launch a DOS application. Therefore the AppInit-DLL also has
to hook CreateProcess() functions in order to determine if the called process
is a Commandline process and if so, inject the process via remote thread
injection.
Of course this mess could be all avaoided if Microsoft would implement all
the required patches from the AppInit-DLL into their WOW64 and into NTDLL
(which wouldn't hurt anyway). Further technical details will be provided 
later.

With the help of the mentioned AppInit hook loader it is possible to start 
the NTVDM when a user tries to launch a DOS-Application.
The NTVDM.EXE has to reside within SYSTEM32 and SysWow64 directories, as
it gets searched and called from 32- and 64bit appliations. As NTVDM.EXE
is a 32bit application, all its support files need to be in SysWow64 
directory as this is the SYSTEM32 directory seen by 32bit applications.

Futhermore the way how the windows console works changed with Windows 7.
In earlier times, this was all managed by the CSRSS, but nowadays it is done
with the conHost.exe Console host process, which processes the calls to the
console. The calls are transparent for the caller, so this shouldn't be a
problem, but there are 2 different ConHost DLLs that implement 2 versions
of the console server. Only version 1 of the Console supports direct console
I/O by the NTVDM. On Windows 10 for instance, a registry entry forces the
conhost to use Console V2, so this has to be changed to use V1 console so 
that NTVDM can interact with it, but this fortunately is just a matter of
setting a registry key.

Some registry keys also need to be set to show that the NTVDM is present and
can get called. 
So this is all rather complex, but it sort of works (except DPMI-stuff, 
which doesn't seem to work too well or still crash for unknown resons)
with some patches to the operating system. It's enough as a 
proof-of-concept and to use it for most business applications and even some
games.

Cool, where can I get it?
=========================
Redistribution of the compiled NTVDM is probematic, as it is based on leaked
Windows NT 4 sourcecode, which is (C) by Insignia and Microsoft, so I'm
legally not allowed to redistribute the code.
However I published the code and compiled versions of the loader that 
enables you to run the patched NTVDM, as this is my own code.
As the OpenNT project released the sourcecode, copyright issues are up to
them, I can just say that you need the old-src.trunk.r687.20150728.7z
source package released by them and the old-sdk.trunk_r57.20150728.7z
SDK package (Google and web.archive.org are your friends) if you want
to try it out. 
I'm providing patchfiles that patch the sourcecode to compile your own
version of NTVDM which works on x64, publishing patches shouldn't be a 
copyright issue, I hope. And therefore you can try to patch the source 
and compile it yourself.
Of course it would be nice if Microsoft would do that themselves to provide
x64 Windows users a NTVDM again, but I'm not very positive that this 
will happen, they seem to forget about us DOS-users. :-(

Here is a short howto on how to compile this:
1) Prepare your machine according the the OpenNT project build instructions:
   a) Unpack contents of old-src directory from 
      old-src.trunk.r687.20150728.7z to your working directory
      You do not need to extract the .svn folder in it
   b) Create directory nt\public\sdk in your working directory and 
      unpack the contentes of old-sdk directory from 
      old-sdk.trunk_r57.20150728.7z to it.
      You do not need to extract the .svn folder in it
   c) Ensure that you don't have w: drive already assigned and then
      setup w: drive to point to the working directory by typing
      SUBST W: .
      inside the working directory and then switch to W:
2) Now extract the ntvdmpatch directory from this source repository
   to the root of W: drive 
3) Run the patch.cmd file inside the ntvdmpatch directory to patch 
   the sourcecode tree accordingly
4) Run zSHtst.cmd to enter build shell where you then type:
   a) bld-ntos.cmd 
      to build base components that are required for building NTVDM.
      errors after build can be ignored
   b) bld.cmd 
      to build now patched NTVDM
      "Build error" in the end isn't a problem as long as ntvdm.exe
      is built.
5) Run mkrelease.bat to create a redistributable package in the
   release\ subdirectory
6) Pack release-Directory as installation package for target machine.

How to install
==============
Now you can install the NTVDM on the destination machine:
Run install.bat so that all files will be installed on 
the target machine and Registry patches will get applied.
Afterwards you may need to reboot the machine to get the AppInit-DLL loaded
into explorer, but it should already get loaded into the next GUI process
you start (if you don't have secure boot enabled which would prevent
proper operation anyway), so if you start a DOS-Application from this 
process, it should also work.

You must also ensure, that the machine is connected to the Internet during 
the first run of a DOS-Application, because the loader code needs to fetch
symbols from the Microsoft Symbol server so that it can call OS internal
functions in order to properly start NTVDM. When the symbols are once 
fetched for the current user, they won't be fetched again, until either
the Temp-folder is cleared or the OS files are upgraded.

Debugging
=========
Name of debugger Master Yoda is.
Compiling frontend in mvdm\softpc.new\debugger you do.
Like Gandalf, debugger master Yoda smokes pipe in \\.\pipe\softpc
Therefore writing REG_SZ Key named PIPE containing
   \\.\pipe\softpc
to HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\WOW\CpuEnv
you need.
After running vdmdebug.exe, you will be enlighted by running vdmdebug.
Do not call Yoda, Yoda will call you.

Known bugs
==========
Currently there are crashes with various DPMI programs like i.e
Borland Turbo C or some DOS extenders, currently I don't know what
is causing this, maybe someone can help me debugging them?
But for simple DOS business applications, it may be enough.

Tested on
=========
Windows 10 x64

You are invited to improve this project to bring back DOS to Windows.
