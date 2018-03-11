//
//	SDLGameObject.h
//
//

#ifndef __SDLGameObject__
#define __SDLGameObject__

#include "GameObject.h"

class SDLGameObject : public GameObject
{
public:

	SDLGameObject(const LoaderParams* pParams);

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void setPosition(int x, int y);
	virtual void moveTowardPoint(int x, int y);
	virtual int getX();
	virtual int getY();
	virtual bool getIsFollowing();
	virtual void setIsFollowing(bool isFollowingPath);

protected:

	int m_x;
	int m_y;

	int m_width;
	int m_height;

	int m_currentRow;
	int m_currentFrame;

	bool m_isFollowingPath;

	std::string m_textureID;

};
#endif
