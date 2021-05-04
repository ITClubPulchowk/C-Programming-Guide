@echo off

setlocal

set ResourceFileIn=resource.rc
set ResourceFileOut=resource.res
set SourceFiles=../../main.c ../../glfw/src/context.c ../../glfw/src/egl_context.c ../../glfw/src/init.c ../../glfw/src/input.c ../../glfw/src/monitor.c ../../glfw/src/osmesa_context.c ../../glfw/src/vulkan.c ../../glfw/src/wgl_context.c ../../glfw/src/win32_init.c ../../glfw/src/win32_joystick.c ../../glfw/src/win32_monitor.c ../../glfw/src/win32_thread.c ../../glfw/src/win32_time.c ../../glfw/src/win32_window.c ../../glfw/src/window.c ../../glad/src/glad.c
set IncludeDirs=-I"../../glfw/include/" -I"../../glad/include/"

set MsBuildConfiguration=Debug
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

:MSBUILD
where msbuild >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSBUILD
echo Building with MsBuild
call msbuild Mandelbrot-GPU.vcxproj -p:Configuration=%MsBuildConfiguration% -nologo -p:Platform=x64
call msbuild Mandelbrot-GPU.vcxproj -p:Configuration=%MsBuildConfiguration% -nologo -p:Platform=x86
echo -------------------------------------
goto MSVC
:SkipMSBUILD
echo MsBuild not found. Skipping build with msbuild
echo -------------------------------------

if not exist "bin" mkdir bin

:MSVC
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
where rc >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
echo Building with Msvc
if not exist "bin\MsvcBuild" mkdir bin\MsvcBuild
call xcopy "resource.h" bin\MsvcBuild\ /Y
call xcopy "resource.rc" bin\MsvcBuild\ /Y
call xcopy "Logo.ico" bin\MsvcBuild\ /Y
call xcopy "Logo.bmp" bin\MsvcBuild\ /Y
call xcopy "mandelbrot.vert" bin\MsvcBuild\ /Y
call xcopy "mandelbrot.frag" bin\MsvcBuild\ /Y
pushd bin\MsvcBuild
call rc /fm%ResourceFileOut% %ResourceFileIn%
call cl -nologo -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 -nologo -Zi -EHsc %CLFlags% %IncludeDirs% %SourceFiles% %ResourceFileOut% -FeMandelbrot-GPU.exe /link user32.lib gdi32.lib shell32.lib
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
call xcopy "resource.h" bin\ClangBuild /Y
call xcopy "resource.rc" bin\ClangBuild /Y
call xcopy "Logo.ico" bin\ClangBuild /Y
call xcopy "Logo.bmp" bin\ClangBuild /Y
call xcopy "mandelbrot.vert" bin\ClangBuild\ /Y
call xcopy "mandelbrot.frag" bin\ClangBuild\ /Y
pushd bin\ClangBuild
call llvm-rc %ResourceFileIn%
call clang -Wno-switch -Wno-pointer-sign -Wno-enum-conversion -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 %CLANGFlags% %IncludeDirs% %SourceFiles% %ResourceFileOut% -o Mandelbrot-GPU.exe -luser32.lib -lgdi32.lib -lshell32.lib -lopengl32.lib
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
call xcopy "resource.h" bin\GccBuild /Y
call xcopy "resource.rc" bin\GccBuild /Y
call xcopy "Logo.ico" bin\GccBuild /Y
call xcopy "Logo.bmp" bin\GccBuild /Y
call xcopy "mandelbrot.vert" bin\GccBuild\ /Y
call xcopy "mandelbrot.frag" bin\GccBuild\ /Y
pushd bin\GccBuild
call windres -i %ResourceFileIn% -o resource.o
call gcc -Wno-switch -Wno-pointer-sign -Wno-enum-conversion -D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 %GCCFlags% %IncludeDirs% %SourceFiles% resource.o -o Mandelbrot-GPU.exe -luser32 -lgdi32 -lshell32 -lopengl32
popd
echo -------------------------------------
goto Finished
:SkipGCC
echo Gcc not found. Skipping build with Gcc
echo -------------------------------------

:Finished
