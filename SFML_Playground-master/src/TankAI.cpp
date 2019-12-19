#include "TankAi.h"

////////////////////////////////////////////////////////////
TankAi::TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiState(AiState::PATROL_MAP)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}

////////////////////////////////////////////////////////////
void TankAi::update(Tank & playerTank, double dt)
{
	m_position = m_tankBase.getPosition();
	sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());
	sf::Vector2f vectorToCircle = seek(m_patrolCircle.getPosition());
	sf::Vector2f acceleration;
	switch (m_aiState)
	{
	case AiState::SEEK_PLAYER:
		MAX_SPEED = 105;
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		acceleration = m_steering / MASS;
		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);
		break;
	case AiState::RETREAT:
		m_steering -= thor::unitVector(sf::Vector2f(vectorToPlayer));
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		acceleration = m_steering / MASS;
		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);
		if (m_retreatTimer >= 0.0)
		{
			m_retreatTimer -= dt/1000;
		}
		else
		{
			m_retreatTimer = 5.0f; 
			m_aiState = AiState::PATROL_MAP;
			m_tankBase.setScale(1,1);
			m_turret.setScale(1, 1);
		}
	case AiState::PATROL_MAP:
		MAX_SPEED = 60;
		m_steering += thor::unitVector(vectorToCircle);
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		acceleration = m_steering / MASS;
		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);
		checkForHeatTrail(playerTank.getHeatTrail());
		break;
	default:
		break;
	}
	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}

	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation) - 1) % 360;
	}
	if (checkCollisionPatrolCircle() == true)
	{
		collisionPatrolCircle();
	}
	m_showCirlces = playerTank.getShowCircles();
	

	aheadArray.clear();
	aheadArray.append(m_position + sf::Vector2f(1, 1));
	aheadArray.append(m_halfAhead + sf::Vector2f(1, 1));
	aheadArray.append(m_position);
	aheadArray.append(m_ahead);

	aheadArray[0].color = sf::Color::Green;
	aheadArray[1].color = sf::Color::Green;
	aheadArray[2].color = sf::Color::Green;
	aheadArray[3].color = sf::Color::Green;
	updateMovement(dt);
}

void TankAi::takeDamage()
{
	m_tankBase.setScale(0.5,0.5);
	m_turret.setScale(0.5,0.5);
	m_velocity = MathUtility::truncate(m_velocity, 20);
	m_aiState = AiState::RETREAT;
	MAX_SPEED = 30;
}

////////////////////////////////////////////////////////////
void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);
	window.draw(m_turret);
	if (m_showCirlces == true)
	{
		for (sf::CircleShape const & circle : m_obstacles)
		{
			window.draw(circle);
		}
		window.draw(m_patrolCircle);
		window.draw(aheadArray);
	}
}

////////////////////////////////////////////////////////////
void TankAi::init(sf::Vector2f position)
{
	MAX_SPEED = 60;
	m_aiState = AiState::PATROL_MAP;
	m_tankBase.setScale(1, 1);
	m_turret.setScale(1, 1);
	m_velocity = {0.0, 0.0};
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);
	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
	m_patrolCircle.setPosition(sf::Vector2f(rand() % ScreenSize::s_height, rand() % ScreenSize::s_width));
	m_patrolCircle.setRadius(30);
	m_patrolCircle.setFillColor(sf::Color::Magenta);
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::seek(sf::Vector2f Position) const
{
	return sf::Vector2f(Position - m_position);
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::collisionAvoidance()
{
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);
	if (mostThreatening.getRadius() != 0.0)
	{		
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}
	else
	{
		avoidance *= 0.0f;
	}
	return avoidance;
}

////////////////////////////////////////////////////////////
const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	sf::CircleShape circleTemp;
	circleTemp.setRadius(0.0);
	circleTemp.setPosition(-5000, -2000);
	// The initialisation of mostThreatening is just a placeholder...
	sf::CircleShape mostThreatening = circleTemp;

	for (sf::CircleShape const circle : m_obstacles)
	{
		bool collision = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, circle);
		{
			if (collision && (mostThreatening.getRadius() == 0 || MathUtility::distance(m_position, circle.getPosition()) <
				MathUtility::distance(m_position, mostThreatening.getPosition())))
			{
				mostThreatening = circle;
			}
		}
	}
	return mostThreatening;
}

void TankAi::collisionPatrolCircle()
{
	m_patrolCircle.setPosition(sf::Vector2f(rand() % ScreenSize::s_height, rand() % ScreenSize::s_width));
}

bool TankAi::checkCollisionPatrolCircle()
{
	if (m_patrolCircle.getGlobalBounds().intersects(m_tankBase.getGlobalBounds()) || m_patrolCircle.getGlobalBounds().intersects(m_turret.getGlobalBounds()))
	{
		return true;
	}
	return false;
}

void TankAi::checkForHeatTrail(std::list<sf::CircleShape> t_heatList)
{
	std::list<sf::CircleShape>::iterator start;
	std::list<sf::CircleShape>::iterator end = t_heatList.end();
	for (start = t_heatList.begin(); start != end; start++)
	{
		if (true == MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, *start))
		{
			m_aiState = AiState::SEEK_PLAYER;
		}
	}
}

////////////////////////////////////////////////////////////
void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
}


////////////////////////////////////////////////////////////
void TankAi::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000),
	m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000));
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
 	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);
}

bool TankAi::collidesWithPlayer(Tank const & playerTank)
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getBase()))
	{
		if (AiState::RETREAT == m_aiState)
		{
			m_aiState = AiState::DEAD;
		}
		return true;
	}
	return false;
}