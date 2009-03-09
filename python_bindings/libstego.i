%module libstego
int lstg_errno;
%{
#include "../src/include/libstego/types.h"

#include "../src/libstego/include/libstego/battlesteg.h"
#include "../src/libstego/include/libstego/cpt.h"
#include "../src/libstego/include/libstego/echo_hiding.h"
#include "../src/libstego/include/libstego/f5.h"
#include "../src/libstego/include/libstego/frirui.h"
#include "../src/libstego/include/libstego/gifshuffle.h"
#include "../src/libstego/include/libstego/lsb.h"
#include "../src/libstego/include/libstego/mb2.h"
#include "../src/libstego/include/libstego/pq.h"
#include "../src/libstego/include/libstego/sort_unsort.h"
#include "../src/libstego/include/libstego/svg_steg.h"
%}
%typedef char uint8_t;
%typedef unsigned int uint32_t;
%typedef int int32_t;
%include typemaps.i
%include cpointer.i
%pointer_functions(char*, charp);
%pointer_functions(unsigned int, intp);
%pointer_functions(char**, charpp);
%apply unsigned int *OUTPUT { unsigned int *capacity };
%include "../src/include/libstego/types.h"
%include "../src/libstego/include/libstego/battlesteg.h"
%include "../src/libstego/include/libstego/cpt.h"
%include "../src/libstego/include/libstego/echo_hiding.h"
%include "../src/libstego/include/libstego/f5.h"
%include "../src/libstego/include/libstego/frirui.h"
%include "../src/libstego/include/libstego/gifshuffle.h"
%include "../src/libstego/include/libstego/lsb.h"
%include "../src/libstego/include/libstego/mb2.h"
%include "../src/libstego/include/libstego/pq.h"
%include "../src/libstego/include/libstego/sort_unsort.h"
%include "../src/libstego/include/libstego/svg_steg.h"
