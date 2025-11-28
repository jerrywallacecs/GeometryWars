#include "Game.h"

#include <iostream>
#include <fstream>

Game::Game(const std::string& config) : m_text(m_font, "Default", 24)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// TODO: read in config file, use structs
	std::string section;
	uint16_t windowWidth = 1280;
	uint16_t windowHeight = 720;
	int frameLimit = 165;
	int fullscreen = 0;
	std::ifstream fin(path);

	if (!fin)
	{
		std::cerr << "could not open file" << '\n' << "current working dir: " << std::filesystem::current_path() << '\n';
	}
	
	while (fin >> section)
	{
		if (section == "Window")
		{
			fin >> windowWidth >> windowHeight >> frameLimit >> fullscreen;

			if (fullscreen == 1)
			{
				m_window.create(sf::VideoMode({ windowWidth, windowHeight }), "GeometryWars", sf::Style::Default, sf::State::Fullscreen);
			}

			else
			{
				m_window.create(sf::VideoMode({ windowWidth, windowHeight }), "GeometryWars", sf::Style::Default, sf::State::Windowed);
			}

			m_window.setKeyRepeatEnabled(false);
			m_window.setFramerateLimit(frameLimit);
		}

		if (section == "Font")
		{
			
		}

		if (section == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
			spawnPlayer();
		}

		if (section == "Enemy")
		{
			spawnEnemy();
		}
	}

	if (!ImGui::SFML::Init(m_window)) {}

	// not a huge fan of the scaling, might change my mind

	// scale the imgui ui and text size by 2
	//ImGui::GetStyle().ScaleAllSizes(2.0f);
	//ImGui::GetIO().FontGlobalScale = 2.0f;
}

std::shared_ptr<Entity> Game::player()
{
	return m_entities.getEntities("player").back();
}

void Game::run()
{
	// TODO: add pause functionality
	//		some systems should function while paused (rendering)
	//		some shouldn't (movement / input)
	while (true)
	{
		// update the entity manager
		m_entities.update();

		// required update call to imgui
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		sUserInput();
		sEnemySpawner();
		sMovement();
		sCollision();
		sGUI();
		sRender();

		// increment the current frame
		// may need to be moved when pause implemented
		m_currentFrame++;
	}
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct values from the config file

	// We create every entity by calling EntityManager.addEntitity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto e = m_entities.addEntity("player");

	// Give this entity a Transform so it spawns at (200, 200) with velocity (1, 1) and angle 0
	e->add<CTransform>(Vec2<float>(200.0f, 200.0f), Vec2<float>(1.0f, 1.0f), 0.0f);

	// note the static_cast here | solves issues with narrowing conversions
	e->add<CShape>( m_playerConfig.SR, m_playerConfig.V, sf::Color({ static_cast<uint8_t>(m_playerConfig.FR), static_cast<uint8_t>(m_playerConfig.FG), static_cast<uint8_t>(m_playerConfig.FB) }), sf::Color({ static_cast<uint8_t>(m_playerConfig.OR), static_cast<uint8_t>(m_playerConfig.OG), static_cast<uint8_t>(m_playerConfig.OB) }), m_playerConfig.OT);

	// Add an input component to the player so that we can use inputs
	e->add<CInput>();

	std::cout << "created player with the following attributes: " << '\n';
	std::cout << "shape radius: " << m_playerConfig.SR << '\n';
	std::cout << "collision radius: " << m_playerConfig.CR << '\n';
	std::cout << "speed: " << m_playerConfig.S << '\n';
	std::cout << "fill color: " << m_playerConfig.FR << ' ' << m_playerConfig.FG << ' ' << m_playerConfig.FB << '\n';
	std::cout << "outline color: " << m_playerConfig.OR << ' ' << m_playerConfig.OG << ' ' << m_playerConfig.OB << '\n';
	std::cout << "outline thickness: " << m_playerConfig.OT << '\n';
	std::cout << "vertices: " << m_playerConfig.V << '\n';

}

