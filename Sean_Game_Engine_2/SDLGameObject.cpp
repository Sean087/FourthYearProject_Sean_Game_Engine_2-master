//
//	SDLGameObject.cpp
//
//

#include "SDLGameObject.h"
#include "Game.h"
#include "TextureManager.h"

/* set member variables */
SDLGameObject::SDLGameObject(const LoaderParams * pParams) :
	GameObject(pParams)
{
	m_x = pParams->getX();
	m_y = pParams->getY();
	m_width = pParams->getWidth();
	m_height = pParams->getHeight();
	
	m_textureID = pParams->getTextureID();

	m_currentRow = 1;
	m_currentFrame = 1;
}

void SDLGameObject::draw()
{
	/*TextureManager::Instance()->drawFrame(m_textureID, m_x, m_y,
		m_width, m_height, m_currentRow, m_currentFrame,
		TheGame::Instance()->getRenderer());*/
	TextureManager::Instance()->draw(m_textureID, m_x, m_y,
		m_width, m_height, TheGame::Instance()->getRenderer());
}

void SDLGameObject::update()
{

}

void SDLGameObject::clean()
{

}

void SDLGameObject::setPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void SDLGameObject::moveTowardPoint(int x, int y)
{
	if (m_x != x) {
		if (m_x < x)
			m_x += 5;
		else
			m_x -= 5;
	}

	if (m_y != y) {
		if (m_y < y)
			m_y += 5;
		else
			m_y -= 5;
	}
}

int SDLGameObject::getX()
{
	return m_x;
}

int SDLGameObject::getY()
{
	return m_y;
}

bool SDLGameObject::getIsFollowing()
{
	return m_isFollowingPath;
}

void SDLGameObject::setIsFollowing(bool isFollowingPath)
{
	m_isFollowingPath = isFollowingPath;
}