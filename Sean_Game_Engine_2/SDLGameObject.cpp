//
//	SDLGameObject.cpp
//
//

#include "SDLGameObject.h"
#include "Game.h"
#include "TextureManager.h"

/* set member variables */
SDLGameObject::SDLGameObject(const Loader * pParams) :
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
	// Generic object behaviour code can be placed here.
}

void SDLGameObject::clean()
{
	// Code can be placed here to define how the object
	// is removed form memory upon being destroyed/despanwed/etc.
}

void SDLGameObject::setPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

/* moveTowardPoint() - Move an object toward a specific (X, Y) location */
void SDLGameObject::moveTowardPoint(int x, int y)
{
	if (m_x != x) {				// If the objects X position is not equal to that of target points X position...
		if (m_x < x)			// If their X position less than the targets X... 
			m_x += 2;			// Increment position by 2
		else
			m_x -= 2;			// Else its greater than, then decrement it by 2
	}

	if (m_y != y) {				// If the objects Y position is not equal to that of target points Y position...
		if (m_y < y)			// If their Y position less than the targets Y... 
			m_y += 2;			// Increment position by 2
		else
			m_y -= 2;			// Else its greater than, then decrement it by 2
	}
}

int SDLGameObject::getX()
{
	return m_x;					// return objects X position
}

int SDLGameObject::getY()
{
	return m_y;					// return objects Y position
}

bool SDLGameObject::getIsFollowing()
{
	return m_isFollowingPath;	// return whether the object is folliwing a path or not
}

void SDLGameObject::setIsFollowing(bool isFollowingPath)
{
	m_isFollowingPath = isFollowingPath;	// sets the boolean to determine whether object is following a path or not
}