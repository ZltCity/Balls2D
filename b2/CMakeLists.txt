cmake_minimum_required(VERSION 3.15)

add_library(
	b2 STATIC	src/platform/eventloop.cpp
				src/platform/io.cpp
				src/platform/rendercontext.cpp
				src/camera.cpp
				src/config.cpp
				src/exception.cpp
				src/game.cpp
				src/gearbox.cpp
				src/gl.cpp
				src/isosurface.cpp
				src/logger.cpp
				src/main.cpp
				src/physics.cpp
				src/threadpool.cpp
				src/timer.cpp
)

target_include_directories(
	b2	PRIVATE	${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(
	TARGETS	b2 ARCHIVE	DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(
	DIRECTORY ${CMAKE_SOURCE_DIR}/b2/include/ DESTINATION	${CMAKE_INSTALL_PREFIX}/include/b2
)
