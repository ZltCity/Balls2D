cmake_minimum_required(VERSION 3.15)

add_executable(b2-app
	src/desktopapp.cpp
	src/main.cpp)

target_link_libraries(b2-app PRIVATE
	b2-core)

if (UNIX)
	target_link_libraries(b2-app PRIVATE
		atomic)
endif ()

install(TARGETS b2-app
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
