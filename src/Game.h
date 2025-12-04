#pragma once

#include "Core/Entity.h"
#include "Core/EntityManager.hpp"
#include "Vec2.hpp"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

struct WindowConfig
{
	uint16_t windowWidth, windowHeight;
	int framerate, fullscreen;
};

struct FontConfig
{
	std::string filepath;
	int fontSize = 0;
	int r = 0;
	int g = 0;
	int b = 0;
};

struct PlayerConfig
{
	int SR, CR, OT, V;
	int FR, FG, FB, OR, OG, OB;
	float S;
};

struct EnemyConfig
{
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
	float SMIN, SMAX;
};

struct BulletConfig
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
	float S;
};

class Game
{
	// basic stuff
	sf::RenderWindow m_window;
	sf::Font m_font;
	std::string section; // file parsing

	// config structs
	WindowConfig m_windowConfig;
	FontConfig m_fontConfig;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;

	// vector of entities to maintain
	EntityManager m_entities;

	// unused?
	sf::Clock m_deltaClock;
	
	// background
	sf::Color m_tileColor1;
	sf::Color m_tileColor2;
	int m_backgroundTileSize;

	// scoring
	sf::Text m_scoreText;
	int m_score = 0;

	// counters
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	int m_lastQuillFired = 0;

	// special ability
	sf::Text m_specialAbilityText;
	int m_cooldownFrames = 600;
	int m_cooldownRemaining = 0;

	// whether we update game logic
	bool m_paused = false;

	// whether debug gui is visible
	bool m_devMode = false;

	// systems toggle
	bool m_movement = true;
	bool m_userInput = true;
	bool m_lifespan = true;
	bool m_enemySpawner = true;
	bool m_collision = true;

	// init gamestate with config file
	void init(const std::string& config);
	
	// SYSTEMS
	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sGUI();
	void sEnemySpawner();
	void sCollision();

	// helper functions
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2<float>& mousePos);
	void spawnQuill(std::shared_ptr<Entity> entity, const Vec2<float>& mousePos);
	void spawnSmallQuills(std::shared_ptr<Entity> entity);

	// ptr to player
	std::shared_ptr<Entity> player();

public:
	Game(const std::string& config); // constructor, takes in game config
	void run();
};
