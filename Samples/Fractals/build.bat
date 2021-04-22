@echo off

setlocal

set SDL2_Include="C:/Libraries/SDL2/"
set SDL2_Library="C:/Libraries/SDL2/lib/x64/"
set SDL2_DLL="C:\Libraries\SDL2\lib\x64\SDL2.dll"

rem Check if the libraries exist in default directory
if exist %SDL2_DLL%  if exist %SDL2_Include% if exist %SDL2_Library% goto StartBuildProcess

echo Searching for SDL2 library
echo Note: The SDL2 include and library directory path should use "/" for directory change and "\" for path to DLL file (Windows eh...)
echo Note: The given SDL2 include path should contain the directory name SDL2 having all the header files for SDL2
echo Note: The given SDL2 library path should contain the files SDL2main.lib and SDL2.lib
echo Note: The given SDL2 DLL path should contain SDL2.dll file
echo Note: SDL2 include directory and library directoy must end with "/" and dll path directory end with "\"
echo Note: When compiling from MSVC, note x86 and x64 bit libraries with the VS Developer Prompt
set /p SDL2_Include="SDL2 Include path: "
set /p SDL2_Library="SDL2 Library path: "
set /p SDL2_DLL_Path="SDL2 DLL path: "
set SDL2_DLL=%SDL2_DLL_Path%"SDL2.dll"

:StartBuildProcess

if not exist "bin" mkdir bin

echo -------------------------------------
xcopy %SDL2_DLL% bin\ /Y
echo SDL2 inlude path:  %SDL2_Include%
echo SDL2 library path: %SDL2_Library%

where cl >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
echo Building with MSVC
pushd bin
call cl -I%SDL2_Include% -nologo -Zi -EHsc ../koch-snowflake-fractal.c /link /LIBPATH:%SDL2_Library% SDL2.lib SDL2main.lib Shell32.lib /subsystem:console
popd
goto Finished

:SkipMSVC

where clang >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipCLANG
echo Building with CLANG
pushd bin
call clang -I%SDL2_Include% -L%SDL2_Library% ../koch-snowflake-fractal.c -o koch-snowflake-fractal.exe -lSDL2.lib -lSDL2main.lib -lShell32.lib
popd
goto Finished

:SkipCLANG

where gcc >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipGCC
echo Building with GCC
pushd bin
call gcc -I%SDL2_Include% -L%SDL2_Library% ../koch-snowflake-fractal.c -o koch-snowflake-fractal.exe -lSDL2.lib -lSDL2main.lib -lShell32.lib
popd
goto Finished

:SkipGCC

echo No compiler found, please setup compiler. Instructions: https://github.com/IT-Club-Pulchowk/C-Programming-Guide

:Finished
echo -------------------
