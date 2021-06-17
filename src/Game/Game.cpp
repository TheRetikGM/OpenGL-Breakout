#include "Game/Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "config.h"
#include <GLFW/glfw3.h>
#include "Game/BallObject.h"
#include "Game/ParticleGen.h"
#include <tuple>
typedef std::tuple<bool, Direction, glm::vec2> Collision;

SpriteRenderer* Renderer = nullptr;

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);	// 500px / 1s
GameObject* Player = nullptr;

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
BallObject* Ball = nullptr;

ParticleGenerator *Particles;

bool initialized = false;

Game::Game (unsigned int width, unsigned int height) : Width(width), Height(height), State(GAME_ACTIVE) {}
Game::~Game()
{
	if (initialized)
	{
		delete Renderer;
		delete Player;
		delete Ball;
	}
}
void Game::Init()
{
	ResourceManager::LoadShader(SHADERS_DIR "SpriteRender.vert", SHADERS_DIR "SpriteRender.frag", nullptr, "sprite");
	ResourceManager::LoadShader(SHADERS_DIR "particle.vert", SHADERS_DIR "particle.frag", nullptr, "particle");

	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInt("spriteImage", 0);
	ResourceManager::GetShader("sprite").SetMat4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetMat4("projection", projection);
	
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	
	// Load textures
	ResourceManager::LoadTexture(TEXTURES_DIR "awesomeface.png", true, "face");
	ResourceManager::LoadTexture(TEXTURES_DIR "block.png", false, "block");
	ResourceManager::LoadTexture(TEXTURES_DIR "block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture(TEXTURES_DIR "background.jpg", false, "background");
	ResourceManager::LoadTexture(TEXTURES_DIR "paddle.png", true, "paddle");
	ResourceManager::LoadTexture(TEXTURES_DIR "particle.png", true, "particle");
	// Load levels
	GameLevel one; one.Load(ASSETS_DIR "levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load(ASSETS_DIR "levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load(ASSETS_DIR "levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load(ASSETS_DIR "levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	glm::vec2 playerPos = glm::vec2((this->Width - PLAYER_SIZE.x) / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));	

	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		512
	);
	
	initialized = true;
}
void Game::ProccessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}
void Game::Update(float dt)
{
	Ball->Move(dt, this->Width);
	this->DoCollisions();

	if (Ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}

	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
}
void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height));
		this->Levels[this->Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		Particles->Draw();
		Ball->Draw(*Renderer);
	}
}
void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load(ASSETS_DIR "/levels/one.lvl", this->Width, this->Height / 2.0f);
	else if (this->Level == 1)
		this->Levels[1].Load(ASSETS_DIR "/levels/two.lvl", this->Width, this->Height / 2.0f);
	else if (this->Level == 2)
		this->Levels[2].Load(ASSETS_DIR "/levels/three.lvl", this->Width, this->Height / 2.0f);
	else if (this->Level == 3)
		this->Levels[3].Load(ASSETS_DIR "/levels/four.lvl", this->Width, this->Height / 2.0f);
}
void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}
// Collisions
Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f)
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
bool CheckCollision(GameObject& a, GameObject& b)
{
	bool collisionX = a.Position.x + a.Size.x >= b.Position.x && b.Position.x + b.Size.x >= a.Position.x;
	bool collisionY = a.Position.y + a.Size.y >= b.Position.y && b.Position.y + b.Size.y >= a.Position.y;
	return collisionX && collisionY;
}
Collision CheckCollision(BallObject& a, GameObject& b)
{
	glm::vec2 center(a.Position + a.Radius);	// Ball center
	glm::vec2 aabb_half_extents(b.Size.x / 2.0f, b.Size.y / 2.0f);
	glm::vec2 aabb_center(b.Position.x + aabb_half_extents.x, b.Position.y + aabb_half_extents.y);
	glm::vec2 difference = center - aabb_center; // Rectangle center -> Ball center
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	if (glm::length(difference) <= a.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, Direction::up, glm::vec2(0.0f, 0.0f));	
}
void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
					box.Destroyed = true;
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == Direction::left || dir == Direction::right)
				{
					Ball->Velocity.x *= -1;
					float penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == Direction::left)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				else {
					Ball->Velocity.y *= -1;
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == Direction::up)
						Ball->Position.y -= penetration;
					else
						Ball->Position.y += penetration;
				}
			}
		}
	}
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1 * std::abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}