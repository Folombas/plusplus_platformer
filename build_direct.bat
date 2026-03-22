@echo off
REM Build script using cl.exe with full environment setup
echo Building PlusPlus Platformer...

set VS_PATH=C:\Program Files\Microsoft Visual Studio\18\Community
set VC_PATH=%VS_PATH%\VC
set TOOLS_VERSION=14.50.35717

REM Call vcvarsall to setup environment
call "%VC_PATH%\Auxiliary\Build\vcvars64.bat"

REM Add raylib include and lib
set INCLUDE=%INCLUDE%;%~dp0raylib\include
set LIB=%LIB%;%~dp0raylib\lib

REM Create output directory
if not exist "bin\Release" mkdir "bin\Release"
if not exist "obj\Release" mkdir "obj\Release"

REM Compile source files
echo Compiling source files...
cl /c /O2 /EHsc /std:c++17 /I"include" /I"raylib\include" ^
    src\main.cpp ^
    src\Game.cpp ^
    src\Player.cpp ^
    src\Enemy.cpp ^
    src\Level.cpp ^
    src\ParticleSystem.cpp ^
    src\AudioManager.cpp ^
    src\AchievementManager.cpp ^
    src\Camera2D.cpp ^
    src\TileMap.cpp ^
    /Fo"obj\Release\/"

if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

REM Link
echo Linking...
link /OUT:"bin\Release\PlusPlusPlatformer.exe" ^
    obj\Release\main.obj ^
    obj\Release\Game.obj ^
    obj\Release\Player.obj ^
    obj\Release\Enemy.obj ^
    obj\Release\Level.obj ^
    obj\Release\ParticleSystem.obj ^
    obj\Release\AudioManager.obj ^
    obj\Release\AchievementManager.obj ^
    obj\Release\Camera2D.obj ^
    obj\Release\TileMap.obj ^
    raylib.lib opengl32.lib glu32.lib winmm.lib gdi32.lib shell32.lib comdlg32.lib uuid.lib ole32.lib user32.lib msvcrt.lib ucrt.lib

if errorlevel 1 (
    echo Linking failed!
    exit /b 1
)

REM Copy assets
echo Copying assets...
xcopy /E /I /Y "assets" "bin\Release\assets"

REM Copy raylib DLL
echo Copying raylib.dll...
xcopy /Y "raylib\lib\raylib.dll" "bin\Release\"

echo.
echo ========================================
echo   BUILD COMPLETE!
echo ========================================
echo Executable: bin\Release\PlusPlusPlatformer.exe
pause
