//
//	Player.h
//
//

#ifndef __Player__
#define __Player__

#include "SDLGameObject.h"

class Player : public SDLGameObject 
{
public:

	Player(const LoaderParams* pParams);

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void setPosition(int x, int y);
	virtual int getX();
	virtual int getY();
	virtual bool getIsFollowing();
	virtual void setIsFollowing(bool isFollowingPath);

};
#endif