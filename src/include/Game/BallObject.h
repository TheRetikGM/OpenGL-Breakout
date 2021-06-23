#ifndef __BALLOBJECT_H__
#define __BALLOBJECT_H__

#include "Game/GameObject.h"
#include <glm/glm.hpp>

class BallObject : public GameObject
{
public:
	float	Radius;
	float	Stuck;
	bool 	PassThrough;
	bool	Sticky;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // !__BALLOBJECT_H__
