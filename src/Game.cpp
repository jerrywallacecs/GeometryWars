#include "Game.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

Game::Game(const std::string& config) : m_text(m_font, "Default", 24)
{
	init(config);
}

void Game::init(const std::string& path)
{
	std::ifstream fin(path);

	if (!fin)
	{
		std::cerr << "could not open file" << '\n' << "current working dir: " << std::filesystem::current_path() << '\n';
	}
	
	while (fin >> section)
	{
		if (section == "Window")
		{
			fin >> m_windowConfig.windowWidth >> m_windowConfig.windowHeight >> m_windowConfig.framerate >> m_windowConfig.fullscreen;

			if (m_windowConfig.fullscreen == 1)
			{
				m_window.create(sf::VideoMode({ m_windowConfig.windowWidth, m_windowConfig.windowHeight }), "GeometryWars", sf::Style::Default, sf::State::Fullscreen);
			}

			else
			{
				m_window.create(sf::VideoMode({ m_windowConfig.windowWidth, m_windowConfig.windowHeight }), "GeometryWars", sf::Style::Default, sf::State::Windowed);
			}

			m_window.setKeyRepeatEnabled(false);
			m_window.setFramerateLimit(m_windowConfig.framerate);
		}

		if (section == "Font")
		{
			fin >> m_fontConfig.filepath >> m_fontConfig.fontSize >> m_fontConfig.r >> m_fontConfig.g >> m_fontConfig.b;
			if (!m_font.openFromFile(m_fontConfig.filepath))
			{
				std::cerr << "Could not load font!\n";
				std::exit(-1);
			}
			m_text.setCharacterSize(m_fontConfig.fontSize);
			m_text.setFillColor(sf::Color(m_fontConfig.r, m_fontConfig.g, m_fontConfig.b));
		}

		if (section == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
			spawnPlayer();
		}

		if (section == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}

		if (section == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	if (!ImGui::SFML::Init(m_window)) {}
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
		sLifespan();
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
	auto e = m_entities.addEntity("player");
	e->add<CTransform>(Vec2<float>(m_windowConfig.windowWidth / 2, m_windowConfig.windowHeight / 2), Vec2<float>(m_playerConfig.S, m_playerConfig.S), 0.0f);

	// note the static_cast here | solves issues with narrowing conversions
	e->add<CShape>( m_playerConfig.SR, m_playerConfig.V, sf::Color({ static_cast<uint8_t>(m_playerConfig.FR), static_cast<uint8_t>(m_playerConfig.FG), static_cast<uint8_t>(m_playerConfig.FB) }), sf::Color({ static_cast<uint8_t>(m_playerConfig.OR), static_cast<uint8_t>(m_playerConfig.OG), static_cast<uint8_t>(m_playerConfig.OB) }), m_playerConfig.OT);
	e->add<CCollision>(m_playerConfig.CR);
	e->add<CInput>();
}

void Game::spawnEnemy()
{
	// for random numbers we can use mersenne twister (#include <random>) and uniform int distribution
	// as well as using std::chrono::steady_clock to seed our mersenne twister.

	// curious if the psuedo-random values are connected due to using only one twister... RESEARCH

	std::mt19937 twister{ static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()) };
	// random location on screen | note: we are using the collision radius (CR) in the config to avoid looping through all entities when the collision radius is the same for all shapes
	std::uniform_real_distribution width{ 0.0f + m_enemyConfig.CR, static_cast<float>(m_windowConfig.windowWidth) - m_enemyConfig.CR };
	std::uniform_real_distribution height{ 0.0f + m_enemyConfig.CR, static_cast<float>(m_windowConfig.windowHeight) - m_enemyConfig.CR };
	Vec2 position(width(twister), height(twister));

	// random speed between SMIN and SMAX
	std::uniform_real_distribution speedDistibution{ m_enemyConfig.SMIN, m_enemyConfig.SMAX };
	Vec2 speed(speedDistibution(twister), speedDistibution(twister));

	// random number of vertices between VMIN and VMAX
	std::uniform_int_distribution vertexDistibution{ m_enemyConfig.VMIN, m_enemyConfig.VMAX };
	int vertices = vertexDistibution(twister);

	// random value for R
	std::uniform_int_distribution colorDistribution{ 0, 255 };
	uint8_t r = colorDistribution(twister);
	uint8_t g = colorDistribution(twister);
	uint8_t b = colorDistribution(twister);


	auto e = m_entities.addEntity("enemy");
	e->add<CTransform>(position, speed, 0.0f);
	e->add<CShape>(m_enemyConfig.SR, vertices, sf::Color(r, g, b, 255), sf::Color({static_cast<uint8_t>(m_enemyConfig.OR), static_cast<uint8_t>(m_enemyConfig.OG), static_cast<uint8_t>(m_enemyConfig.OB), 255}), m_enemyConfig.OT);
	e->add<CCollision>(m_enemyConfig.CR);
	e->add<CScore>(vertices * 100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: spawn small enemies at the location of the input enemy e
	int count = e->get<CShape>().circle.getPointCount();

	for (int i = 1; i <= count; i++)
	{
		float angleStep = 360.0f / count;
		float angle = i * angleStep;

		auto smallEnemy = m_entities.addEntity("smallEnemy");

		// note std::cos and std::sin take in radians, not degrees
		float radians = angle * (3.1415926f / 180.0f);
		float speed = e->get<CTransform>().velocity.length();
		Vec2 velocity((speed * std::cos(radians) / 2), (speed * std::sin(radians) / 2));

		smallEnemy->add<CTransform>(
			e->get<CTransform>().pos,
			velocity,
			angle
		);

		smallEnemy->add<CShape>(
			m_enemyConfig.SR / 2,
			count,
			e->get<CShape>().circle.getFillColor(),
			e->get<CShape>().circle.getOutlineColor(),
			e->get<CShape>().circle.getOutlineThickness()
		);

		// not sure how i feel about this.
		smallEnemy->add<CCollision>(e->get<CCollision>().radius / 2);

		smallEnemy->add<CLifespan>(m_enemyConfig.L);

		smallEnemy->add<CScore>(e->get<CScore>().score * 2);
	}
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2<float>& target)
{
	auto& transform = entity->get<CTransform>();
	auto bullet = m_entities.addEntity("bullet");

	// calculate difference vector
	Vec2 differenceVector = target - entity->get<CTransform>().pos;
	// normalize difference vector
	differenceVector.normalize();
	// scale the normalized vector with speed scalar
	Vec2 speedScalar(m_bulletConfig.S, m_bulletConfig.S);
	Vec2 velocityVector = speedScalar * differenceVector;

	bullet->add<CTransform>(Vec2(transform.pos.x, transform.pos.y), velocityVector, 0.0f);
	bullet->add<CCollision>(m_bulletConfig.CR);
	bullet->add<CLifespan>(m_bulletConfig.L);
	bullet->add<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: implement special weapon
	// idea: railgun... could charge up and fire a "heat seeking bullet"
}

void Game::sMovement()
{
	if (m_movement)
	{
		auto& playerTransform = player()->get<CTransform>();

		// TODO: use formula for two directional movement, it is currently faster than it should be | note: could use if else and check for two directions
		if (player()->get<CInput>().up)
		{
			playerTransform.pos.y -= playerTransform.velocity.y;
		}

		if (player()->get<CInput>().down)
		{
			playerTransform.pos.y += playerTransform.velocity.y;
		}

		if (player()->get<CInput>().left)
		{
			playerTransform.pos.x -= playerTransform.velocity.x;
		}

		if (player()->get<CInput>().right)
		{
			playerTransform.pos.x += playerTransform.velocity.x;
		}

		// enemy animation
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			auto& enemyTransform = enemy->get<CTransform>();

			enemyTransform.pos += enemyTransform.velocity;
		}

		for (auto& smallEnemy : m_entities.getEntities("smallEnemy"))
		{
			smallEnemy->get<CTransform>().pos += smallEnemy->get<CTransform>().velocity;
		}

		// bullet animation
		for (auto& bullet : m_entities.getEntities("bullet"))
		{
			bullet->get<CTransform>().pos += bullet->get<CTransform>().velocity;
		}
	}
}

void Game::sLifespan()
{
	if (m_lifespan)
	{
		for (auto& e : m_entities.getEntities())
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
				sf::Color fillColor = e->get<CShape>().circle.getFillColor();
				sf::Color outlineColor = e->get<CShape>().circle.getOutlineColor();
				float ratio = (float)e->get<CLifespan>().remaining / e->get<CLifespan>().lifespan;
				fillColor.a = 255 * ratio;
				outlineColor.a = 255 * ratio;
				e->get<CShape>().circle.setFillColor(fillColor);
				e->get<CShape>().circle.setOutlineColor(outlineColor);
			}

			if (e->get<CLifespan>().remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

void Game::sCollision()
{
	if (m_collision)
	{
		// checking player collision with window
		for (auto& player : m_entities.getEntities("player"))
		{
			if (player->get<CTransform>().pos.x < 0 + player->get<CCollision>().radius)
			{
				player->get<CTransform>().pos.x = 0 + player->get<CCollision>().radius;
			}

			if (player->get<CTransform>().pos.x > m_windowConfig.windowWidth - player->get<CCollision>().radius)
			{
				player->get<CTransform>().pos.x = m_windowConfig.windowWidth - player->get<CCollision>().radius;
			}

			if (player->get<CTransform>().pos.y < 0 + player->get<CCollision>().radius)
			{
				player->get<CTransform>().pos.y = 0 + player->get<CCollision>().radius;
			}

			if (player->get<CTransform>().pos.y > m_windowConfig.windowHeight - player->get<CCollision>().radius)
			{
				player->get<CTransform>().pos.y = m_windowConfig.windowHeight - player->get<CCollision>().radius;
			}

			// checking player collision with enemy
			for (auto& enemy : m_entities.getEntities("enemy"))
			{
				Vec2 differenceVector(player->get<CTransform>().pos.x - enemy->get<CTransform>().pos.x, player->get<CTransform>().pos.y - enemy->get<CTransform>().pos.y);
				float squaredRadiusSum = (player->get<CCollision>().radius * player->get<CCollision>().radius) + ((player->get<CCollision>().radius * enemy->get<CCollision>().radius) * 2) + (enemy->get<CCollision>().radius * enemy->get<CCollision>().radius);
				if ((differenceVector.x * differenceVector.x) + (differenceVector.y * differenceVector.y) < squaredRadiusSum)
				{
					// collision occured - put player in middle of screen & destroy enemy
					player->get<CTransform>().pos = { static_cast<float>(m_windowConfig.windowWidth / 2), static_cast<float>(m_windowConfig.windowHeight / 2) };
					enemy->destroy();
				}
			}

			// checking player collision with small enemy
			for (auto& smallEnemy : m_entities.getEntities("smallEnemy"))
			{
				Vec2 differenceVector(player->get<CTransform>().pos.x - smallEnemy->get<CTransform>().pos.x, player->get<CTransform>().pos.y - smallEnemy->get<CTransform>().pos.y);
				float squaredRadiusSum = (player->get<CCollision>().radius * player->get<CCollision>().radius) + ((player->get<CCollision>().radius * smallEnemy->get<CCollision>().radius) * 2) + (smallEnemy->get<CCollision>().radius * smallEnemy->get<CCollision>().radius);
				if ((differenceVector.x * differenceVector.x) + (differenceVector.y * differenceVector.y) < squaredRadiusSum)
				{
					// collision occured - put player in middle of screen & destroy small enemy
					player->get<CTransform>().pos = { static_cast<float>(m_windowConfig.windowWidth / 2), static_cast<float>(m_windowConfig.windowHeight / 2) };
					smallEnemy->destroy();
				}
			}
		}

		// checking enemy collision with window
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			if (enemy->get<CTransform>().pos.x < 0 + enemy->get<CCollision>().radius || enemy->get<CTransform>().pos.x > m_windowConfig.windowWidth - enemy->get<CCollision>().radius)
			{
				enemy->get<CTransform>().velocity.x *= -1;
			}

			if (enemy->get<CTransform>().pos.y < 0 + enemy->get<CCollision>().radius || enemy->get<CTransform>().pos.y > m_windowConfig.windowHeight - enemy->get<CCollision>().radius)
			{
				enemy->get<CTransform>().velocity.y *= -1;
			}

			// if, for some reason, the entitity is out of the window, we should destroy it
			if (enemy->get<CTransform>().pos.x < 0 || enemy->get<CTransform>().pos.x > m_windowConfig.windowWidth)
			{
				enemy->destroy();
			}

			if (enemy->get<CTransform>().pos.y < 0 || enemy->get<CTransform>().pos.y > m_windowConfig.windowHeight)
			{
				enemy->destroy();
			}
		}

		// checking small enemy collision with window
		for (auto& smallEnemy : m_entities.getEntities("smallEnemy"))
		{
			if (smallEnemy->get<CTransform>().pos.x < 0 + smallEnemy->get<CCollision>().radius || smallEnemy->get<CTransform>().pos.x > m_windowConfig.windowWidth - smallEnemy->get<CCollision>().radius)
			{
				smallEnemy->get<CTransform>().velocity.x *= -1;
			}

			if (smallEnemy->get<CTransform>().pos.y < 0 + smallEnemy->get<CCollision>().radius || smallEnemy->get<CTransform>().pos.y > m_windowConfig.windowHeight - smallEnemy->get<CCollision>().radius)
			{
				smallEnemy->get<CTransform>().velocity.y *= -1;
			}
		}

		for (auto& bullet : m_entities.getEntities("bullet"))
		{
			// checking bullet collision with enemy
			for (auto& enemy : m_entities.getEntities("enemy"))
			{
				Vec2 differenceVector(bullet->get<CTransform>().pos.x - enemy->get<CTransform>().pos.x, bullet->get<CTransform>().pos.y - enemy->get<CTransform>().pos.y);
				float squaredRadiusSum = (bullet->get<CCollision>().radius * bullet->get<CCollision>().radius) + ((bullet->get<CCollision>().radius * enemy->get<CCollision>().radius) * 2) + (enemy->get<CCollision>().radius * enemy->get<CCollision>().radius);
				if ((differenceVector.x * differenceVector.x) + (differenceVector.y * differenceVector.y) < squaredRadiusSum)
				{
					// collision occured
					spawnSmallEnemies(enemy);
					m_score += enemy->get<CScore>().score;
					enemy->destroy();
					bullet->destroy();
				}
			}

			// checking bullet collision with small enemy
			for (auto& smallEnemy : m_entities.getEntities("smallEnemy"))
			{
				Vec2 differenceVector(bullet->get<CTransform>().pos.x - smallEnemy->get<CTransform>().pos.x, bullet->get<CTransform>().pos.y - smallEnemy->get<CTransform>().pos.y);
				float squaredRadiusSum = (bullet->get<CCollision>().radius * bullet->get<CCollision>().radius) + ((bullet->get<CCollision>().radius * smallEnemy->get<CCollision>().radius) * 2) + (smallEnemy->get<CCollision>().radius * smallEnemy->get<CCollision>().radius);
				if ((differenceVector.x * differenceVector.x) + (differenceVector.y + differenceVector.y) < squaredRadiusSum)
				{
					// collision occured
					m_score += smallEnemy->get<CScore>().score;
					smallEnemy->destroy();
				}
			}
		}
	}
}

void Game::sEnemySpawner()
{
	if (m_enemySpawner)
	{
		if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
		{
			spawnEnemy();
		}
	}
}

void Game::sGUI()
{
	ImGui::Begin("GeometryWars");
	if (ImGui::BeginTabBar("MainTabs"))
	{
		if (ImGui::BeginTabItem("Systems"))
		{
			ImGui::Text("Enable/Disable Systems");
			ImGui::Checkbox("User Input", &m_userInput);
			ImGui::Checkbox("Enemy Spawner", &m_enemySpawner);
			ImGui::Checkbox("Movement", &m_movement);
			ImGui::Checkbox("Collision", &m_collision);
			ImGui::Checkbox("Lifespan", &m_lifespan);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Entities"))
		{
			for (auto& [tag, vec] : m_entities.getEntityMap())
			{
				// dropdown header
				if (ImGui::TreeNode(tag.c_str()))
				{
					ImGui::Text("Delete | Entity Tag | ID");
					ImGui::Separator();

					for (auto& e : vec)
					{
						// unique id for button
						std::string buttonID = "DELETE##" + std::to_string(e->id());

						if (ImGui::Button(buttonID.c_str()))
						{
							if (e->tag() != "player")
							{
								e->destroy();
							}
							else
							{
								e->get<CTransform>().pos = { static_cast<float>(m_windowConfig.windowWidth / 2), static_cast<float>(m_windowConfig.windowHeight / 2) };
							}
						}

						ImGui::SameLine();
						ImGui::Text(e->tag().c_str());
						ImGui::SameLine();
						ImGui::Text(std::to_string(e->id()).c_str());
					}

					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Settings"))
		{
			ImGui::Text("Enemy Settings:");
			ImGui::Separator();
			if (ImGui::Button("Spawn Enemy"))
			{
				spawnEnemy();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Enemies"))
			{
				for (auto& e : m_entities.getEntities("enemy"))
				{
					e->destroy();
				}
			}
			ImGui::InputInt("Spawn Interval", &m_enemyConfig.SI);
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();
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

	m_text.setString("Current Score: " + std::to_string(m_score));
	m_window.draw(m_text);

	// draw the ui last
	ImGui::SFML::Render(m_window);

	m_window.display();
}

void Game::sUserInput()
{
	if (m_userInput)
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
}