cmake_minimum_required(VERSION 3.15)

ExternalProject_Add(
    format
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    CMAKE_ARGS		"${CONTRIB_CMAKE_ARGS}"
					-DBUILD_SHARED_LIBS=OFF
    UPDATE_COMMAND  ""
)
