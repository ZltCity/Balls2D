cmake_minimum_required(VERSION 3.15)

project("BlueWater2 Android App")

set(BLUEWATER2_ROOT $ENV{BLUEWATER2_ROOT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

include_directories(
	${BLUEWATER2_ROOT}/install/include
	${BLUEWATER2_ROOT}/install/contrib/include
	${ANDROID_NDK}/sources/android/native_app_glue
)

link_directories(
	${BLUEWATER2_ROOT}/install
)

add_library(
	native_app_glue STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

add_library(
	bluewater	SHARED	src/eventloop.cpp
						src/io.cpp
						src/main.cpp
						src/rendercontext.cpp
						src/sensormanager.cpp
)

target_link_libraries(
	bluewater	b2
				android
				native_app_glue
				GLESv3
				EGL
				log
)
