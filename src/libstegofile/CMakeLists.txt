INCLUDE(FindPkgConfig)


file(GLOB source_files *.c)

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

include_directories(include/ ../include ${LIBXML2_INCLUDE_DIRS})


add_library(stegofile SHARED ${source_files})

add_definitions(-std=c99)

 

install(TARGETS stegofile ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

#install(DIRECTORY ../include/ DESTINATION include
#  FILES_MATCHING PATTERN *.h)
