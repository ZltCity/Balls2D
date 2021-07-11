cmake_minimum_required ( VERSION 3.10 )

include(ExternalProject)

ExternalProject_Add ( 
	glm
	GIT_REPOSITORY 	https://github.com/g-truc/glm.git
	CMAKE_ARGS		"${CONTRIB_CMAKE_ARGS}"
					-DGLM_TEST_ENABLE=OFF
	UPDATE_COMMAND	""
	INSTALL_COMMAND ""
)

add_custom_command (
	TARGET	glm	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/glm-prefix/src/glm/glm ${CMAKE_SOURCE_DIR}/install/contrib/include/glm
)
