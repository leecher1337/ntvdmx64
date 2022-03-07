For the impatient ones, who don't want to read:
ntvdmpatch\doc\autobuild.txt should be the fastest way to get NTVDMx64 compiled.

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
only works if "Secure Boot" is off OR if Windows is in Testsigning mode, 
which therefore is a requirement for this all to work.
Starting with Windows 11, "Secure Boot" cannot be disabled out of the box,
therefore we use AppCert.DLL approach on Win 11 loader.
Fortunately, the Windows 11 loader can also be used under Windows 10, so
it is now possible to also use ntvdmx64 without Secure Boot.
AppInit-DLLs only get injected to GUI processes by the 
CSRSS, but injection is also required into CLI processes as these are even
more likely to launch a DOS application. Therefore the AppInit-DLL also has
to invade the console server and inject into created console applications
from there (WinEvent hook).
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
So this is all rather complex, but it sort of works with some patches to 
the operating system. It's enough as a proof-of-concept and to use it for 
most business applications and even some games.

Cool, where can I get it?
=========================
Redistribution of the compiled NTVDM is probematic, as it is based on leaked
Windows NT 4 sourcecode, which is (C) by Insignia and Microsoft, so I'm
legally not allowed to redistribute the code.
However I published the code and compiled versions of the loader that 
enables you to run the patched NTVDM, as this is my own code.
As the OpenNT project released the sourcecode, copyright issues are up to
them, I can just say that you need the old-src.trunk.r687.20150728.7z
source package released by them (Google and web.archive.org are your 
friends) if you want to try it out. 
Depending on the build flavour you choose, you also need the appropriate
SDK package.
I'm providing patchfiles that patch the sourcecode to compile your own
version of NTVDM which works on x64, publishing patches shouldn't be a 
copyright issue, I hope. And therefore you can try to patch the source 
and compile it yourself.
Of course it would be nice if Microsoft would do that themselves to provide
x64 Windows users a NTVDM again, but I'm not very positive that this 
will happen, they seem to forget about us DOS-users. :-(

There are various "flavours" of NTVDM that can be built:
 * old-src  [!DO NOT USE!]
   The original NT4 NTVDM. Maybe the easiest version to build, but
   it doesn't have multilanguage and lacks some features like LFN
   This has been the default repository until Feb/2020 where 
   development has been switched to MINNT tree.
   This build method therefore is DEPRECATED in favour of the 
   MINNT build systemand will not receive any future updates!
   This build is left unmaintained and unchecked, so don't complain
   if it doesn't build anymore.
   Please note that you need Windows XP for building.
   Build instructions in: doc\old-src.txt
 * HAXM
   Instead of using the emulated CCPU, it uses HAXM VT-x hardware 
   accelleration (CPU needs to support it), so it is significally 
   faster in textmode.
   But it will probably never support graphics, as it is technically 
   impossible to emulate a real VGA card with sufficient performance on VT-x
   Works with minnt and old-src build.
   Build instructions in: doc\haxm.txt
 * MINNT
   This is the most recent NTVDM source code, it supports multiple
   languages, but is probably harder to build.
   It is planned to continue development on this source tree as of
   Feb/2020
   Build instructions in: doc\minnt.txt

To make building easier, there is an automated build system available.
Please refer to doc\autobuild.txt for details.

In case you want a fully compiled build, I recommend Googling for ntvdmx64

How to install
==============
Now you can install the NTVDM on the destination machine:
Run install.bat so that all files will be installed on 
the target machine and Registry patches will get applied.
Afterwards you may need to reboot the machine to get the AppInit-DLL loaded
into explorer, but it should already get loaded into the next GUI process
you start (as long as it's a loader based on AppInitDLLs which is true for
Windows Versions <10 and Windows 10 with Secure Boot disabled), 
so if you start a DOS-Application from this process, it should also work.

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
Setting empty REG_SZ named TRACE to CpuEnv you must.
After running vdmdebug.exe, you will be enlighted by running vdmdebug.
Do not call Yoda, Yoda will call you.

Known bugs
==========
Currently there are crashes with some DPMI programs like i.e
dpmi.exe, but most programs should work, I hope.

Tested on
=========
Windows Server 2003 x64
Windows 7 x64
Windows 8 x64
Windows Server 2008 x64
Windows 10 x64
Windows 11 x64

You are invited to improve this project to bring back DOS to Windows.


FAQ
===
I want to have a proper soundcard emulation
---------------------------------------------------------------------------
MINNT build supports AdLib soundcard emulation by incorporating code parts
of SoundFX2000 into NTVDM SB20 emulation.

To make it more clear: 
  As MUSIC device, select AdLib
  As SOUND device, select Soundblaster 2.0

But you can also try to use the real

http://www.softsystem.co.uk/products/soundfx.htm

To remove time limit (as it's free nowadays anyway), please
see:
https://github.com/leecher1337/ntvdmx64/issues/40#issuecomment-510697281

But it sometimes causes applications to hang and not react to keyboard
input, so only try it if the NTVDM internal sound emulation isn't good 
enough for your needs.


The PC speaker output is choppy and generally inaccurate, I want my PC
speaker sound back
---------------------------------------------------------------------------
This is not NTVDMx64's fault, it is related to a bad design change in 
Windows starting with Windows 7.
It can be solved by installing a proper PC Speaker driver:
https://www.vogons.org/viewtopic.php?f=46&t=58233


I want to run 16bit Windows applications
---------------------------------------------------------------------------
As ob 05/2020, NTVDMx64 now generally supports WOW32, so your applications 
should run.
However, they currently only work with the CCPU build and we know that the
CCPU is slow, so I still recommend wineVDM:
	https://github.com/otya128/winevdm
The loader tries to detect if winevdm is installed and if so, it gets
precedence over NTVDMx64. If you restore original registry entries, WOW32
will be handled by NTVDMx64 again.
The crucial registry key for the check is:
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\NtVdm64\0OTVDM
If it is present, NTVDMx64 won't load its WOW32 implementation.
Please note, that detection of the key is done upon inital load of ldntvdm.dll
into the process address space, so if you toggle the 0OTVDM, the process where
you are launching your 16 bit application from needs to be restarted.
If unsure, you can reboot your machine after installation/uninstallation of
WineVDM handlers.

To debug wow32, before launching your application:
set WOWTRACE=C:\log.txt
set WOWLOGLVL=16

For more detailled information about debugging, please refer to 
doc\debug-wow.txt


Is there a way to enlarge the graphics window?
---------------------------------------------------------------------------
You can use the EyeStrain parameter for this.
Just execute reg\eyestrain.reg from the release-folder of NTVDMx64 and
as soon as the parameter is set, you can switch between 3 variants of
graphics display (Standard - Big - Huge) by pressing the "Scroll Lock" key.
This only works in graphics mode!

Here is how it works:
The .reg file creates a REG_SZ key named "EyeStrain" under
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\WOW\CpuEnv
which contains the string representatino of a hex value of the VK_
virtual key code that needs to be checked for in order to switch between
magnificaton sizes. The .reg file contains "91" as value, as 0x91 is the
VK_SCROLL.
You can find a list of Virtual keycodes here and change it accordingly:
https://nehe.gamedev.net/article/msdn_virtualkey_codes/15009/

Further explanation how it used to work on MIPS/Alpha builds:
https://github.com/leecher1337/ntvdmx64/issues/95#issuecomment-637202206


I want to use more than 16MB of DPMI memory
---------------------------------------------------------------------------
NTVDM is patched to obey DPMI memory size from .PIF file. However, its size
is limited to maximum of 64MB due to .PIF specification. If you want to
use more, you must set a registry key under:

  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\WOW\CpuEnv

Create a REG_SZ value named DPMIMEM with the requested size of DPMI memory
in KB, i.e. 131070 for 128MB of DPMI memory max.

You can just execute the reg\dpmimem.reg from the release-folder of NTVDMx64
to set 128MB of DPMI mem.


I want to use my mouse in my textmode application and not select text 
---------------------------------------------------------------------------
You forgot to disable QuickEdit mode of the windows console.
See: https://github.com/leecher1337/ntvdmx64/issues/80


When launching a DOS program from a user's console, it doesn't start on
first launch and on second launch, I get System Error 4001h
---------------------------------------------------------------------------
This is due to the fact that the ForceV2 key of the Windows console is on
a per-user basis, not systemwide, so it resides in HKEY_CURRENT_USER\Console
Do you need to set the DWORD ForceV2 key in mentioned path to 0.

You can just execute reg\conhost.reg from the release-folder of NTVDMx64
as the correct user to fix that key.


I want to print to my windows GDI printer
---------------------------------------------------------------------------
Use DOSPR.ZIP, see here: https://github.com/leecher1337/ntvdmx64/issues/161


I have trouble printing to a printer which has been redirected via "net use"
---------------------------------------------------------------------------
You may need to use SMBv1 and disable SMBv2.
See:
https://github.com/leecher1337/ntvdmx64/issues/161
https://docs.microsoft.com/en-us/windows-server/storage/file-server/troubleshoot/detect-enable-and-disable-smbv1-v2-v3

Why doesn't QUAKE work?
---------------------------------------------------------------------------
See: http://www.delorie.com/djgpp/v2faq/faq18_6.html


Are there any documents that describe the inner workings of the NTVDM 
subsystem?
---------------------------------------------------------------------------
Not much documentation available, but this slide may be interesting to you:
https://documents.pub/document/2013-syscan360-wang-yuntvdm.html


For other questions, I recommend looking at the Issue tracker:
https://github.com/leecher1337/ntvdmx64

