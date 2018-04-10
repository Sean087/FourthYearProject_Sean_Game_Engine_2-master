//
//	Enemy.cpp
//
//

#include "Enemy.h"

Enemy::Enemy(const Loader* pParams) : SDLGameObject(pParams)
{
}

/* draw() - Uses SDLGameObject's draw function to screen */
void Enemy::draw()
{
	SDLGameObject::draw();
}

/* update() - Defines behaviour of object */
void Enemy::update()
{
	// Custom behaviour can be specified here.
}

/* clean() - Cleans object from memory (not implemented yet) */
void Enemy::clean()
{
}