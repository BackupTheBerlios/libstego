FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE(FindPkgConfig)

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

include_directories(${LIBXML2_INCLUDE_DIRS})


INCLUDE_DIRECTORIES(../src/include
  ../src/libstego/include ../src/libstegofile/include)

#SET(CMAKE_SWIG_FLAGS "")

SWIG_ADD_MODULE(libstego python libstego.i)
SWIG_LINK_LIBRARIES(libstego ${PYTHON_LIBRARIES})

SWIG_ADD_MODULE(libstegofile python libstegofile.i)
SWIG_LINK_LIBRARIES(libstegofile ${PYTHON_LIBRARIES})

# Fix libpng/setjmp annoyance

ADD_CUSTOM_TARGET(setjmpfix ALL
  ./linkfix.sh
  DEPENDS libstegofilePYTHON_wrap.c)

ADD_DEPENDENCIES(_libstegofile
  setjmpfix)

install(TARGETS _libstego ${include_files}
  LIBRARY DESTINATION lib)

install(TARGETS _libstegofile ${include_files}
  LIBRARY DESTINATION lib)

install(FILES libstego.py libstegofile.py DESTINATION /usr/lib/python/site-packages/libstego)