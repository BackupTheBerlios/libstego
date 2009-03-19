
file(GLOB source_files *.c)

include_directories(include/ ../include)



add_library(stego SHARED ${source_files})

set_target_properties(stego PROPERTIES COMPILE_FLAGS -std=c99)
 
install(TARGETS stego ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

install(DIRECTORY ../include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

