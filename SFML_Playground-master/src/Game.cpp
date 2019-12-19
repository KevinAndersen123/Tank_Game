//Kevin Andersen
//21-09-2018
//ID: c00225541

#include "Game.h"
#include <iostream>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "SFML Playground", sf::Style::Default)
	, m_tank(m_texture,m_wallSprites, m_aiTank)
	, m_aiTank(m_texture, m_wallSprites)
	, m_hud(m_font)
{
	std::srand(static_cast<unsigned int>(time(NULL)));
	int currentLevel = 1;

	// Will generate an exception if level loading fails.
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception & e)
	{
		std::cout << "Level Loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}
	m_window.setVerticalSyncEnabled(true); //makes tank faster

	if (!m_bgTexture.loadFromFile("./resources/images/background.jpg"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}
	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}
	if (!m_font.loadFromFile("./resources/fonts/Norse-Bold.otf"))
	{
		std::string s("Error loading font");
		throw std::exception(s.c_str());
	}
	m_tank.setPosition(m_level.m_tank.m_position);
	
	m_bgSprite.setTexture(m_bgTexture);
	generateWalls();
	// Populate the obstacle list and set the AI tank position.
	m_aiTank.init(m_level.m_aiTank.m_position);
	
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Int32 lag = 0;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt.asMilliseconds();
		m_timeLeft -= dt;

		if (sf::seconds(0.0f) >= m_timeLeft)
		{
			switch (m_gameState)
			{
			case GameState::GAME_RUNNING:
				break;
			case GameState::GAME_WIN:
				restartGame();
				break;
			case GameState::GAME_LOSE:
				restartGame();
				break;
			default:
				break;
			}
		}
		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}
		update(MS_PER_UPDATE);

		render();
	}

}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		default:
			break;
		}
	}
	
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	switch (m_gameState)
	{
	case GameState::GAME_RUNNING:
		m_tank.update(dt);
		m_aiTank.update(m_tank, dt);

		if (m_aiTank.collidesWithPlayer(m_tank))
		{
			if (m_aiTank.getState() != TankAi::AiState::DEAD)
			{
				m_timeLeft = sf::seconds(3.0f);
				m_gameState = GameState::GAME_LOSE;
			}
			else
			{
				m_aiTanksAlive--;
			}
		}
		if (0 == m_aiTanksAlive)
		{
			m_timeLeft = sf::seconds(3.0f);
			m_gameState = GameState::GAME_WIN;
		}
		if (m_aiTank.getState() != TankAi::AiState::DEAD && m_timeLeft <= sf::seconds(3.0f))
		{
			m_gameState = GameState::GAME_LOSE;
		}
		break;
	case GameState::GAME_WIN:
		break;
	case GameState::GAME_LOSE:
		break;
	default:
		break;
	}
	m_hud.update(m_gameState,m_timeLeft, m_tank.getRemainingBullets(), m_aiTanksAlive);

}
////////////////////////////////////////////////////////////
void Game::generateWalls()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	// Create the Walls 
	for (ObstacleData const & obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

void Game::restartGame()
{
	m_timeLeft = sf::seconds(63.0f);
	m_tank.setPosition(m_level.m_tank.m_position);
	m_tank.reset();
	m_aiTank.init(m_level.m_aiTank.m_position);
	m_aiTanksAlive = 1;
	m_gameState = GameState::GAME_RUNNING;
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	
	m_window.draw(m_bgSprite);
	m_tank.render(m_window);
	m_aiTank.render(m_window);
	m_hud.render(m_window);
	for (auto &obstacle :m_wallSprites)
	{
		m_window.draw(obstacle);
	}

	m_window.display();

}






