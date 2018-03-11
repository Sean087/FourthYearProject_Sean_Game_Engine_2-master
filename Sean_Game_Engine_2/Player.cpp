//
//	Player.cpp
//
//

#include "Player.h"

Player::Player(const LoaderParams* pParams) : SDLGameObject(pParams)
{
}

/* -- draw() - Uses SDLGameObject's draw function to screen -- */
void Player::draw()
{
	SDLGameObject::draw();
}

/* -- update() - Simple update function to move object -- */
void Player::update()
{
}

void Player::clean()
{
}

void Player::setPosition(int x, int y)
{
	SDLGameObject::setPosition(x, y);
}


int Player::getX()
{
	return SDLGameObject::getX();
}

int Player::getY()
{
	return SDLGameObject::getY();
}

bool Player::getIsFollowing()
{
	return SDLGameObject::getIsFollowing();
}

void Player::setIsFollowing(bool isFollowingPath)
{
	SDLGameObject::setIsFollowing(isFollowingPath);
}