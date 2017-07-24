cmake_minimum_required(VERSION 3.5)
project(xrepo)

set(CMAKE_CXX_STANDARD 14)

function(find_and_include_libs LIB_NAME)
    find_package(${LIB_NAME} REQUIRED)
    include_directories(${${LIB_NAME}_INCLUDE_DIRS})
endfunction(find_and_include_libs)

function(add_lib LIB_NAME)
    find_and_include_libs(${LIB_NAME})
    set(LIBS ${LIBS} ${LIB_NAME} PARENT_SCOPE)
endfunction(add_lib)

function(add_auto_lib LIB_NAME)
    find_and_include_libs(${LIB_NAME})
    set(LIBS ${LIBS} ${${LIB_NAME}_LIBRARIES} PARENT_SCOPE)
endfunction(add_auto_lib)

include(conanbuildinfo.cmake)
conan_basic_setup()
include_directories(${CONAN_INCLUDE_DIRS})

add_lib(SDL2)
add_lib(Box2D)
add_auto_lib(OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE_FILES src/main.cpp src/application.cpp src/gl_context.cpp src/gl_core_4_5.c src/log.cpp src/window.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS} ${CONAN_LIBS} GL)
