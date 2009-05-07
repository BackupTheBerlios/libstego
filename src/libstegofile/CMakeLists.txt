INCLUDE(FindPkgConfig)


file(GLOB source_files *.c)

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
pkg_check_modules(SNDFILE REQUIRED sndfile)

find_package(GIF REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

set(LIBS ${LIBS} 
  ${GIF_LIBRARIES}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${SNDFILE_LIBRARIES}
  ${LIBXML2_LIBRARIES})


include_directories(include/ ../include 
  ${LIBXML2_INCLUDE_DIRS}
  ${GIF_INCUDE_DIRS}
  ${PNG_INCUDE_DIRS}
  ${JPEG_INCUDE_DIRS}
  ${SNDFILE_INCUDE_DIRS})


add_library(stegofile SHARED ${source_files})

target_link_libraries(stegofile ${LIBS})

add_definitions(-std=c99)

 

install(TARGETS stegofile ${include_files}
  LIBRARY DESTINATION lib)

install(DIRECTORY ./include/ DESTINATION include
  FILES_MATCHING PATTERN *.h)

#install(DIRECTORY ../include/ DESTINATION include
#  FILES_MATCHING PATTERN *.h)
