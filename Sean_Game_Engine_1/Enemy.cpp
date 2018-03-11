//
//	Enemy.cpp
//
//

#include "Enemy.h"

Enemy::Enemy(const LoaderParams* pParams) : SDLGameObject(pParams)
{
}

/* -- draw() - Uses SDLGameObject's draw function to screen -- */
void Enemy::draw()
{
	SDLGameObject::draw();
}

/* -- update() - Simple update function to move object -- */
void Enemy::update()
{
	m_x += 1;	// Override GameObject::update()
	m_y += 1;
	m_currentFrame = int(((SDL_GetTicks() / 100) % 4));
}

void Enemy::clean()
{
}