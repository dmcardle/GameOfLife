#!/bin/bash

echo "compiling..."

#g++ -I/Developer/SDKs/MacOSX10.6.sdk/usr/X11/include  -L/Developer/SDKs/MacOSX10.6.sdk/usr/X11/lib/ -lglut -lGLU -lGL -lXmu -lX11 *.c
#gcc -framework GLUT -framework OpenGL -framework Cocoa $1
gcc $1 -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lXi -lXmu -lglut -lGL -lGLU

