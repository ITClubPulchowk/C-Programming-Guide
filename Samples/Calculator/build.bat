@echo off

setlocal

if not exist "./bin" (md bin)

set SourceFiles=./src/main.c ./src/parser.c ./src/regex.c ./src/symbol_table.c ./src/token.c

where gcc >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipGCC
call gcc -I./includes/ %SourceFiles% -lm -o ./bin/calculator
goto Finished
:SkipGCC

where clang >nul 2>nul
IF %ERRORLEVEL% NEQ 0 goto SkipCLANG
echo Building with CLANG
call clang -g -gcodeview -I./includes/ %SourceFiles% -o ./bin/calculator.exe
goto Finished
:SkipCLANG

where cl >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipMSVC
echo Building with MSVC
pushd bin
call cl -I../includes/ -nologo -Zi -EHsc ../src/main.c ../src/parser.c ../src/regex.c ../src/symbol_table.c ../src/token.c /Fecalculator.exe
popd
goto Finished

echo No compiler found, please setup compiler. Instructions: https://github.com/IT-Club-Pulchowk/C-Programming-Guide

:Finished
echo -------------------
