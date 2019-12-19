#include "ProjectilePool.h"

////////////////////////////////////////////////////////////
void ProjectilePool::create(sf::Texture const & texture, double x, double y, double rotation)
{
	// If no projectiles available, simply re-use the next in sequence.
	if (m_poolFull)
	{
		m_nextAvailable = (m_nextAvailable + 1) % s_POOL_SIZE;
	}
	
	m_projectiles.at(m_nextAvailable).init(texture, x, y, rotation);
}

////////////////////////////////////////////////////////////
void ProjectilePool::update(double dt, std::vector<sf::Sprite> & wallSprites)
{	
	// The number of active projectiles.
	int activeCount = 0;
	// Assume the pool is not full initially.
	m_poolFull = false;
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		if( !m_projectiles.at(i).update(dt, wallSprites))
		{
			// If this projectile has expired, make it the next available.
			m_nextAvailable = i;
		}
		else
		{
			// So we know how many projectiles are active.
			activeCount++;
		}
	}
	// If no projectiles available, set a flag.
	if (s_POOL_SIZE == activeCount)
	{		
		m_poolFull = true;
	}
}

////////////////////////////////////////////////////////////
void ProjectilePool::render(sf::RenderWindow & window)
{
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		// If projectile is active...
		if (m_projectiles.at(i).inUse())
		{
			window.draw(m_projectiles.at(i).m_projectile);
		}
	}
}

bool ProjectilePool::checkCollisionTank(sf::Sprite t_aiTankTurret, sf::Sprite t_aiTankBase)
{	
	bool result = false;
	for (int i = 0; i < s_POOL_SIZE; i++)
	{				
		if (m_projectiles[i].inUse())
		{
 			if (CollisionDetector::collision(t_aiTankBase, m_projectiles[i].getBulletSprite()) ||
				CollisionDetector::collision(t_aiTankTurret, m_projectiles[i].getBulletSprite()))
			{
				m_projectiles[i].kill();
				result = true;
			}
		}
	}
	return result;
}
