cmake_minimum_required(VERSION 3.15)

add_library(
	b2 STATIC	src/application.cpp
				src/assetmanager.cpp
				src/camera.cpp
				src/config.cpp
				src/event.cpp
				src/exception.cpp
				src/game.cpp
				src/gapi.cpp
				src/gearbox.cpp
				src/isosurface.cpp
				src/logger.cpp
				src/main.cpp
				src/physics.cpp
				src/platform.cpp
				src/spinlock.cpp
				src/system.cpp
				src/test_scene.cpp
				src/threadpool.cpp
				src/timer.cpp
)

target_include_directories(
	b2	PRIVATE	${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(
	APPEND	B2_PUBLIC_HEADERS	include/application.hpp
								include/assetmanager.hpp
								include/bytebuffer.hpp
								include/camera.hpp
								include/config.hpp
								include/event.hpp
								include/exception.hpp
								include/game.hpp
								include/gapi.hpp
								include/gearbox.hpp
								include/isosurface.hpp
								include/logger.hpp
								include/physics.hpp
								include/platform.hpp
								include/spinlock.hpp
								include/system.hpp
								include/test_scene.hpp
								include/threadpool.hpp
								include/timer.hpp
)

set_target_properties(b2 PROPERTIES PUBLIC_HEADER "${B2_PUBLIC_HEADERS}")

install(
	TARGETS	b2	DESTINATION					${CMAKE_INSTALL_PREFIX}
				PUBLIC_HEADER	DESTINATION	${CMAKE_INSTALL_PREFIX}/include/b2
)
