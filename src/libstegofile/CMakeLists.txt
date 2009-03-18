INCLUDE(FindPkgConfig)


file(GLOB source_files *.c)

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

include_directories(include/ ../include ${LIBXML2_INCLUDE_DIRS})


add_library(stegofile SHARED ${source_files})

set_target_properties(stegofile PROPERTIES COMPILE_FLAGS -std=c99)
 


