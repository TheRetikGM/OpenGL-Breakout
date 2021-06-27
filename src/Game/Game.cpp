#include "Game/Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "config.h"
#include <GLFW/glfw3.h>
#include "Game/BallObject.h"
#include "Game/ParticleGen.h"
#include "Game/PostProcessor.h"
#include <tuple>
#include <stdexcept>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <iostream>
#include "DebugColors.h"
#include "TextRenderer.h"
#include <sstream>
typedef std::tuple<bool, Direction, glm::vec2> Collision;

SpriteRenderer* Renderer = nullptr;

unsigned int max_levels = 4;
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);	// 500px / 1s
GameObject* Player = nullptr;

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
BallObject* Ball = nullptr;

ParticleGenerator *Particles;
PostProcessor *Effects;
float shake_time = 0.0f;

sf::Music *music = nullptr;
sf::SoundBuffer *sb_bleep1 = nullptr;
sf::SoundBuffer *sb_bleep2 = nullptr;
sf::SoundBuffer *sb_solid = nullptr;
sf::SoundBuffer *sb_powerup = nullptr;
sf::Sound *sound_1 = nullptr;
sf::Sound *sound_2 = nullptr;

TextRenderer *Text = nullptr;

bool initialized = false;

void ActivatePowerUp(PowerUp &powerup);
bool ShouldSpawn(unsigned int chance);
Direction VectorDirection(glm::vec2 target);
bool CheckCollision(GameObject& a, GameObject& b);
Collision CheckCollision(BallObject& a, GameObject& b);
bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, PowerUpType type);

