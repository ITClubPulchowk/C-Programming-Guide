#!/bin/bash
mkdir -p ./bin
# e.g.
INCLUDE="${HOME}/libraries/SDL2-2.0.14/include"
#INCLUDE=""
CC=""
compile_command=""
SRC="main.c"
echo "Checking if gcc exists...."
if gcc -v >/dev/null 2>&1; then
  echo "gcc exists. Compiling with gcc."
  CC="gcc "
else
  echo "gcc not found. Searching for clang instead..."
  if clang -v>/dev/null 2>&1; then
    echo "clang exists. Compiling with clang."
    CC="clang "
  else
    echo "No compiler found. Exiting.."
    exit 0
  fi
fi

if [ $# -eq 0 ]; then
  #Run debug build
  compile_command="$CC -g"
elif [ "$1" = "optimized" ]; then
  compile_command="$CC -O2"
else
  compile_command="$CC -g"
fi

if [ "${INCLUDE}" = "" ]; then
  echo "Warning: INCLUDE directory not set!"
else
  compile_command="${compile_command} -I${INCLUDE}"  
fi

compile_command="${compile_command} -o ./bin/main ${SRC} -lSDL2"
echo $compile_command
if eval $compile_command ; then
  echo "Compilation finished successfully!"
fi

