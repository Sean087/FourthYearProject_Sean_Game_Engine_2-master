//
//	GameObject.h
//
//

#ifndef __GameObject__
#define __GameObject__

#include "LoaderParams.h"

class GameObject
{
public:

	/* 
	  • Virtual because they are being called through a pointer.
	  • Set to 0 to make class abstract (pure virtual).
	  • Forces derived classes to declare and implement these functions. 
	*/
	virtual void draw()=0;
	virtual void update()=0;
	virtual void clean()=0;
	virtual void setPosition(int x, int y) = 0;
	virtual void moveTowardPoint(int x, int y)=0;
	virtual int getX()=0;
	virtual int getY()=0;

protected:

	GameObject(const LoaderParams* pParams) {}

	virtual ~GameObject() {}

};
#endif