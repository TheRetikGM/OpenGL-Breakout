#pragma once
typedef unsigned int uint;

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
class Game
{
    public:
        GameState State;
        bool      Keys[1024];
        uint      Width, Height;

        Game (uint Width, uint Height);
        ~Game();

        void Init();

        void ProccessInput(float dt);
        void Update(float dt);
        void Render();
};