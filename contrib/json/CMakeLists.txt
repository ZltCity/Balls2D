cmake_minimum_required(VERSION 3.15)

ExternalProject_Add(
	json
	GIT_REPOSITORY	https://github.com/nlohmann/json.git
	CMAKE_ARGS  	"${CONTRIB_CMAKE_ARGS}"
					-DJSON_BuildTests=OFF
	UPDATE_COMMAND 	""
)
