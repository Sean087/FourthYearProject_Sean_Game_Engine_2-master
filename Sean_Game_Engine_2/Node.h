#ifndef _NODE_H
#define _NODE_H

#include <vector>
#include <SDL.h>

struct sNode
{
	bool bObstacle = false;															// Is the node an obstruction?
	bool bVisited = false;															// Have we searched this node before?
	float fGlobalGoal;																// Distance to goal so far
	float fLocalGoal;																// Distance to goal if we took the alternative route
	int x;																			// Nodes X-position in 2D space
	int y;																			// Nodes Y-position in 2D space
	std::vector<sNode*> vecNeighbours;												// Connections to neighbours
	sNode* parent;																	// Node connecting to this node that offers shortest parent
	SDL_Rect box;
};// end struct

#endif