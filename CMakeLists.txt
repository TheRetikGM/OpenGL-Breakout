cmake_minimum_required(VERSION 3.13.4)

project("Breakout")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("src")

target_link_libraries(${PROJECT_NAME}
	Game
	Shader
	Texture	
	GLAD
)

target_link_directories("${PROJECT_NAME}"
	PRIVATE "${CMAKE_SOURCE_DIR}/lib"
)

if (MSVC)
	set (glfw3Lib debug Debug/glfw3d optimized Release/glfw3)
	set (freeTypeLib debug Debug/freetyped optimized/freetype)
	target_link_libraries("${PROJECT_NAME}"
		opengl32
		${glfw3Lib}
		${freeTypeLib}
		winmm.lib
		legacy_stdio_definitions.lib
	)
	install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
	include(CPack)
elseif (UNIX)
	target_link_libraries("${PROJECT_NAME}"
		glfw		
		dl
		m
		Xi
		pthread
		Xrandr
		X11
		GL
		GLU
		sfml-audio
		sfml-system
		freetype
	)
endif()
