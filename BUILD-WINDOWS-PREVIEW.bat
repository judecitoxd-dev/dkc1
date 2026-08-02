@echo off
setlocal
cd /d "%~dp0"

if not "%~1"=="" if exist "%~1" goto build_with_rom

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\build_windows_preview.ps1" %*
goto build_finished

:build_with_rom
echo Building and certifying with:
echo   %~f1
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\build_windows_preview.ps1" -RomPath "%~f1"

:build_finished
set "DK1_BUILD_EXIT=%ERRORLEVEL%"
if not "%DK1_BUILD_EXIT%"=="0" (
    echo.
    echo DK1 Windows preview build failed with exit code %DK1_BUILD_EXIT%.
    pause
    exit /b %DK1_BUILD_EXIT%
)
echo.
echo DK1 Windows preview build completed successfully.
echo The ZIP and its SHA-256 are in this repository folder.
pause
exit /b 0
