# - Locate SOIL library
# This module defines
#  SOIL_LIBRARY, the name of the library to link against
#  SOIL_FOUND
#  SOIL_INCLUDE_DIR, where to find SOIL.h
#  SOIL_LIBRARY_DIR
#
# To Adding search path, set SOIL_DIR as follows
#    set(SOIL_DIR "path/to/soil")
# or launch cmake with -DSOIL_DIR="/path/to/SOIL_DIR".
#
# author: Kazunori Kimura
# email : kazunori.abu@gmail.com
 
find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h
  HINTS ${SOIL_DIR}
  PATH_SUFFIXES include
  )

find_library(SOIL_LIBRARY
  NAMES SOIL
  HINTS ${SOIL_DIR}
  PATH_SUFFIXES lib
  )

string(REGEX REPLACE "\(.*/+\).*" "\\1" SOIL_LIBRARY_DIR ${SOIL_LIBRARY})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOIL
                                  REQUIRED_VARS SOIL_LIBRARY SOIL_INCLUDE_DIR)
