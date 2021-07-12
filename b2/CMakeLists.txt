cmake_minimum_required(VERSION 3.15)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(
	b2 STATIC	src/application.cpp
				src/assetmanager.cpp
				src/camera.cpp
				src/config.cpp
				src/exception.cpp
				src/game.cpp
				src/gapi.cpp
				src/gearbox.cpp
				src/isosurface.cpp
				src/logger.cpp
				src/main.cpp
				src/physics.cpp
				src/spinlock.cpp
				src/system.cpp
				src/test_scene.cpp
				src/threadpool.cpp
				src/timer.cpp
)

install(TARGETS b2 DESTINATION ${CMAKE_INSTALL_PREFIX})
