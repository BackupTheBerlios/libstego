#!/bin/sh
swig -make_default -python libstego.i
gcc -fPIC -fno-stack-protector -c libstego_wrap.c -I /usr/local/include/python/ -I ../src/include/ -I ../src/libstegofile/include/ -I ../src/libstego/include/
ld -shared ../src/libstego/libstego.so libstego_wrap.o -o _libstego.so
