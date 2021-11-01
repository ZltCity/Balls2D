cmake_minimum_required(VERSION 3.15)

add_executable(
	bluewater	src/eventloop.cpp
				src/io.cpp
				src/main.cpp
				src/rendercontext.cpp
)

set(
	LIBRARIES	b2
				SDL2
				SDL2main
)

if (UNIX)
	set(
		LIBRARIES	${LIBRARIES}
					dl
					X11
					pthread
					atomic
					GL
	)
endif()

target_include_directories(
	bluewater PUBLIC ${CMAKE_INSTALL_PREFIX}/include
)
target_link_libraries(
	bluewater PUBLIC ${LIBRARIES}
)

install(
	TARGETS	bluewater RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
