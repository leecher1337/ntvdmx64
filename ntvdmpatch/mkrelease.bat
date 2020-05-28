@echo off
echo Copying DOS files
util\settsaware W:\binaries\nt\ntvdm.exe
md release\dos >nul
for %%I in (ANSI.SYS append.exe autoexec.nt backup.exe bios1.rom bios4.rom cmos.ram COMMAND.COM config.nt country.sys debug.exe dosx.exe edit.com edit.hlp edlin.exe exe2bin.exe fastopen.exe gorilla.bas graphics.com graphics.pro HIMEM.SYS kb16.com KEYBOARD.SYS krnl386.exe loadfix.com mem.exe money.bas mscdexnt.exe ntvdm.exe nibbles.bas NTDOS.SYS NTIO.SYS qbasic.exe qbasic.hlp redir.exe remline.bas setver.exe share.exe v7vga.rom vdmdbg.dll vdmredir.dll) do copy /Y W:\binaries\nt\%%I release\dos\
copy /Y W:\binaries\nt\system32\msherc.com release\dos\
