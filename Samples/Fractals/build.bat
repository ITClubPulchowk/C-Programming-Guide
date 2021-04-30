@echo off

setlocal
pushd ..

if not exist "Libraries" mkdir Libraries
if exist "Libraries/SDL2" goto SkipDownload

pushd Libraries
mkdir SDL2
curl "https://www.libsdl.org/release/SDL2-devel-2.0.14-VC.zip" --output SDL2.zip
tar -zxvf SDL2.zip -C SDL2
del SDL2.zip
ren "SDL2\SDL2-2.0.14\include" "SDL2"
popd

:SkipDownload
popd

set SDL2_Include="../../Libraries/SDL2/SDL2-2.0.14/"
set SDL2_Library="../../Libraries/SDL2/SDL2-2.0.14/lib/x64/"
set SDL2_DLL="..\..\Libraries\SDL2\SDL2-2.0.14\lib\x64\SDL2.dll"

if not exist "bin" mkdir bin

echo -------------------------------------
pushd bin
xcopy %SDL2_DLL% bin\ /Y
popd
echo SDL2 inlude path:  %SDL2_Include%
echo SDL2 library path: %SDL2_Library%

where cl >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
echo Building with MSVC
pushd bin
call cl -I%SDL2_Include% -nologo -Zi -EHsc ../koch-snowflake-fractal.c /link /LIBPATH:%SDL2_Library% SDL2.lib SDL2main.lib Shell32.lib /subsystem:windows
popd
goto Finished

:SkipMSVC

where clang >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipCLANG
echo Building with CLANG
pushd bin
call clang -I%SDL2_Include% -L%SDL2_Library% ../koch-snowflake-fractal.c -o koch-snowflake-fractal.exe -lSDL2main -lSDL2 -lShell32 -Xlinker -subsystem:windows
popd
goto Finished

:SkipCLANG

where gcc >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipGCC
echo Building with GCC
pushd bin
call gcc -I%SDL2_Include% -L%SDL2_Library% ../koch-snowflake-fractal.c -o koch-snowflake-fractal.exe -lSDL2main -lSDL2 -lShell32
popd
goto Finished

:SkipGCC

echo No compiler found, please setup compiler. Instructions: https://github.com/IT-Club-Pulchowk/C-Programming-Guide

:Finished
echo -------------------
