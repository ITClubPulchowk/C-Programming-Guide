@echo off

setlocal

set ResourceFileIn=Resource.rc
set ResourceFileOut=Resource.res
set SourceFiles=../../main.c

set MsBuildConfiguration=Debug
set CLFlags=-Od -D_DEBUG

if "%1" neq "optimize" goto DoneConfig
set MsBuildConfiguration=Release
set CLFlags=-O2 -D_RELEASE

echo -------------------------------------
echo Optimize Build configured
echo -------------------------------------
:DoneConfig

echo -------------------------------------

:MSBUILD
where msbuild >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSBUILD
echo Building with MsBuild
call msbuild Mandelbrot-DX11.vcxproj -p:Configuration=%MsBuildConfiguration% -nologo -p:Platform=x64
call msbuild Mandelbrot-DX11.vcxproj -p:Configuration=%MsBuildConfiguration% -nologo -p:Platform=x86
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
call xcopy "Resource.h" bin\MsvcBuild\ /Y
call xcopy "Resource.rc" bin\MsvcBuild\ /Y
call xcopy "Logo.ico" bin\MsvcBuild\ /Y
call xcopy "mandelbrot.hlsl" bin\MsvcBuild\ /Y
pushd bin\MsvcBuild
call rc /fm%ResourceFileOut% %ResourceFileIn%
call cl -nologo -D_CRT_SECURE_NO_WARNINGS -DUNICODE -nologo -Zi -EHsc %CLFlags% %SourceFiles% %ResourceFileOut% -FeMandelbrot-DX11.exe
popd
echo -------------------------------------
goto Finished
:SkipMSVC
echo Msvc not found. Skipping build with Msvc
echo -------------------------------------

:Finished
