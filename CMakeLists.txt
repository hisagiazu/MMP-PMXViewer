cmake_minimum_required(VERSION 2.8)

project(MMP-PMXViewer)

add_definitions("-ggdb")
add_definitions("-DDATA_PATH=\"${PROJECT_SOURCE_DIR}/data\"")

add_definitions("-std=c++0x")
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

if(APPLE)
	set(CMAKE_LDFLAGS "-framework OpenGL")
endif()

### set output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

### find dependencies
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(Bullet REQUIRED)
find_package(GLM REQUIRED)
find_package(SOIL REQUIRED)
find_package(Iconv REQUIRED)

### set include path
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${GLFW_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${BULLET_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${SOIL_INCLUDE_DIR}
	${ICONV_INCLUDE_DIR})

if(MSYS)
  set(libmmp_dep iconv)
endif()

set(libmmp_dep ${libmmp_dep}
  ${GLFW_LIBRARY}
  ${GLEW_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${BULLET_LIBRARIES}
  ${ICONV_LIBRARIES})

add_subdirectory(libmmp)
add_subdirectory(src)
#add_subdirectory(demos/PMXViewer)
