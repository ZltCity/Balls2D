cmake_minimum_required(VERSION 3.15)

project(BlueWater2)

include(ExternalProject)

# CMake arguments for external projects.
list(
	APPEND	CONTRIB_CMAKE_ARGS	-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
								-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
								-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
								-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
								-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/contrib
)

if (CMAKE_SYSTEM_NAME MATCHES Android)
	list(
		APPEND	CONTRIB_CMAKE_ARGS	-DCMAKE_ANDROID_NDK=${CMAKE_ANDROID_NDK}
									-DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}
									-DANDROID_PLATFORM=${ANDROID_PLATFORM}
									-DANDROID_ABI=${ANDROID_ABI}
									-DANDROID_STL=${ANDROID_STL}
	)
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/install/contrib/include
	${ANDROID_NDK}/sources/android/native_app_glue
)

add_subdirectory(b2)
add_subdirectory(contrib/format)
add_subdirectory(contrib/glm)
add_subdirectory(contrib/json)

add_dependencies(
	b2	format
		glm
		json
)

if (CMAKE_SYSTEM_NAME MATCHES Android)
	if (CMAKE_HOST_SYSTEM_NAME MATCHES Linux)
		set(GRADLE_SCRIPT run-gradle.sh)
	endif()

#	add_custom_command(
#		TARGET	b2	POST_BUILD
#		COMMAND BLUEWATER2_ROOT=${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/${GRADLE_SCRIPT} ${CMAKE_SOURCE_DIR}/android
#	)
endif()
