@echo off
cls

Rem Build
if not exist Build\Release mkdir Build\Release
cd Build\Release
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
C:\Users\gerku\Documents\mingw64\bin\mingw32-make.exe
xcopy /y compile_commands.json ..\compile_commands.json

Rem Copy dlls
setlocal
set SFML_BINARYS="C:\Users\gerku\Documents\SFML-2.6.1\bin"
echo n | copy /-y %SFML_BINARYS%\sfml-system-2.dll .
echo n | copy /-y %SFML_BINARYS%\sfml-graphics-2.dll .
echo n | copy /-y %SFML_BINARYS%\sfml-window-2.dll .
endlocal

Rem Lauch
MyProject.exe
cd ../../

