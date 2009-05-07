include(FindPkgConfig)

file(GLOB source_files *.c)

pkg_check_modules(GSL REQUIRED gsl)
#find_package(gslcblas REQUIRED)

file(GLOB source_files *.c)
include_directories(include/ ../include 
  ${GSL_INCLUDE_DIRS})

set(LIBS ${LIBS} ${GSL_LIBRARIES})


add_library(stego SHARED ${source_files})
target_link_libraries(stego ${LIBS})
add_definitions(-std=c99)

install(TARGETS stego ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

install(DIRECTORY ../include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

