set(ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_INCLUDE "${SRC_DIR}/include")
set(BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(ASSETS_DIR "${ROOT_DIR}/assets")

include_directories(
	"${BINARY_DIR}"
	"${ROOT_DIR}/include"
	"${SRC_INCLUDE}"
)

add_subdirectory("Game")
add_subdirectory("Shader")
add_subdirectory("Texture")

configure_file(config.h.in config.h @ONLY)

file(GLOB SHADERS
	"shaders/*.frag"
	"shaders/*.vert"
	"shaders/*.geom"
)

add_executable("${PROJECT_NAME}"
	main.cpp
	ResourceManager.cpp
	include/ResourceManager.h
	glad.c
	stb_image.cpp
	stb_image_write.cpp
	include/DebugColors.h
	include/CustomTypes.h
	"${BINARY_DIR}/config.h"
	"${SHADERS}"
)
source_group(TREE ${SRC_DIR} PREFIX "" FILES ${SHADERS})

install(DIRECTORY "${REPO_ROOT}/assets" DESTINATION ./)
install(DIRECTORY "${SRC_DIR}/shaders" DESTINATION src)
