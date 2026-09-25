@echo off
setlocal
set "GAME=%~dp0Builds\Windows\PupPop.exe"
if exist "%GAME%" (
    start "" "%GAME%" -windowed -ResX=1440 -ResY=900
) else (
    echo Pup Pop has not been packaged. Run Scripts\Build.ps1 first.
    pause
)
