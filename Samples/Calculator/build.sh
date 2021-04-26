#!/bin/sh

cc ./src/main.c ./src/regex.c ./src/parser.c ./src/symbol_table.c -I./includes ./src/token.c -lm -o calculator
