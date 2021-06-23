#ifndef __POWERUP_H__
#define __POWERUP_H__

#include <string>
#include "Game/GameObject.h"
#include <glm/glm.hpp>

const glm::vec2 SIZE(60.0f, 20.0f);
const glm::vec2 VELOCITY(0.0f, 150.0f);

enum class PowerUpType : int {
    speed, sticky, passthrough, increase, confuse, chaos
};

class PowerUp : public GameObject
{
public:
    PowerUpType Type;
    float       Duration;
    bool        Activated;

    PowerUp(PowerUpType type, glm::vec3 color, float duration, glm::vec2 postition, Texture2D texture);
};

#endif