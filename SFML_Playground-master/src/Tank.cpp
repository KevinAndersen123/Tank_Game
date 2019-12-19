#include "Tank.h"

Tank::Tank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites, TankAi &t_aiTank)
: m_texture(texture)
, m_enableRotation(true),
m_wallSprites(wallSprites),
m_aiTank(t_aiTank)
{
	initSprites(sf::Vector2f{ 0,0 });
}

void Tank::setPosition(sf::Vector2f const & pos)
{
	m_tankBase.setPosition(pos);
	m_turret.setPosition(pos);
	
}

void Tank::update(double dt)
{
	handleKeyInput();
	sf::Vector2f pos = { m_tankBase.getPosition().x,m_tankBase.getPosition().y };
	m_previousPosition = pos;
	pos.x = m_tankBase.getPosition().x + std::cos(m_rotation * (3.14159265359 /180)) * m_speed * (dt/ 1000);
	pos.y = m_tankBase.getPosition().y + std::sin(m_rotation * (3.14159265359 /180)) * m_speed * (dt / 1000);
	m_tankBase.setRotation(m_rotation);
	m_turret.setRotation(m_turretRotation);
	setPosition(pos);

	if (checkWallCollision())
	{
		deflect();
	}
	if (m_fireRequested)
	{
		m_shootTimer -= dt;
		if (m_shootTimer <= 0)
		{
			m_shootTimer = s_TIME_BETWEEN_SHOTS;
			m_fireRequested = false;
		}
	}
	m_pool.update(dt, m_wallSprites);
 	if (m_pool.checkCollisionTank(m_aiTank.getTurretSprite(), m_aiTank.getBaseSprite()))
	{
		m_aiTank.takeDamage();
	}

	circleTimer -= dt;
	if (circleTimer < (0.0))
	{
		sf::CircleShape circleTrail;
		circleTrail.setPosition(m_tankBase.getPosition());
		circleTrail.setFillColor(sf::Color::Red);
		circleTrail.setRadius(15.0f);
		circleTrail.setOrigin(7.5, 7.5);
		m_heatTrail.push_front(circleTrail);
		if (m_heatTrail.size() > 30)
		{
			m_heatTrail.pop_back();
		}
		circleTimer = 100;
	}
}

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
	if (m_showCircles == true)
	{
		for (auto &trail : m_heatTrail)
		{
			window.draw(trail);
		}
	}
	
	m_pool.render(window);
}


void Tank::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		increaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		decreaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		centreTurret();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		if (m_bulletsLeft > 0)
		{
			requestFire();
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
	{
		m_showCircles = true;
	}
	else
	{
		m_showCircles = false;
	}
}

void Tank::increaseTurretRotation()
{
	if (m_enableRotation == true) 
	{
		m_turretRotation += 1;
		if (m_turretRotation == 360.0)
		{
			m_turretRotation = 0;
		}
	}
}

void Tank::decreaseTurretRotation()
{
	m_previousTurretRotation = m_turretRotation;
	if (m_enableRotation == true)
	{
		m_turretRotation -= 1;
		if (m_turretRotation == 0.0)
		{
			m_turretRotation = 359.0;
		}
	}
}

void Tank::centreTurret()
{
	if (m_enableRotation == true)
		m_turretRotation = m_rotation;
}

void Tank::reset()
{
	m_bulletsLeft = 100;
	m_speed = 0.0f;
	m_rotation = 0.0;
	m_turretRotation = 0.0;
	m_heatTrail.clear();
}

void Tank::initSprites(sf::Vector2f const & pos)
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	m_tankBase.setPosition(pos);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setPosition(pos);

}
void Tank::increaseSpeed()
{
	if (m_speed < 100.0)
	{
		m_speed += 1;
	}
}

////////////////////////////////////////////////////////////
void Tank::decreaseSpeed()
{
	if (m_speed > -100.0)
	{
		m_speed -= 1;
	}
}

////////////////////////////////////////////////////////////
void Tank::increaseRotation()
{
	m_previousRotation = m_rotation;
	increaseTurretRotation();
	if (m_enableRotation == true)
	{
		m_rotation += 1;
		if (m_rotation == 360.0)
		{
			m_rotation = 0;
		}
	}
}

////////////////////////////////////////////////////////////
void Tank::decreaseRotation()
{
	
	m_previousRotation = m_rotation;
	decreaseTurretRotation();
	if (m_enableRotation == true)
	{
		m_rotation -= 1;
		if (m_rotation == 0.0)
		{
			m_rotation = 359.0;
		}
	}
}

////////////////////////////////////////////////////////////
bool Tank::checkWallCollision()
{
	for (sf::Sprite const & sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			return true;
		}
	}
	return false;
}

void Tank::deflect()
{
	// In case tank was rotating.
	adjustRotation();

	// If tank was moving.
	if (m_speed != 0)
	{
		// Temporarily disable turret rotations on collision.
		m_enableRotation = false;
		// Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		// Apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 8;
		}
		else
		{
			m_speed = -8;
		}
	}
	
	m_enableRotation = true;
}
void Tank::adjustRotation()
{
	// If tank was rotating...
	if (m_rotation != m_previousRotation)
	{
		// Work out which direction to rotate the tank base post-collision.
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}
	// If turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotation)
	{
		// Set the turret rotation back to it's pre-collision value.
		m_turretRotation = m_previousTurretRotation;
	}
	m_enableRotation = true;
}
////////////////////////////////////////////////////////////
void Tank::requestFire()
{
	m_fireRequested = true;
	if (m_shootTimer == s_TIME_BETWEEN_SHOTS)
	{
		sf::Vector2f tipOfTurret(m_turret.getPosition().x + 2.0f, m_turret.getPosition().y);
		tipOfTurret.x += std::cos(MathUtility::DEG_TO_RAD  * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		tipOfTurret.y += std::sin(MathUtility::DEG_TO_RAD  * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		m_pool.create(m_texture, tipOfTurret.x, tipOfTurret.y, m_turret.getRotation());
		m_bulletsLeft--;
	}
	
}
////////////////////////////////////////////////////////////
sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}


