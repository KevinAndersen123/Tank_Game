#include "HUD.h"

////////////////////////////////////////////////////////////
HUD::HUD(sf::Font & hudFont)
	: m_textFont(hudFont)
{
	m_gameStateText.setFont(hudFont);
	m_gameStateText.setCharacterSize(40);
	m_gameStateText.setFillColor(sf::Color::Blue);
	m_gameStateText.setPosition(sf::Vector2f(30, 3));
	m_gameStateText.setString("Game Running");

	//Setting up our hud properties 
	m_hudOutline.setSize(sf::Vector2f(1440.0f, 50.0f));
	m_hudOutline.setFillColor(sf::Color(0, 0, 0, 38));
	m_hudOutline.setOutlineThickness(-.5f);
	m_hudOutline.setOutlineColor(sf::Color(0, 0, 0, 100));
	m_hudOutline.setPosition(0, 0);

	//time remaining
	m_timeLeftText.setFont(hudFont);
	m_timeLeftText.setCharacterSize(40);
	m_timeLeftText.setFillColor(sf::Color::Black);
	m_timeLeftText.setPosition(sf::Vector2f(1250, 3));
	m_timeLeftText.setString("Time Left: ");
	
	//bullets left setup
	m_bulletsLeftText.setFont(hudFont);
	m_bulletsLeftText.setCharacterSize(40);
	m_bulletsLeftText.setFillColor(sf::Color::Black);
	m_bulletsLeftText.setPosition(sf::Vector2f(530, 3));
	m_bulletsLeftText.setString("Bullets Left: ");
	//enemys left
	m_enemysLeftText.setFont(hudFont);
	m_enemysLeftText.setCharacterSize(40);
	m_enemysLeftText.setFillColor(sf::Color::Black);
	m_enemysLeftText.setPosition(sf::Vector2f(900, 3));
	m_enemysLeftText.setString("Enemies Left: ");
}

////////////////////////////////////////////////////////////
void HUD::update(GameState const & gameState, sf::Time t_timeLeft,int t_remainingBullets, int t_enemysLeft)
{
	switch (gameState)
	{
	case GameState::GAME_RUNNING:
		m_gameStateText.setString("Game Running");
		m_gameStateText.setFillColor(sf::Color::Blue);
		m_timeLeftText.setString("Time Left: " + std::to_string(static_cast<int>(t_timeLeft.asSeconds()-3)));
		m_bulletsLeftText.setString("Bullets Left: " + std::to_string(t_remainingBullets));
		m_enemysLeftText.setString("Enemies Left: " + std::to_string(t_enemysLeft));
		break;
	case GameState::GAME_WIN:
		m_gameStateText.setString("You Won");
		break;
	case GameState::GAME_LOSE:
		m_gameStateText.setString("You Lost");
		m_gameStateText.setFillColor(sf::Color::Red);
		break;
	default:
		break;
	}
}

void HUD::render(sf::RenderWindow & window)
{
	window.draw(m_hudOutline);
	window.draw(m_gameStateText);
	window.draw(m_timeLeftText);
	window.draw(m_bulletsLeftText);
	window.draw(m_enemysLeftText);
}