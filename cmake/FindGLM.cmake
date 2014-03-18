# - Locate GLM library
# This module defines
#  GLM_LIBRARY, the name of the library to link against
#  GLM_FOUND
#  GLM_INCLUDE_DIR, where to find glm/glm.hpp
#  GLM_VERSION
#  GLM_VERSION_STRING
# To Adding search path, set GLM_DIR as follows
#    set(GLM_DIR "path/to/glm")
# or launch cmake with -DGLM_DIR="/path/to/GLM_DIR"
#
# author: Kazunori Kimura
# email : kazunori.abu@gmail.com

find_path(GLM_INCLUDE_DIR glm/glm.hpp
  HINTS ${GLM_DIR}
  PATH_SUFFIXES include
  )

set(GLM_SETUP_HEADER "${GLM_INCLUDE_DIR}/glm/core/setup.hpp")
if(GLM_INCLUDE_DIR AND EXISTS "${GLM_SETUP_HEADER}")
  file(STRINGS "${GLM_SETUP_HEADER}" GLM_VERSION_MAJOR_LINE REGEX "^#define[ \t]+GLM_VERSION_MAJOR[ \t]+[0-9]+$")
  file(STRINGS "${GLM_SETUP_HEADER}" GLM_VERSION_MINOR_LINE REGEX "^#define[ \t]+GLM_VERSION_MINOR[ \t]+[0-9]+$")
  file(STRINGS "${GLM_SETUP_HEADER}" GLM_VERSION_PATCH_LINE REGEX   "^#define[ \t]+GLM_VERSION_PATCH[ \t]+[0-9]+$")
  file(STRINGS "${GLM_SETUP_HEADER}" GLM_VERSION_REV_LINE REGEX   "^#define[ \t]+GLM_VERSION_REVISION[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+GLM_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" GLM_VERSION_MAJOR "${GLM_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLM_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" GLM_VERSION_MINOR "${GLM_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLM_VERSION_PATCH[ \t]+([0-9]+)$" "\\1" GLM_VERSION_PATCH "${GLM_VERSION_PATCH_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLM_VERSION_REVISION[ \t]+([0-9]+)$" "\\1" GLM_VERSION_REV "${GLM_VERSION_REV_LINE}")
  set(GLM_VERSION_STRING ${GLM_VERSION_MAJOR}.${GLM_VERSION_MINOR}.${GLM_VERSION_PATCH}.${GLM_VERSION_REV})
  set(GLM_VERSION "${GLM_VERSION_MAJOR}${GLM_VERSION_MINOR}${GLM_VERSION_PATCH}")
  unset(GLM_SETUP_HEADER)
  unset(GLM_VERSION_MAJOR_LINE)
  unset(GLM_VERSION_MINOR_LINE)
  unset(GLM_VERSION_PATCH_LINE)
  unset(GLM_VERSION_REV_LINE)
  unset(GLM_VERSION_MAJOR)
  unset(GLM_VERSION_MINOR)
  unset(GLM_VERSION_PATCH)
  unset(GLM_VERSION_REV)
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLM
  REQUIRED_VARS GLM_INCLUDE_DIR
  VERSION_VAR GLM_VERSION_STRING)
