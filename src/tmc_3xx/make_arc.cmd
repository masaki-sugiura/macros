@echo off

if "%1"=="" goto ERROR
set TMCVER=%1
set SRCDIR=c:\hidemaru\macros\src\tmc_cur
set GENCMD=cmd /C .\gen_pkg.bat

for %%d in (DVIOUT WinDVI DVI_PS ADOBE_AR DVI2TTY) do %GENCMD% %%d
for %%d in (pTeX2GUI WinTeX) do %GENCMD% %%d tex_mc\tools\wtxccchk.exe tex_mc\tools\mkfmtdb.exe
%GENCMD% pTeX2 tex_mc\tools\execvtex.exe tex_mc\tools\err_hook.exe
cmd /C .\gen_base.bat
goto END

:ERROR
echo You should specify the Version, cf. "330".

:END

