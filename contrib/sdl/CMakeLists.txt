cmake_minimum_required(VERSION 3.15)

ExternalProject_Add(
	sdl
	GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
	GIT_TAG			release-2.0.14
	CMAKE_ARGS		"${CONTRIB_CMAKE_ARGS}"
					-DBUILD_SHARED_LIBS=OFF
	UPDATE_COMMAND  ""
)
