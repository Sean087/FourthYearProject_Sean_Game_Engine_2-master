//
//	Name:			LoaderParams.h
//
//	Description:	Class that takes values into its constructor and sets them
//					as member variables that can then be accessed to set the 
//					initial values of an object. Alleviates the need to define
//					a load() function for every class.
//

#ifndef __LoaderParams__
#define __LoaderParams__

#include <string>
#include <SDL.h>

class LoaderParams
{
public:

	LoaderParams(int x, int y, int width, int height, std::string textureID)
		: 
	m_x(x), 
	m_y(y), 
	m_width(width), 
	m_height(height), 
	m_textureID(textureID)
	{
	}

	int getX() const { return m_x; }
	int getY() const { return m_y; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	std::string getTextureID() const { return m_textureID; }

private:

	int m_x;
	int m_y;

	int m_width;
	int m_height;

	std::string m_textureID;

};
#endif