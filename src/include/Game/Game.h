#pragma once

#include <vector>
#include "Game/GameLevel.h"
#include "Game/PowerUp.h"


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
enum class Direction : unsigned int {
    up = 0,
    right = 1,
    down = 2,
    left = 3
};
class Game
{
public:
    GameState       State;
    bool            Keys[1024];
    unsigned int    Width, Height;
    unsigned int    Level;
    std::vector<GameLevel> Levels;
    std::vector<PowerUp> PowerUps;

    Game (unsigned int Width, unsigned int Height);
    ~Game();

    void Init();

    void ProccessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();

    void ResetLevel();
    void ResetPlayer();

    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(float dt);
};