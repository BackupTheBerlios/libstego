FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE(FindPkgConfig)

INCLUDE(FindPythonLibrary)
PKG_CHECK_MODULES(LIBXML2 REQUIRED libxml-2.0)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIRS})


INCLUDE_DIRECTORIES(../src/include
  ../src/libstego/include ../src/libstegofile/include)

#SET(CMAKE_SWIG_FLAGS "")

SWIG_ADD_MODULE(libstego python libstego.i)
SWIG_LINK_LIBRARIES(libstego ${PYTHON_LIBRARIES} 
  ${CMAKE_SOURCE_DIR}/src/libstego/libstego.so)

SWIG_ADD_MODULE(libstegofile python libstegofile.i)
SWIG_LINK_LIBRARIES(libstegofile ${PYTHON_LIBRARIES} 
  ${CMAKE_SOURCE_DIR}/src/libstegofile/libstegofile.so)


# Fix libpng/setjmp annoyance

ADD_CUSTOM_TARGET(setjmpfix ALL
  ./linkfix.sh
  DEPENDS libstegofilePYTHON_wrap.c)

ADD_DEPENDENCIES(_libstegofile
  setjmpfix)


INSTALL(TARGETS _libstego ${include_files}
  DESTINATION ${PYTHON_SITE_PACKAGES_DIR})

INSTALL(TARGETS _libstegofile ${include_files}
  DESTINATION ${PYTHON_SITE_PACKAGES_DIR})

INSTALL(FILES libstego.py libstegofile.py 
  DESTINATION ${PYTHON_SITE_PACKAGES_DIR})