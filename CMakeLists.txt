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

add_lib(jsoncpp)
add_lib(SDL2)
add_lib(Box2D)
add_lib(glm)
add_auto_lib(OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE_FILES main.cpp application.cpp gl_context.cpp gl_core_4_5.c log.cpp window.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
message(STATUS ${LIBS})
target_link_libraries(${PROJECT_NAME} ${LIBS} GL)
