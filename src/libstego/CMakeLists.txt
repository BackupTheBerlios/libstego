
file(GLOB source_files *.c)

find_package(gsl REQUIRED)




include_directories(include/ ../include 
  ${gsl_INCLUDE_DIRS})

set(LIBS ${LIBS} ${gsl_LIBRARIES})


add_library(stego SHARED ${source_files})

target_link_libraries(stego ${LIBS})

add_definitions(-std=c99)



install(TARGETS stego ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

install(DIRECTORY ../include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

