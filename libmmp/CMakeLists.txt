set(libmmp_SRC
  bulletdebugdrawer.cpp
  bulletphysics.cpp
  glm_helper.cpp
  interpolation.cpp
  mmdphysics.cpp
  motioncontroller.cpp
  pmx.cpp
  shader.cpp
  texthandle.cpp
  vmd.cpp
  )

if(APPLE)
	set(CMAKE_LDFLAGS "-framework OpenGL")
endif()

add_library(mmp STATIC ${libmmp_SRC})
target_link_libraries(mmp
  ${libmmp_dep}
)
