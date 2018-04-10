//
//	Enemy.h
//
// Class to represent enemy objects - Current not used

#ifndef __Enemy__
#define __Enemy__

#include "SDLGameObject.h"

class Enemy : public SDLGameObject // Inherit from SDLGameObject
{
public:

	Enemy(const Loader* pParams);

	virtual void draw();
	virtual void update();
	virtual void clean();

};
#endif