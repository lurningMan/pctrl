@echo off
setlocal

:: Step 1: Run Doxygen
echo Running Doxygen...
doxygen doc\Doxyfile
if errorlevel 1 (
    echo Doxygen failed.
    exit /b %errorlevel%
)

:: Step 2: Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

:: Step 3: Run CMake configuration
echo Configuring project with CMake...
cd build
cmake .. -G "MinGW Makefiles"
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b %errorlevel%
)

:: Step 4: Build with CMake
echo Building project...
cmake --build .
if errorlevel 1 (
    echo Build failed.
    exit /b %errorlevel%
)

cd ..
echo All steps completed successfully.
endlocal
