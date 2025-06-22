@echo off
setlocal

echo Cleaning project...

:: Remove executables from bin directory
if exist bin (
    echo Deleting executables from bin...
    del /q bin\*.exe
)

:: Remove Doxygen-generated documentation
if exist doc\html (
    echo Deleting Doxygen HTML documentation...
    rmdir /s /q doc\html
)

if exist doc\latex (
    echo Deleting Doxygen LaTeX documentation...
    rmdir /s /q doc\latex
)

:: Remove CMake-generated build files
if exist build (
    echo Deleting build directory...
    rmdir /s /q build
)

echo Clean complete.
endlocal
