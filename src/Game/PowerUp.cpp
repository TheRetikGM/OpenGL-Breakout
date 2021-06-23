#include "Game/PowerUp.h"

PowerUp::PowerUp(PowerUpType type, glm::vec3 color, float duration, glm::vec2 postition, Texture2D texture)
    : GameObject(postition, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated()
{

}
