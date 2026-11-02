@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 (
    echo Error initializing MSVC environment.
    exit /b %errorlevel%
)

echo Compiling Castaway Capital with Modern C++20...
cl /EHsc /std:c++20 /W4 /O2 /Fe:CastawayCapital.exe main.cpp GameEngine.cpp Board.cpp Player.cpp Tile.cpp Wallet.cpp

if %errorlevel% equ 0 (
    echo [BUILD SUCCESS] CastawayCapital.exe generated successfully!
) else (
    echo [BUILD FAILED]
)
