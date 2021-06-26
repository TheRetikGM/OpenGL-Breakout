add_library(Game 
	Game.cpp ${SRC_INCLUDE}/Game/Game.h
	GameObject.cpp ${SRC_INCLUDE}/Game/GameObject.h
	GameLevel.cpp ${SRC_INCLUDE}/Game/GameLevel.h
	BallObject.cpp ${SRC_INCLUDE}/Game/BallObject.h
	ParticleGen.cpp ${SRC_INCLUDE}/Game/ParticleGen.h
	PostProcessor.cpp ${SRC_INCLUDE}/Game/PostProcessor.h
	PowerUp.cpp ${SRC_INCLUDE}/Game/PowerUp.h
)
set (sfml-audio-lib 
		debug 
			Debug/sfml-audio-s-d 
		optimized 
			Release/sfml-audio-s)
set (sfml-system-lib debug Debug/sfml-system-s-d optimized Release/sfml-system-s)
target_link_libraries(Game
	openal32
	flac
	vorbisenc
	vorbisfile
	vorbis
	ogg
	${sfml-audio-lib}
	${sfml-system-lib}
)
add_compile_definitions(SFML_STATIC)
