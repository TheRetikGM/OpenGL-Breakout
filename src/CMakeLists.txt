set(ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_INCLUDE "${SRC_DIR}/include")
set(BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(ASSETS_DIR "${ROOT_DIR}/assets")

include_directories(
	"${BINARY_DIR}"
	"${REPO_ROOT}/include"
	"${SRC_INCLUDE}"
)

configure_file(config.h.in config.h @ONLY)

file(GLOB SHADERS
	"shaders/*.frag"
	"shaders/*.vert"
	"shaders/*.geom"
)

add_executable("${PROJECT_NAME}"
	main.cpp
	glad.c
	stb_image.cpp
	stb_image_write.cpp
	include/DebugColors.h
	"${BINARY_DIR}/config.h"
	"${SHADERS}"
)
source_group(TREE ${SRC_DIR} PREFIX "" FILES ${SHADERS})

install(DIRECTORY "${REPO_ROOT}/assets" DESTINATION ./)
install(DIRECTORY "${SRC_DIR}/shaders" DESTINATION src)