//
//	Player.cpp
//
//

#include "Player.h"

Player::Player(const Loader* pParams) : SDLGameObject(pParams)
{
}

/* draw() - Uses SDLGameObject's draw function to screen */
void Player::draw()
{
	SDLGameObject::draw();
}

/* update() - Simple update function to move object */
void Player::update()
{
}

/* clean() - cleans the object from memory (not implemented yet) */
void Player::clean()
{
}

/* setPosition() - Sets the postition of the object in the game world */
void Player::setPosition(int x, int y)
{
	SDLGameObject::setPosition(x, y);
}

/* getX() - returns the objects X-position in the game world */
int Player::getX()
{
	return SDLGameObject::getX();
}

/* getY() - returns the objects Y-position in the game world */
int Player::getY()
{
	return SDLGameObject::getY();
}

/* getIsFollowing() - Returns whether the object is following a path or not */
bool Player::getIsFollowing()
{
	return SDLGameObject::getIsFollowing();
}

/* setIsFollowing() - sets the boolean to determine whether the object is following a path or not */
void Player::setIsFollowing(bool isFollowingPath)
{
	SDLGameObject::setIsFollowing(isFollowingPath);
}