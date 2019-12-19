#pragma once
#include <SFML/Graphics.hpp>
#include "CollisionDetector.h"
#include "ProjectilePool.h"
#include <list>
class TankAi;

/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	/// <summary>
	/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
	/// Stores references to the texture and container of wall sprites. 
	/// Creates sprites for the tank base and turret from the supplied texture.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="texture">A reference to the container of wall sprites</param>  
	Tank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites, TankAi &t_aiTank);
	//Tank(sf::Texture const & texture, sf::Vector2f const & pos = sf::Vector2f(0,0)); //not in lab 5(changed)
	void update(double dt);
	void render(sf::RenderWindow & window);
	/// <summary>
	/// @brief Increases the speed by 1, max speed is capped at 100.
	/// 
	/// </summary>
	void increaseSpeed();

	/// <summary>
	/// @brief Decreases the speed by 1, min speed is capped at -100.
	/// 
	/// </summary>
	void decreaseSpeed();

	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359.
	/// 
	/// </summary>
	void increaseRotation();

	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0.
	/// 
	/// </summary>
	void decreaseRotation();

	/// <summary>
	/// sets tanks position at start
	/// </summary>
	void setPosition(sf::Vector2f const & pos);
	/// <summary>
	/// @brief Processes control keys and applies speed/rotation as appropriate.
	/// </summary>
	
	/// <summary>
	/// @brief Checks for collisions between the tank and the walls.
	/// 
	/// </summary>
	/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();


	/// <summary>
	/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
	/// If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
	///  (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
	/// If the tank is moving, further rotations are disabled and the previous tank position is restored.
	/// The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation 
	///  is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
	///  speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
	///  pre-collision rotation).
	/// </summary>
	void deflect();
	/// <summary>
	/// @brief Generates a request to fire a projectile.
	/// A projectile will be fired from the tip of the turret only if time limit between shots
	///  has expired. Fire requests are not queued.
	/// </summary>
	void requestFire();
	/// <summary>
	/// @brief Reads the player tank position.
	/// <returns>The tank base position.</returns>
	/// </summary>
	sf::Vector2f getPosition() const;

	void handleKeyInput();
	//increase rotation of turret
	void increaseTurretRotation();
	//decrease turret rotation
	void decreaseTurretRotation();
	//re centres turret
	void centreTurret();
	//returns turret
	sf::Sprite getTurret() const {	return m_turret; }
	sf::Sprite getBase() const { return m_tankBase; }
	void reset();
	int getRemainingBullets() const { return m_bulletsLeft; }
	bool getShowCircles() const { return m_showCircles; }

	std::list<sf::CircleShape> getHeatTrail() { return m_heatTrail; }
private:
	void initSprites(sf::Vector2f const & pos);
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	TankAi & m_aiTank;
	sf::Texture const & m_texture;
	// A reference to the container of wall sprites.
	std::vector<sf::Sprite> & m_wallSprites;
	//The tank speed.
	double m_speed{ 0.0 };
	ProjectilePool m_pool;
	// The current rotation as applied to tank base.
	double m_rotation{ 0.0 };
	//the currrent rotation of the tank turret
	double m_turretRotation{ 0.0 };
	bool m_enableRotation = true;
	sf::Vector2f m_previousPosition = { 0,0 };
	double m_previousSpeed = { 0.0 };
	double m_previousRotation = { 0.0 };
	double m_previousTurretRotation{ 0.0 };
	void adjustRotation();
	// The delay time between shots in milliseconds.
	static int const s_TIME_BETWEEN_SHOTS{ 200 };
	int m_shootTimer = 200;
	bool m_fireRequested;
	int m_bulletsLeft = 100;
	bool m_showCircles = false;
	std::list<sf::CircleShape> m_heatTrail;
	double circleTimer = 0.0f;

};
#include "TankAI.h"
