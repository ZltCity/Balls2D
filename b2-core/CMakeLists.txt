cmake_minimum_required(VERSION 3.15)

add_library(b2-core STATIC
	src/games/particles.cpp
	src/games/shapes.cpp
	src/render/backends/gles3.cpp
	src/render/material.cpp
	src/render/mesh.cpp
	src/render/uniform.cpp
	src/application.cpp
	src/camera.cpp
	src/config.cpp
	src/game.cpp
	src/gearbox.cpp
	src/isosurface.cpp
	src/logger.cpp
	src/main.cpp
	src/physics.cpp
	src/threadpool.cpp
	src/timer.cpp src/render/cache.hpp src/utils.hpp src/utils.cpp)

target_include_directories(b2-core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(b2-core PUBLIC
	assimp
	fmt
	glm
	nlohmann_json
	SDL2-static
	SDL2main
	GL)
