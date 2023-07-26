cmake_minimum_required(VERSION 3.15)

project(BlueWater2
	C
	CXX)

include(FetchContent)

# 3rd party
add_subdirectory(contrib/assimp)
add_subdirectory(contrib/format)
add_subdirectory(contrib/glm)
add_subdirectory(contrib/json)
add_subdirectory(contrib/sdl)
#
add_subdirectory(b2-core)
add_subdirectory(b2-app)

set_target_properties(b2-core b2-app
	PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON)
