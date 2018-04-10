//
//	TextureManager.h
//
//

#include "TextureManager.h"

TextureManager* TextureManager::s_pInstance = 0;		// Static instance of texture manager singleton

bool TextureManager::load(std::string fileName, std::string id, SDL_Renderer* pRenderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());

	if (pTempSurface == 0)
	{
		return false;
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);

	SDL_FreeSurface(pTempSurface);
	
	if (pTexture != 0)
	{
		m_textureMap[id] = pTexture;					// Add texture to map
		return true;									// Success, return true
	}

	return false;										// Something went wrong, return false
}// end load

void TextureManager::draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip)
{
	SDL_Rect srcRect;					// Defines what part of the texture to drawn
	SDL_Rect destRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);		// Use
}// end draw

void TextureManager::drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, SDL_RendererFlip flip)
{	
	SDL_Rect srcRect;						// Defines what part of the texture to drawn
	SDL_Rect destRect;
	srcRect.x = width * currentFrame;		// X position of the row on the texture to draw for animation
	srcRect.y = height * (currentRow - 1);	// Y position of the row on the texture to draw for animation
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
}// end drawFrame