#!/bin/sh

SourceFiles=../../main.c ../../glfw/src/context.c ../../glfw/src/egl_context.c ../../glfw/src/glx_context.c ../../glfw/src/init.c ../../glfw/src/input.c ../../glfw/src/linux_joystick.c ../../glfw/src/monitor.c ../../glfw/src/osmesa_context.c ../../glfw/src/posix_thread.c ../../glfw/src/posix_time.c ../../glfw/src/vulkan.c ../../glfw/src/window.c ../../glfw/src/x11_init.c ../../glfw/src/x11_monitor.c ../../glfw/src/x11_window.c ../../glfw/src/xkb_unicode.c

clang -Od $SourceFiles -D_GLFW_X11 -lx11
