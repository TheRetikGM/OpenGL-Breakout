#pragma once
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
        unsigned int      Width, Height;

        Game (unsigned int Width, unsigned int Height);
        ~Game();

        void Init();

        void ProccessInput(float dt);
        void Update(float dt);
        void Render();
};