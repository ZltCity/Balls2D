cmake_minimum_required(VERSION 3.15)

add_library(b2-core STATIC
	src/application.cpp
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
	src/timer.cpp include/application.hpp src/application.cpp)

target_include_directories(b2-core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(b2-core PUBLIC
	fmt
	glm
	nlohmann_json
	SDL2-static
	SDL2main
	GL)
