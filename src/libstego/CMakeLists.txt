
file(GLOB source_files *.c)

include_directories(include/ ../include)


add_library(stego SHARED ${source_files})

set_target_properties(stego PROPERTIES COMPILE_FLAGS -std=c99)
 
