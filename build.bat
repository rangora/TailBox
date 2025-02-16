@echo off
setlocal

:: Define build directory
set BUILD_DIR=build

:: Check if build directory exists, then delete it
if exist %BUILD_DIR% (
    echo Deleting existing build directory...
    rmdir /s /q %BUILD_DIR%
)

:: Create a new build directory
echo Creating build directory...
mkdir %BUILD_DIR%

:: Navigate into the build directory
cd %BUILD_DIR%

:: Run CMake and build
echo Running CMake configuration...
cmake .. -A x64

echo Building project...
cmake --build .

:: Return to original directory
cd ..
echo Done.

endlocal