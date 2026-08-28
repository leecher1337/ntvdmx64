X86_SOURCES=x86\monitor.c \
             x86\sas.c \
             x86\spcstubs.c \
             x86\thread.c \
             x86\debug.c \
             x86\int.c \
             x86\whp_api.c

# Windows Hypervisor Platform is loaded dynamically at runtime via
# LoadLibrary / GetProcAddress (see whp_api.c) so that the same binary can
# also start on Windows XP / 7 / pre-1803 builds that do not ship
# WinHvPlatform.dll.  No WHP .lib is needed.
