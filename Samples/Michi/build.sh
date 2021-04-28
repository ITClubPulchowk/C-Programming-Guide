#!/bin/bash

SourceFiles="../../main.c ../../glfw/src/context.c ../../glfw/src/egl_context.c ../../glfw/src/glx_context.c ../../glfw/src/init.c ../../glfw/src/input.c ../../glfw/src/linux_joystick.c ../../glfw/src/monitor.c ../../glfw/src/osmesa_context.c ../../glfw/src/posix_thread.c ../../glfw/src/posix_time.c ../../glfw/src/vulkan.c ../../glfw/src/window.c ../../glfw/src/x11_init.c ../../glfw/src/x11_monitor.c ../../glfw/src/x11_window.c ../../glfw/src/xkb_unicode.c"

CompilerFlags=-O
if [ "$1" == "optimized" ]; then
	CompilerFlags=-O2
	echo --------------------------------------------------
	echo Compiling with Optimizations
fi

mkdir -p bin

echo --------------------------------------------------
which clang &> /dev/null
if [ $? == 0 ]; then
	mkdir -p bin/CLANG
	pushd bin/CLANG &> /dev/null
	echo Compiling with CLANG...
	clang $CompilerFlags -Wno-switch -Wno-pointer-sign -D_GLFW_X11 $SourceFiles -o Michi.out -ldl -lGL -lpthread -lm
	popd
else
	echo Clang compiler not detected. Skipping compiling with Clang.
fi
echo --------------------------------------------------

which gcc &> /dev/null
if [ $? == 0 ]; then
	mkdir -p bin/GCC
	pushd bin/GCC &> /dev/null
	echo Compiling with GCC...
	gcc $CompilerFlags -Wno-switch -Wno-pointer-sign -Wno-unused-result -D_GLFW_X11 $SourceFiles -o Michi.out -ldl -lGL -lpthread -lm
	popd
else
	echo Gcc compiler not detected. Skipping compiling with Gcc.
fi
echo --------------------------------------------------
