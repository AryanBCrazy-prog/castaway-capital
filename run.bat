@echo off
cd /d "%~dp0"
if not exist "CastawayCapital.exe" (
    call build.bat
)
CastawayCapital.exe
pause
