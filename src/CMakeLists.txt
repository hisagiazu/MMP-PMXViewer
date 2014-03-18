include_directories(${CMAKE_CURRENT_LIST_DIR})

set(pmx_SRC
  dummysound.cpp
  glfw_func_callbacks.cpp
  main.cpp
  pmxvLogger.cpp
  viewer.cpp)

add_executable(pmx ${pmx_SRC})
target_link_libraries(pmx
  mmp
  ${SOIL_LIBRARY}
  ${libmmp_dep})