void Game::spawnEnemy()
{
	// TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
	//		the enemy must be spawned completely within the bounds of the window
	auto e = m_entities.addEntity("enemy");
	e->add<CTransform>(Vec2<float>(600.0f, 600.0f), Vec2<float>(1.0f, 1.0f), 0.0f);
	e->add<CShape>(16.0f, 5, sf::Color(10, 10, 10), sf::Color(0, 255, 0), 4.0f);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: spawn small enemies at the location of the input enemy e

	// when we create the smaller enemy, we have to read the values from the original enemy
	// - spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2<float>& target)
{
	// TODO: implement the spawning of a bullet which travels toward target
	//		- bullet speed is given as a scalar speed
	//		- you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: implement special weapon
}

void Game::sMovement()
{
	// TODO: implement all entity movement in this function
	//		Should read the m_player->cInput component to determine if the player is moving

	auto& transform = player()->get<CTransform>();

	// TODO: use formula for two directional movement, it is currently faster than it should be | note: could use if else and check for two directions
	if (player()->get<CInput>().up)
	{
		transform.pos.y -= transform.velocity.y;
	}

	if (player()->get<CInput>().down)
	{
		transform.pos.y += transform.velocity.y;
	}

	if (player()->get<CInput>().left)
	{
		transform.pos.x -= transform.velocity.x;
	}

	if (player()->get<CInput>().right)
	{
		transform.pos.x += transform.velocity.x;
	}

	// after we implement vec2 + operator
}

void Game::sLifespan()
{
	// TODO: implement all lifespan functionality

	/* 
	for all entities
		if entity has no lifespan component, skip it
		if entity has > 0 remaining lifespan, subtract 1
		if entity has lifespan and is alive
			scale alpha channel properly
		if entity has lifespan and its time is up
			destroy entity
	*/
	for (auto e : m_entities.getEntities())
	{
		if (!e->has<CLifespan>())
		{
			continue;
		}

		if (e->get<CLifespan>().remaining > 0)
		{
			e->get<CLifespan>().remaining -= 1;
		}

		if (e->has<CLifespan>() && e->isAlive())
		{
			// TODO
		}

		if (e->get<CLifespan>().remaining <= 0)
		{
			// TODO
		}
	}
}

void Game::sCollision()
{
	// TODO: implement all proper collisions between entities
	// be sure to use the collision radius, NOT the shape radius

	for (auto b : m_entities.getEntities("bullet"))
	{
		for (auto e : m_entities.getEntities("enemy"))
		{
			// do collision logic
		}

		for (auto e : m_entities.getEntities("smallEnemy"))
		{
			// do collision logic
		}
	}
}

void Game::sEnemySpawner()
{
	//spawnEnemy();
}

void Game::sGUI()
{
	ImGui::Begin("GeometryWars");
	ImGui::Text("Stuff Goes Here");
	ImGui::End();
}

void Game::sRender()
{
	if (!m_window.isOpen()) { return; }

	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		if (e->isAlive())
		{
			// sets the position of the shape based on the entity's transform->pos
			e->get<CShape>().circle.setPosition(e->get<CTransform>().pos);
			// sets the rotation of the shape based on the entity's transform->angle
			e->get<CTransform>().angle += 1.0f;
			e->get<CShape>().circle.setRotation(sf::degrees(e->get<CTransform>().angle));
			m_window.draw(e->get<CShape>().circle);
		}
	}

	// draw the ui last
	ImGui::SFML::Render(m_window);

	m_window.display();
}

void Game::sUserInput()
{
	while (auto event = m_window.pollEvent())
	{
		// pass the event to imgui to be parsed
		ImGui::SFML::ProcessEvent(m_window, *event);

		// this event triggers when the window is closed
		if (event->is<sf::Event::Closed>())
		{
			std::exit(0);
		}

		// this event is triggered when a key is pressed
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			// print the key that was pressed to the console
			std::cout << "Key pressed = " << int(keyPressed->scancode) << '\n';

			if (keyPressed->scancode == sf::Keyboard::Scancode::W)
			{
				player()->get<CInput>().up = true;
			}

			if (keyPressed->scancode == sf::Keyboard::Scancode::A)
			{
				player()->get<CInput>().left = true;
			}

			if (keyPressed->scancode == sf::Keyboard::Scancode::S)
			{
				player()->get<CInput>().down = true;
			}

			if (keyPressed->scancode == sf::Keyboard::Scancode::D)
			{
				player()->get<CInput>().right = true;
			}

			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
			{
				std::exit(0);
			}
		}

		// this event is triggered when a key is released
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			// print the key that was released to the console
			std::cout << "Key released = " << int(keyReleased->scancode) << '\n';

			if (keyReleased->scancode == sf::Keyboard::Scancode::W)
			{
				player()->get<CInput>().up = false;
			}

			if (keyReleased->scancode == sf::Keyboard::Scancode::A)
			{
				player()->get<CInput>().left = false;
			}

			if (keyReleased->scancode == sf::Keyboard::Scancode::S)
			{
				player()->get<CInput>().down = false;
			}

			if (keyReleased->scancode == sf::Keyboard::Scancode::D)
			{
				player()->get<CInput>().right = false;
			}
		}

		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
		{
			Vec2<float> mpos(mousePressed->position);
			if (mousePressed->button == sf::Mouse::Button::Left)
			{
				spawnBullet(player(), mpos);
			}
		}
	}
}