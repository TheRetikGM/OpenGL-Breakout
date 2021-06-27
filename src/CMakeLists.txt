set(ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_INCLUDE "${SRC_DIR}/include")
set(BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(ASSETS_DIR "${ROOT_DIR}/assets/")

include_directories(
	"${BINARY_DIR}"
	"${ROOT_DIR}/include"
	"${SRC_INCLUDE}"
)

add_subdirectory("Game")
add_subdirectory("Shader")
add_subdirectory("Texture")
add_library(GLAD glad.c "${ROOT_DIR}/include/glad/glad.h")

configure_file(config.h.in config.h @ONLY)

file(GLOB SHADERS
	"shaders/*.frag"
	"shaders/*.vert"
	"shaders/*.geom"
)

add_executable("${PROJECT_NAME}"
	main.cpp
	ResourceManager.cpp	
	SpriteRenderer.cpp
	TextRenderer.cpp	
	stb_image.cpp
	stb_image_write.cpp
	include/SpriteRenderer.h	
	include/DebugColors.h
	include/ResourceManager.h
	include/TextRenderer.h
	"${BINARY_DIR}/config.h"	
	"${SHADERS}"
)
source_group(TREE ${SRC_DIR} PREFIX "" FILES ${SHADERS})

# Copy openal32.dll to the executable directory
if (MSVC)
	add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/lib/openal32.dll"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)
endif()
