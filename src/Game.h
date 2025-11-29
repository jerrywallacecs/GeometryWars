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
	sf::RenderWindow m_window; // window we will draw to
	EntityManager m_entities; // vector of entities to maintain
	sf::Font m_font;
	sf::Text m_text; // score text to be drawn
	std::string section; // for file parsing
	WindowConfig m_windowSize;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	sf::Clock m_deltaClock;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false; // whether we update game logic

	void init(const std::string& config); // initialize the GameState with a config file
	void setPaused(bool paused); // pause the game
	
	// SYSTEMS
	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sGUI();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2<float>& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

	std::shared_ptr<Entity> player();

public:
	Game(const std::string& config); // constructor, takes in game config
	void run();
};