Game::Game (unsigned int width, unsigned int height) : Width(width), Height(height), State(GAME_MENU), Lives(3) {}
Game::~Game(){}
void Game::Destroy()
{
	if (initialized)
	{
		delete Renderer;
		delete Player;
		delete Ball;
		delete Particles;
		delete Effects;

		delete music;
		delete sb_bleep1;
		delete sb_bleep2;
		delete sb_solid;
		delete sb_powerup;
		delete sound_1;
		delete sound_2;

		delete Text;
	}
}
void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader(SHADERS_DIR "SpriteRender.vert", SHADERS_DIR "SpriteRender.frag", nullptr, "sprite");
	ResourceManager::LoadShader(SHADERS_DIR "particle.vert", SHADERS_DIR "particle.frag", nullptr, "particle");
	ResourceManager::LoadShader(SHADERS_DIR "effects.vert", SHADERS_DIR "effects.frag", nullptr, "effects");

	// Set basic shader uniforms
	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInt("spriteImage", 0);
	ResourceManager::GetShader("sprite").SetMat4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetMat4("projection", projection);	
	//ResourceManager::GetShader("particle").Use().SetMat4("projection", projection);	
	
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	
	// Load textures
	ResourceManager::LoadTexture(TEXTURES_DIR "awesomeface.png", true, "face");
	ResourceManager::LoadTexture(TEXTURES_DIR "block.png", false, "block");
	ResourceManager::LoadTexture(TEXTURES_DIR "block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture(TEXTURES_DIR "background.jpg", false, "background");
	ResourceManager::LoadTexture(TEXTURES_DIR "paddle.png", true, "paddle");
	ResourceManager::LoadTexture(TEXTURES_DIR "particle.png", true, "particle");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_passthrough.png", true, "powerup_passthrough");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture(TEXTURES_DIR "powerup_sticky.png", true, "powerup_sticky");
	
	// Load levels
	GameLevel one; one.Load(ASSETS_DIR "levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load(ASSETS_DIR "levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load(ASSETS_DIR "levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load(ASSETS_DIR "levels/four.lvl", this->Width, this->Height / 2);
	GameLevel test; test.Load(ASSETS_DIR "levels/test.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Levels.push_back(test);
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

	Effects = new PostProcessor(ResourceManager::GetShader("effects"), this->Width, this->Height);	
	
	// init SFML music and audio objects
	music = new sf::Music();
	sb_bleep1 = new sf::SoundBuffer();
	sb_bleep2 = new sf::SoundBuffer();
	sb_solid = new sf::SoundBuffer();
	sb_powerup = new sf::SoundBuffer();
	sound_1 = new sf::Sound();
	sound_2 = new sf::Sound();
	// Load background music
	if (!music->openFromFile(ASSETS_DIR "audio/breakout.wav"))
		std::cerr << DC_ERROR " Could not open music " ASSETS_DIR "audio/breakout.wav" << std::endl;
	else
	{
		music->setLoop(true);		
		music->play();
	}
	// Load sound effects
	if (!sb_bleep1->loadFromFile(ASSETS_DIR "audio/bleep1.wav"))
		std::cerr << DC_ERROR " Could not open sound file " ASSETS_DIR "audio/bleep1.wav" << "\n";
	if (!sb_bleep2->loadFromFile(ASSETS_DIR "audio/bleep2.wav"))
		std::cerr << DC_ERROR " Could not open sound file " ASSETS_DIR "audio/bleep2.wav" << "\n";
	if (!sb_powerup->loadFromFile(ASSETS_DIR "audio/powerup.wav"))
		std::cerr << DC_ERROR " Could not open sound file " ASSETS_DIR "audio/powerup.wav" << "\n";
	if (!sb_solid->loadFromFile(ASSETS_DIR "audio/solid.wav"))
		std::cerr << DC_ERROR " Could not open sound file " ASSETS_DIR "audio/solid.wav" << "\n";

	// Initialize TextRenderer
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load(ASSETS_DIR "fonts/ocraext.ttf", 24);

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
	else if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % (max_levels + 1);
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = max_levels;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}
	else if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_MENU;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->chaos = false;
		}
	}
}
void Game::Update(float dt)
{
	Ball->Move(dt, this->Width);
	this->DoCollisions();

	if (Ball->Position.y >= this->Height)
	{
		--this->Lives;
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}

	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

	if (shake_time > 0.0f)
	{
		shake_time -= dt;
		if (shake_time <= 0.0f)		
			Effects->shake = false;		
	}

	UpdatePowerUps(dt);

	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->chaos = true;
		this->State = GAME_WIN;
	}
}
void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		Effects->BeginRender();

		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height));
		this->Levels[this->Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		for (PowerUp &powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		Particles->Draw();		
		Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render((float)glfwGetTime());

		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
        Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		Text->RenderText("You WON!!!", 320.0f, Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
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
	else if (this->Level == 4)
		this->Levels[4].Load(ASSETS_DIR "/levels/test.lvl", this->Width, this->Height / 2.0f);

	this->Lives = 3;
}
void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
	// Disable powerups
	for (auto& powerUp : this->PowerUps)
	{
		if (powerUp.Activated)
			powerUp.Duration = 0.0f;
		else if (!powerUp.Destroyed)
		{
			powerUp.Destroyed = true;
		}
	}
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
				{
					box.Destroyed = true;
					this->SpawnPowerUps(box);
					sound_1->setBuffer(*sb_bleep1);
					sound_1->play();
				}
				else 
				{
					shake_time = 0.05f;
					Effects->shake = true;
					sound_1->setBuffer(*sb_solid);
					sound_1->play();
				}
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid))
				{
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
	}
	for (auto &powerup : this->PowerUps)
	{
		if (!powerup.Destroyed) 
		{
			if (powerup.Position.y >= this->Height)
				powerup.Destroyed = true;
			if (CheckCollision(*Player, powerup)) {
				ActivatePowerUp(powerup);
				powerup.Destroyed = true;
				powerup.Activated = true;

				sound_2->setBuffer(*sb_powerup);
				sound_2->play();
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
		Ball->Stuck = Ball->Sticky;

		sound_1->setBuffer(*sb_bleep2);
		sound_1->play();
	}
}
bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() & chance;
	return random == 0;
}
void Game::SpawnPowerUps(GameObject &block)
{
	if (ShouldSpawn(75)) // 1 in a 75 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::speed, glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, 
			ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75)) // 1 in a 75 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::sticky, glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, 
			ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75)) // 1 in a 75 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::passthrough, glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, 
			ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75)) // 1 in a 75 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::increase, glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, 
			ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // 1 in a 15 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::confuse, glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, 
			ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15)) // 1 in a 15 chance
		this->PowerUps.push_back(
			PowerUp(PowerUpType::chaos, glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, 
			ResourceManager::GetTexture("powerup_chaos")));
}
void ActivatePowerUp(PowerUp &powerup)
{
	if (powerup.Type == PowerUpType::speed)
	{
		Ball->Velocity *= 1.2;	// increase ball speed by 20%
	}
	else if (powerup.Type == PowerUpType::sticky)
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerup.Type == PowerUpType::passthrough)
	{
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerup.Type == PowerUpType::increase)
	{
		Player->Size.x += 50;	// increase paddle size by 50 pixels
	}
	else if (powerup.Type == PowerUpType::confuse)
	{
		if (!Effects->chaos)
			Effects->confuse = true;
	}
	else if (powerup.Type == PowerUpType::chaos)
	{
		if (!Effects->confuse)
			Effects->chaos = true;
	}
}
void Game::UpdatePowerUps(float dt)
{
	for (auto &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = false;
				
				if (powerUp.Type == PowerUpType::sticky)
				{
					if (!isOtherPowerUpActive(this->PowerUps, PowerUpType::sticky))
					{
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == PowerUpType::passthrough)
				{
					if (!isOtherPowerUpActive(this->PowerUps, PowerUpType::passthrough))
					{
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == PowerUpType::confuse)
				{
					if (!isOtherPowerUpActive(this->PowerUps, PowerUpType::confuse))
					{
						Effects->confuse = false;
					}
				}
				else if (powerUp.Type == PowerUpType::chaos)
				{
					if (!isOtherPowerUpActive(this->PowerUps, PowerUpType::chaos))
					{
						Effects->chaos = false;
					}
				}
			}
		}
	}

	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(), [](const PowerUp &powerUp)
	{
		return powerUp.Destroyed && !powerUp.Activated;
	}), this->PowerUps.end());
}

bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, PowerUpType type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}
