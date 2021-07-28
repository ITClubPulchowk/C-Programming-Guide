#!/bin/bash

SOURCEFILES=../flowfield.c
OUTPUTFILE=flowfield.o

if [ ! -d "./bin" ]; then
  mkdir bin
fi

echo -------------------------
echo Building with GCC
echo -------------------------
if ! command -v gcc &> /dev/null
then
  echo GCC Not Found
  echo -------------------------
  exit
fi
pushd bin
gcc $SOURCEFILES -o $OUTPUTFILE -w -Wall -lSDL2main -lSDL2 -lm
popd
echo -------------------------
echo GCC build Complete
echo -------------------------
