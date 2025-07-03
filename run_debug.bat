@echo off
cd release
echo Starting application...
echo Current directory: %CD%
echo.
echo Files in directory:
dir
echo.
echo DLL files:
dir *.dll
echo.
echo Platform plugins:
dir platforms\*.dll
echo.
echo Environment PATH:
echo %PATH%
echo.
echo Running application...
AlbionZeroToHero.exe
echo.
echo Exit code: %ERRORLEVEL%
pause 