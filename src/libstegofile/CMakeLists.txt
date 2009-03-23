INCLUDE(FindPkgConfig)


file(GLOB source_files *.c)

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

find_package(GIF)
find_package(PNG)
find_package(JPEG)
find_package(Sndfile)

set(LIBS ${LIBS} 
  ${GIF_LIBRARIES}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${Sndfile_LIBRARIES})


include_directories(include/ ../include 
  ${LIBXML2_INCLUDE_DIRS}
  ${GIF_INCUDE_DIRS}
  ${PNG_INCUDE_DIRS}
  ${JPEG_INCUDE_DIRS}
  ${Sndfile_INCUDE_DIRS})


add_library(stegofile SHARED ${source_files})

target_link_libraries(stegofile ${LIBS})

add_definitions(-std=c99)

 

install(TARGETS stegofile ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

#install(DIRECTORY ../include/ DESTINATION include
#  FILES_MATCHING PATTERN *.h)
