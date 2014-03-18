# Find package module for glfw-legacy(2.x)
#   This module sets variables follows:
#     - GLFW_FOUND
#     - GLFW_LIBRARY_DIR
#     - GLFW_INCLUDE_DIR
#     - GLFW_LIBRARIES
#   Options:
#     - GLFW_DIR
#         Additional search path.
#
# Module Author: Kazunori Kimura
# twitter: http://twitter.com/trairia
# mail: kazunori.abu@gmail.com

find_path(GLFW_INCLUDE_DIR GL/glfw.h
  HINTS ${GLFW_DIR}
  PATH_SUFFIXES include)

find_library(GLFW_LIBRARY
  NAMES glfw
  HINTS ${GLFW_DIR}
  PATH_SUFFIXES lib
  )

string(REGEX REPLACE "\(.*/+\).*" "\\1" GLFW_LIBRARY_DIR ${GLFW_LIBRARY})

set(GLFW_HEADER "${GLFW_INCLUDE_DIR}/GL/glfw.h")
if(GLFW_INCLUDE_DIR AND EXISTS "${GLFW_HEADER}")
  file(STRINGS "${GLFW_HEADER}" GLFW_VERSION_MAJOR_LINE REGEX "^#define[ \t]+GLFW_VERSION_MAJOR[ \t]+[0-9]+$")
  file(STRINGS "${GLFW_HEADER}" GLFW_VERSION_MINOR_LINE REGEX "^#define[ \t]+GLFW_VERSION_MINOR[ \t]+[0-9]+$")
  file(STRINGS "${GLFW_HEADER}" GLFW_VERSION_REV_LINE REGEX   "^#define[ \t]+GLFW_VERSION_REVISION[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_MAJOR "${GLFW_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_MINOR "${GLFW_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_REVISION[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_REV "${GLFW_VERSION_REV_LINE}")
  set(GLFW_VERSION_STRING ${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}.${GLFW_VERSION_REV})
  unset(GLFW_VERSION_MAJOR_LINE)
  unset(GLFW_VERSION_MINOR_LINE)
  unset(GLFW_VERSION_REV_LINE)
  unset(GLFW_VERSION_MAJOR)
  unset(GLFW_VERSION_MINOR)
  unset(GLFW_VERSION_REV)
endif()
  
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW
  REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY_DIR GLFW_LIBRARY
  VERSION_VAR GLFW_VERSION_STRING)