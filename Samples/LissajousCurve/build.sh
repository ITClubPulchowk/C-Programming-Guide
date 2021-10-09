#!/bin/bash

SOURCEFILES=../main.c
OUTPUTFILE=lissajous

if [ ! -d "./bin" ]; then
    mkdir bin
fi

if ! command -v gcc &> /dev/null
then
    echo GCC Not Found
    echo ------------------------------
    exit
fi
pushd bin
gcc $SOURCEFILES -o $OUTPUTFILE -lSDL2main -lSDL2 -lm
popd

