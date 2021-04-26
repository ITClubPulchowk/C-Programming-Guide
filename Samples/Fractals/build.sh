#!/usr/bin/bash

COMPILER=gcc
FLAGS="-lSDL2 -lm"
FILENAME="koch-snowflake-fractal.c"
BINARY_NAME="bin"

CHECK_COMPILER_IN_PATH=$( which $COMPILER | (grep -o "not found"))
if [ -n "$CHECK_COMPILER_IN_PATH" ] 
then
  echo "Could not find the compiler ${COMPILER} in your path."
  echo "Aborting the build process..."
  exit 1
fi

${COMPILER} ${FILENAME} -o ${BINARY_NAME} ${FLAGS}
 
exit 0
