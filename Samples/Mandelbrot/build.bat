@echo off

setlocal
set SourceFiles=../../main.c ../../glfw/src/context.c ../../glfw/src/egl_context.c ../../glfw/src/init.c ../../glfw/src/input.c ../../glfw/src/monitor.c ../../glfw/src/osmesa_context.c ../../glfw/src/vulkan.c ../../glfw/src/wgl_context.c ../../glfw/src/win32_init.c ../../glfw/src/win32_joystick.c ../../glfw/src/win32_monitor.c ../../glfw/src/win32_thread.c ../../glfw/src/win32_time.c ../../glfw/src/win32_window.c ../../glfw/src/window.c

set CLFlags=-Od
set CLANGFlags=-g -gcodeview
set GCCFlags=-O

if "%1" neq "optimize" goto DoneConfig
set MsBuildConfiguration=Release
set CLFlags=-O2
set CLANGFlags=-O2 -gcodeview
set GCCFlags=-O2

echo -------------------------------------
echo Optimize Build configured
echo -------------------------------------
:DoneConfig

echo -------------------------------------

if not exist "bin" mkdir bin

:MSVC
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
where rc >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
echo Building with Msvc
if not exist "bin\MsvcBuild" mkdir bin\MsvcBuild
pushd bin\MsvcBuild
call cl -nologo -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 -nologo -Zi -EHsc %CLFlags% %SourceFiles% -Femandelbrot.exe /link user32.lib gdi32.lib shell32.lib opengl32.lib
popd
echo -------------------------------------
goto CLANG
:SkipMSVC
echo Msvc not found. Skipping build with Msvc
echo -------------------------------------

:CLANG
where clang >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipCLANG
where llvm-rc >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipCLANG
echo Building with CLANG
if not exist "bin\ClangBuild" mkdir bin\ClangBuild
pushd bin\ClangBuild
call llvm-rc %ResourceFileIn%
call clang -Wno-switch -Wno-pointer-sign -Wno-enum-conversion -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 %CLANGFlags% %SourceFiles% -o mandelbrot.exe -luser32.lib -lgdi32.lib -lshell32.lib -lopengl32.lib
popd
echo -------------------------------------
goto GCC
:SkipCLANG
echo Clang not found. Skipping build with Clang
echo -------------------------------------

:GCC
where gcc >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipGCC
where windres >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipGCC
echo Building with GCC
if not exist "bin\GccBuild" mkdir bin\GccBuild
pushd bin\GccBuild
call gcc -Wno-switch -Wno-pointer-sign -Wno-enum-conversion -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 %GCCFlags% %SourceFiles% -o mandelbrot.exe -luser32 -lgdi32 -lshell32 -lopengl32
popd
echo -------------------------------------
goto Finished
:SkipGCC
echo Gcc not found. Skipping build with Gcc
echo -------------------------------------

:Finished