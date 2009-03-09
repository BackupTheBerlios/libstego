#!/bin/sh
swig -make_default -python libstegofile.i
cat prefix libstegofile_wrap.c > libstegofile_wrap2.c
mv libstegofile_wrap2.c libstegofile_wrap.c
gcc -fPIC -fno-stack-protector -c libstegofile_wrap.c -I /usr/local/include/python/ -I /usr/include/libxml2/ -I ../src/include/ -I ../src/libstegofile/include/ -I ../src/libstego/include/
ld -shared ../src/libstegofile/libstegofile.so libstegofile_wrap.o -o _libstegofile.so
