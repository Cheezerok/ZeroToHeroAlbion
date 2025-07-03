@echo off
setlocal enabledelayedexpansion

:: Set Qt and compiler paths
set QTDIR=C:\Qt\Qt5.14.0\5.14.0\mingw73_64
set MINGWDIR=C:\Qt\Qt5.14.0\Tools\mingw730_64
set PATH=%QTDIR%\bin;%MINGWDIR%\bin;%PATH%

echo Checking Qt installation...
if not exist "%QTDIR%" (
    echo ERROR: Qt directory not found at %QTDIR%
    echo Please update QTDIR in this script to match your Qt installation
    pause
    exit /b 1
)

echo Creating build directory...
if not exist "build" mkdir build
cd build

echo Running CMake...
cmake -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="%QTDIR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    ..

if errorlevel 1 (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Release -j 4

if errorlevel 1 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

echo Build completed successfully
cd ..

echo Running deployment script...
call deploy_qt_debug.bat

echo Done!
pause 