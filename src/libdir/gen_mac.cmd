@echo off
set HM_SRCDIR=%1
set HM_LIBDIR=c:\hidemaru\macros\src\libdir
set HM_MACDIR=%1
c:\hidemaru\macros\libdir\hmmaclib.cmd gen_mac.awk %2 %3
