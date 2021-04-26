
@echo off
if not exist "./bin" (md bin)
@echo on

gcc ./src/main.c ./src/parser.c ./src/regex.c ./src/symbol_table.c ./src/token.c -o calculator
