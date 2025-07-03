@echo off
set QTDIR=C:\Qt\Qt5.14.0\5.14.0\mingw73_64
set PATH=%QTDIR%\bin;%PATH%

echo Copying Qt DLLs...
copy "%QTDIR%\bin\Qt5Core.dll" "build\release\"
copy "%QTDIR%\bin\Qt5Gui.dll" "build\release\"
copy "%QTDIR%\bin\Qt5Widgets.dll" "build\release\"

echo Copying platform plugin...
if not exist "build\release\platforms" mkdir "build\release\platforms"
copy "%QTDIR%\plugins\platforms\qwindows.dll" "build\release\platforms\"

echo Copying MinGW runtime...
copy "C:\Qt\Qt5.14.0\Tools\mingw730_64\bin\libgcc_s_seh-1.dll" "build\release\"
copy "C:\Qt\Qt5.14.0\Tools\mingw730_64\bin\libstdc++-6.dll" "build\release\"
copy "C:\Qt\Qt5.14.0\Tools\mingw730_64\bin\libwinpthread-1.dll" "build\release\"

echo Done! 