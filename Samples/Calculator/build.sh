#!/bin/sh

if [ ! -d "./bin" ]; then 
    mkdir bin
fi


cc -I./includes/ ./src/main.c ./src/regex.c ./src/parser.c ./src/symbol_table.c -I./includes ./src/token.c -lm -o ./bin/calculator
