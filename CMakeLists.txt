cmake_minimum_required (VERSION 2.6)

project (libstego)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(VER_MAJ "0")
SET(VER_MIN "90")
SET(VER_PATCH "1")

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "libstego")
SET(CPACK_PACKAGE_CONTACT "pg-stego@informatik.uni-oldenburg.de")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "90")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local/")
SET(CPACK_PACKAGE_GENERATOR "TGZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "libstego-${VER_MAJ}.${VER_MIN}.${VER_PATCH}")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_IGNORE_FILES 
  ".gitignore"
  ".git/"
  "**/*.o"
  "**/*.os"
  "_CPack_Packages/")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9-4),libjpeg62 (>=6b-14),libgif4 (>=4.1.6-6),libpng12-0 (>=1.2.27-1),libsndfile1 (>=1.0.17-2),libxml2 (>=2.6.3.dfsg-1)")

INCLUDE(CPack)


OPTION(BUILD_LIBSTEGO "build libstego library" ON)
OPTION(BUILD_LIBSTEGOFILE "build libstegofile library" ON)
OPTION(BUILD_BINDINGS "build swig bindings" ON)

IF(BUILD_LIBSTEGO)
  add_subdirectory (src/libstego)
ENDIF(BUILD_LIBSTEGO)

IF(BUILD_LIBSTEGOFILE)
  add_subdirectory(src/libstegofile)
ENDIF(BUILD_LIBSTEGOFILE)

if(BUILD_BINDINGS)              
  add_subdirectory (swig_bindings)
ENDIF(BUILD_BINDINGS)
