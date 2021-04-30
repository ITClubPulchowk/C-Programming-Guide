#!/bin/bash
mkdir -p ./bin
# Enter your SDL include directory here. SDL.h must exist in this directory and not in any other subdirectory
# e.g. INCLUDE="${HOME}/libraries/SDL2-2.0.14/include"
INCLUDE=""
CC=""
compile_command=""
SRC="main.c"
BIN_NAME="voronoi"

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

while [ ! -f "${INCLUDE}/SDL.h" ]
do
  echo "SDL.h not found in the provided directory"
  read -p "Enter you SDL directory here: " INCLUDE
done
compile_command="${compile_command} -I${INCLUDE} -o ./bin/${BIN_NAME} ${SRC} -lSDL2"
echo $compile_command
if eval $compile_command ; then
  echo "Compilation finished successfully!"
fi

