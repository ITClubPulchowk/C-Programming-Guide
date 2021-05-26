#!/usr/bin/bash

COMPILER=gcc
FLAGS="-lSDL2 -lm"
FILENAME="koch-snowflake-fractal.c"
BINARY_NAME="bin"

if command -v gcc &> /dev/null 
then
  ${COMPILER} ${FILENAME} -o ${BINARY_NAME} ${FLAGS}
else
  echo "Could not find the compiler ${COMPILER} in your path."
  echo "Aborting the build process..."
  exit 1
fi

 
exit 0
