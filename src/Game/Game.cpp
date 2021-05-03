#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "config.h"


SpriteRenderer* Renderer = nullptr;

Game::Game (unsigned int width, unsigned int height) : Width(width), Height(height), State(GAME_ACTIVE)
{

}
Game::~Game()
{
	if (Renderer != nullptr)
		delete(Renderer);
}
void Game::Init()
{
	ResourceManager::LoadShader(SHADERS_DIR "SpriteRender.vert", SHADERS_DIR "SpriteRender.frag", nullptr, "sprite");

	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInt("spriteImage", 0);
	ResourceManager::GetShader("sprite").SetMat4("projection", projection);
	
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ResourceManager::LoadTexture(TEXTURES_DIR "awesomeface.png", true, "face");
}
void Game::ProccessInput(float dt)
{

}
void Game::Update(float dt)
{

}
void Game::Render()
{
	Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}