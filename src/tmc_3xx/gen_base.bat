@echo off

set LZHTEMP=c:\usertemp\tmc_$$$.lzh
set MACDIR=c:\hidemaru\macros
set LZHDEST=%MACDIR%\tmc_work\tmc_base_%TMCVER%.lzh

if not exist %SRCDIR%\ver_up.dat goto MKPKG
if not exist %SRCDIR%\tex_mc\ver_up.dat goto MKPKG
goto NEWEST

:MKPKG
if exist %SRCDIR%\ver_up.dat del %SRCDIR%\ver_up.dat
if exist %SRCDIR%\tex_mc\ver_up.dat del %SRCDIR%\tex_mc\ver_up.dat
if exist %LZHTEMP% del %LZHTEMP%
if exist %LZHDEST% del %LZHDEST%
cd /D %MACDIR%
lha a -axil %LZHTEMP% tmc_main.mac tex_mc\*.* tex_mc\docs\*.*
lha a -axil %LZHTEMP% tex_mc\tools\getlfn.ini tex_mc\tools\getlfn.exe
lha a -axil %LZHTEMP% tex_mc\tools\scanfile.exe tex_mc\tools\tmcunins.exe
lha a -axil %LZHTEMP% tex_mc\tools\macserv.exe tex_mc\tools\macsrv13.lzh
move %LZHTEMP% %LZHDEST%
echo THIS FILE IS MADE BY TeX-MC PACKAGE GENERATOR > %SRCDIR%\ver_up.dat
echo VERSION %TMCVER% >> %SRCDIR%\ver_up.dat
echo THIS FILE IS MADE BY TeX-MC PACKAGE GENERATOR > %SRCDIR%\tex_mc\ver_up.dat
echo VERSION %TMCVER% >> %SRCDIR%\tex_mc\ver_up.dat
goto END

:NEWEST
echo Base package is not modified.
echo .

:END

