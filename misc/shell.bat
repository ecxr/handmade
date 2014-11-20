
@echo off
set path=w:\handmade\misc\;%path%
cd w:\handmade
w:
C:\Dev\msvs12\VC\vcvarsall.bat x64
doskey dd=devenv \build\win32_handmade.exe
