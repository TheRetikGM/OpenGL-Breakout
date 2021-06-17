add_library(Game 
	Game.cpp ${SRC_INCLUDE}/Game/Game.h
	GameObject.cpp ${SRC_INCLUDE}/Game/GameObject.h
	GameLevel.cpp ${SRC_INCLUDE}/Game/GameLevel.h
	BallObject.cpp ${SRC_INCLUDE}/Game/BallObject.h
	ParticleGen.cpp ${SRC_INCLUDE}/Game/ParticleGen.h
)
