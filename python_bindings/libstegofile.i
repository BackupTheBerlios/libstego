%module libstegofile
%{
#include "../src/include/libstego/types.h"

#include "../src/libstegofile/include/libstego/bmp.h"
#include "../src/libstegofile/include/libstego/gif.h"
#include "../src/libstegofile/include/libstego/jpeg.h"
#include "../src/libstegofile/include/libstego/png.h"
#include "../src/libstegofile/include/libstego/svg.h"
#include "../src/libstegofile/include/libstego/wav.h"
%}
%typedef char uint8_t;
%typedef unsigned int uint32_t;
%typedef int int32_t;
%include "../src/include/libstego/types.h"

%include "../src/libstegofile/include/libstego/bmp.h"
%include "../src/libstegofile/include/libstego/gif.h"
%include "../src/libstegofile/include/libstego/jpeg.h"
%include "../src/libstegofile/include/libstego/png.h"
%include "../src/libstegofile/include/libstego/svg.h"
%include "../src/libstegofile/include/libstego/wav.h"
