//
//	Game.cpp
//
// 

#include "Game.h"
#include "TextureManager.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include <list>
#include <stack>

Game* Game::s_pInstance = 0;														// Game singleton
bool quit = false;																	// Flag for quitting the application
SDL_Event the_event;																// SDL_Event for handling I/O events

bool renderConnections = false;														// Render underlying node connections (Toggled during runtime with 'R' key)

Player *player1;

//----------------------------------------------PATHFINDING STUFF-----------------------------------------------

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

int nodeSize = 40;																	// Size of the node (SDL_Rect) in pixels
int nodeBorder = 20;																// Size of border in pixels (Space between nodses) - NOT WORKING ATM

sNode *nodes = nullptr;																// Null pointer - Will be an array of sNode's
int nMapWidth = 16;																	// Width of the map in number of nodes
int nMapHeight = 16;																// Height of the map in numver of nodes

sNode *nodeStart = nullptr;															// The start node - null pointer as it hasn't been set yet
sNode *nodeEnd = nullptr;															// The end node - null pointer as it hasn't been set yet

int dynamicWindowSize_W = (nMapWidth*nodeSize);										// Dynamic size of the windows width - adjusts to width of map
int dynamicWindowSize_H = (nMapHeight*nodeSize);									// Dynamic size of the windows height - adjusts to height of map

std::stack<sNode*> pathToFollow;													// Stack to contain each node in the path for path following



//------------------------------------------------INITIALIZATION-------------------------------------------------

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	int flags = 0;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}// end if

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)											// Attempt to initialise SDL
	{
		std::cout << "INIT STATUS: SDL init success\n";
		
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);		// Initialize the window

		if (m_pWindow != 0)															// If window has initialized successfully...
		{
			std::cout << "INIT STATUS: Window creation success\n";
			
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);						// Create the SDL renderer

			if (m_pRenderer != 0)													// Renderer initialized sucessfully
			{
				std::cout << "INIT STATUS: Render creation success\n";							
				SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
			}// end if
			else
			{
				std::cout << "INIT STATUS: Renderer init fail\n";
				return false;														// Render initialization failed
			}// end else
		}// end if
		else
		{
			std::cout << "INIT STATIS: Window init fail\n";
			return false;															// Window initialization failed
		}// end else
	}// end if
	else
	{
		std::cout << "INIT STATUS: SDL init fail\n";
		return false;																// SDL Failed to initialize
	}// end else

	std::cout << "init success\n";
	m_bRunning = true;																// Everything initialised successfully, start the main loop
 
	if (!TextureManager::Instance()->load("assets/Player1.png", 
		"Player1", m_pRenderer))													// Load the Player image asset
	{
		return false;
	}

	player1 = new Player(new LoaderParams(20, 20, 40, 40, "Player1"));
	//m_gameObjects.push_back(new Player(new LoaderParams(100, 100, 40, 40, "Player1")));
	//m_gameObjects.push_back(new Enemy(new LoaderParams(300, 300, 240, 240, "AnimatedKnight")));


	/* ------------------------------------------------------------------------------------------
	*	Create an array of nodes to act as the map.
	*	Once created, cycle through each element in the map and
	*	populate it with a new node and some default values.
	* ------------------------------------------------------------------------------------------
	*/
	nodes = new sNode[nMapWidth * nMapHeight];
	for (int x = 0; x < nMapWidth; x++)
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y * nMapWidth + x].x = x;											// Set nodes X-position
			nodes[y * nMapWidth + x].y = y;											// Set nodes Y-position
			nodes[y * nMapWidth + x].bObstacle = false;								// Flag it as non-onstacle (traversable)
			nodes[y * nMapWidth + x].parent = nullptr;								// Set its parent to null for now
			nodes[y * nMapWidth + x].bVisited = false;								// Initially won't be visited, set vistsed to false
			nodes[y * nMapWidth + x].box.x = x*nodeSize;							// Set the position of the SDL_Rect's X-position
			nodes[y * nMapWidth + x].box.y = y*nodeSize;							// Set the position of the SDL_Rect's Y-position
			nodes[y * nMapWidth + x].box.w = nodeSize;								// Set the SDL_Rect's width
			nodes[y * nMapWidth + x].box.h = nodeSize;								// Set the SDL_Rect's height
		}// end for

	for (int x = 0; x < nMapWidth; x++)												// Loop through nodes so connections can be created
		for (int y = 0; y < nMapHeight; y++)
		{
			/*------------------------------- SET UP N, S, E, W CONECTIONS -------------------------------*/
			if (y>0)																// If node is not on the top row perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1)		
					* nMapWidth + (x + 0)]);											// Push the node NORTH of current node into its vector of neighbours

			if (y<nMapHeight - 1)													// If node is not on the bottom row perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1)
					* nMapWidth + (x + 0)]);											// Push the node SOUTH of the current node into its	vector of neighbours
			
			if (x>0)																// If node is not on the left-most collumn perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) 
					* nMapWidth + (x - 1)]);											// Push the node WEST of the current node into its vector of neighbours
			
			if (x<nMapWidth - 1)													// If node is not on the right-most collumn perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) 
					* nMapWidth + (x + 1)]);											// Push the node EAST of the current node into its vector of neighbours

			/*------------------------------- SET UP NE, SE, NW, SW CONECTIONS -------------------------------*/
			// Comment out for 4-direction connections
			if (y>0 && x>0)															// If node is not on the outter perimeter of the grid
			nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1)		
				* nMapWidth + (x - 1)]);												// Push the node SOUTH-WEST of current node into its vector of neighbours

			if (y<nMapHeight-1 && x>0)
			nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) 
				* nMapWidth + (x - 1)]);												// Push the node NORTH-WEST of current node into its vector of neighbours

			if (y>0 && x<nMapWidth-1)
			nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) 
				* nMapWidth + (x + 1)]);												// Push the node SOUTH-EAST of current node into its vector of neighbours

			if (y<nMapHeight - 1 && x<nMapWidth-1)
			nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) 
				* nMapWidth + (x + 1)]);												// Push the node NORTH-EAST of current node into its vector of neighbours
			
		}// end for

	nodeStart = &nodes[(nMapHeight / 2) * nMapWidth + 1];							// Manually position the start marker so it's not a null pointer
	nodeEnd = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];					// Manually position the end marker so it's not a null pointer
	player1->setPosition(nodeStart->x*nodeSize, nodeStart->y*nodeSize);								// Set player position to Start node
	return true;
}// end init



//----------------------------------------------UPDATING-----------------------------------------------

void Game::update()
{
	/* ____ NODE UPDATES ____ */
	int nSelectedNodeX = m_mousePosX / nodeSize;									// Get the node X-position of the click through integer division
	int nSelectedNodeY = m_mousePosY / nodeSize;									// Get the node Y-position of the click through integer division

	// SET OBSTACLE
	if (m_bClickEvent) {															// If left mouse button has been clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the map wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the map height) 
			{
				nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle =
					!nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle;		// Set selected nodes obstacle flag to the inverse of what it currently is

				AStarSolver();															// Recalculate the path by calling the solver

			}// end if
	}// end if

	// SET START NODE
	if (m_bDLClickEvent) {															// If left mouse button has been double-clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the map wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the map height) 
			{
				nodeStart = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];		// Set the start node to the node that has been double left-clicked
				//for (std::vector<GameObject*>::size_type i = 0; i != m_gameObjects.size(); i++)	// Loop through objects and perform the following
				//{
					int x = nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].x;				// Get current nodes X position
					int y = nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].y;				// Get current nodes Y position
					x = x*nodeSize;																// Upscale by factor node size for visualization 
					y = y*nodeSize;																// Upscale by factor node size for visualization
					player1->setPosition(x,y);											// Render current object to the screen
				//}// end for

				AStarSolver();															// Recalculate the path by calling the solver
			}// end if
	}// end if

	// SET END NODE
	if (m_bDRClickEvent) {															// If right mouse button has been double-clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the map wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the map height) 
			{
				nodeEnd = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];			// Set the end node to the node that has been double right-clicked

				AStarSolver();															// Recalculate the path by calling the solver

			}// end if
	}// end if

	/* ____ GAME OBJECT UPDATES ____ */
	//for (std::vector<GameObject*>::size_type i = 0; i != m_gameObjects.size(); i++)	// Loop through objects and perform the following
	//{
	//	m_gameObjects[i]->update();													// Call update function of object
	//	followPath();																// Call games follow path function ------------------------------------------------------*************************************
	//}// end for

	followPath();

}// end update



//----------------------------------------------RENDERING-----------------------------------------------

void Game::render()
{
	SDL_RenderClear(m_pRenderer);													// Clear the renderer to the draw colour

	/* ____ NODE, CONNECTION, & PATH RENDERING ____ */
	
	// * CONNECTIONS *
	SDL_SetRenderDrawColor(getRenderer(), 100, 100, 100, 255);						// Set draw color to green for drawing the nodes

	if (renderConnections) {														// If renderConnections is enabled perform the following
		for (int x = 0; x < nMapWidth; x++)												// Loop through nodes in grid
			for (int y = 0; y < nMapHeight; y++)
			{
				for (auto n : nodes[y*nMapWidth + x].vecNeighbours)						// Loop through neighbours of current node
				{
					SDL_RenderDrawLine(getRenderer(), x*nodeSize +
						nodeSize / 2, y*nodeSize + nodeSize / 2,
						n->x*nodeSize + nodeSize / 2, n->y*nodeSize + nodeSize / 2);	// Draw a line from node to its neighbour (signifiying a connection)
				}// end for
			}// end for
	}// end if

	// * NODES *
	SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);						// Set draw color to muted purple for drawing the nodes

	for (int x = 0; x < nMapWidth; x++) {
		for (int y = 0; y < nMapHeight; y++)
		{
			if (nodes[y * nMapWidth + x].bObstacle == true) {						// If the current node is flagged as an obstacle perform the following
				SDL_SetRenderDrawColor(getRenderer(), 155, 155, 155, 255);				// Set draw color to grey to make the node stand out from the green
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with grey color so it is visably an obstacle
			}// end if
			else if (&nodes[y * nMapWidth + x] == nodeStart) {						// Else if the node has been flagged as the start node perform the following
				SDL_SetRenderDrawColor(getRenderer(), 0, 255, 0, 255);				// Set draw colour to yellow
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with yellow color to signify the strat node
			}// end else if
			else if (&nodes[y * nMapWidth + x] == nodeEnd) {						// Else if the node has been flagged as the end node perform the following
				SDL_SetRenderDrawColor(getRenderer(), 255, 0, 0, 255);					// Set draw colour to red
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with red color to signify the end node
			}// end else if
			else if (nodes[y * nMapWidth + x].bVisited) {							// Else if the node has been flagged as visited perform the following
				SDL_SetRenderDrawColor(getRenderer(), 255, 155, 0, 255);				// Set draw colour to orange
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Draw the node (outline)
			}// end else if
			else {																	// Else it is a traversable node and not an obstacle, perform the following
				SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);				// Set draw color back to muted purple for drawing traversable nodes
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Render the node as a regular green node (traversable)
			}// end else
		}// end for
	}// end for

	// * PATH *	
	if (nodeEnd != nullptr)															// If the end node is not a null pointer perform the following
	{
		sNode *p = nodeEnd;																// Create a new temp node to store current, from which the parent can be referenced
		while (p->parent != nullptr)													// Loop until we reach a node with no parent (Start Node)
		{
			SDL_SetRenderDrawColor(getRenderer(), 0, 255, 255, 255);						// Set draw colour to Cyan for path line
			SDL_RenderDrawLine(getRenderer(), p->x*nodeSize + nodeSize / 2, 
				p->y*nodeSize + nodeSize / 2,
				p->parent->x*nodeSize + nodeSize / 2, 
				p->parent->y*nodeSize + nodeSize / 2);										// Draw a line from current node p (x1, y1) to p's parent (x2, y2)

			p = p->parent;																	// Set next node to this node's parent
		}// end while
	}// end if

	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);							// Set draw colour to black for background

	/* ____ GAME OBJECT RENDERING ____ */
	//for (std::vector<GameObject*>::size_type i = 0; i != m_gameObjects.size(); i++)	// Loop through objects and perform the following
	//{
	//	m_gameObjects[i]->draw();														// Render current object to the screen
	//}// end for

	player1->draw();

	SDL_RenderPresent(m_pRenderer); // draw to the screen
}// end render


//--------------------------------------------------------------------------------------------------------------


void Game::handleEvents()
{
	//-------------------- HANDLE EVENTS FROM KEYS AND MOUSE BUTTONS --------------------
	while (SDL_PollEvent(&the_event))												// Poll for specific events
	{
		//if (the_event.key.keysym.sym == SDLK_ESCAPE) quit = true;					// CAUSE OF WEIRD CRASH WHEN CURSOR ON LEFT SIDE OF WINDOW
		if (the_event.type == SDL_KEYDOWN && 
			the_event.key.keysym.sym == SDLK_ESCAPE) quit = true;					// If escape key is hit, shut down the program
		else if (the_event.type == SDL_QUIT) quit = true;							// If user press the close button shut down the program

		if (the_event.type == SDL_MOUSEBUTTONDOWN && the_event.button.button == SDL_BUTTON_LEFT) {								// If mouse button is pressed perform the following
			int clickX, clickY;															// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&clickX, &clickY);										// Get the location of the mouse click, store in temp variables
			setMouseClickX(clickX);														// Store X-position in variables used by nodes
			setMouseClickY(clickY);														// Store Y-position in variables used by nodes
			setClickEvent(true);														// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN && the_event.button.button == SDL_BUTTON_RIGHT) {								// If mouse button is pressed perform the following
			AStarSolver();
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN && the_event.button.button == SDL_BUTTON_LEFT && the_event.button.clicks == 2) {								// If mouse button is pressed perform the following
			int DLClickX, DLClickY;															// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&DLClickX, &DLClickY);										// Get the location of the mouse click, store in temp variables
			setMouseClickX(DLClickX);														// Store X-position in variables used by nodes
			setMouseClickY(DLClickY);														// Store Y-position in variables used by nodes
			setDLClickEvent(true);															// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN && the_event.button.button == SDL_BUTTON_RIGHT && the_event.button.clicks == 2) {								// If mouse button is pressed perform the following
			int DRClickX, DRClickY;															// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&DRClickX, &DRClickY);										// Get the location of the mouse click, store in temp variables
			setMouseClickX(DRClickX);														// Store X-position in variables used by nodes
			setMouseClickY(DRClickY);														// Store Y-position in variables used by nodes
			setDRClickEvent(true);															// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_KEYDOWN) {										// If a keyboard button is pressed performt he following
			switch (the_event.key.keysym.sym) {											// Switch for keyboard event to check key
				case SDLK_r:															// 'R' key is pressed, perform the following
					renderConnections = !renderConnections;								// Set renderConnections to its inverse (flag for rendering connections between nodes)
					break;
			}// end swtitch
		}// end if
	}// end while
}// end handleEvents


//--------------------------------------------------------------------------------------------------------------


void Game::clean() {
	std::cout << "cleaning game\n";
	SDL_DestroyWindow(m_pWindow);													// Destroy the window
	SDL_DestroyRenderer(m_pRenderer);												// Destroy the renderer
	SDL_Quit();																		// Shut down SDL
}// end clean


//--------------------------------------------------------------------------------------------------------------

bool Game::AStarSolver()
{
	if (!pathToFollow.empty()) {
		while (!pathToFollow.empty())
		{
			pathToFollow.pop();
		}
	}
	/* ____ PREPERATION ____ */

	for (int x = 0; x < nMapWidth; x++)												// Resets each node in the map to default states
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y*nMapWidth + x].bVisited = false;								// We haven't visited any nodes yet
			nodes[y*nMapWidth + x].fGlobalGoal = INFINITY;							// Default is Infinity so we can consider it when searching
			nodes[y*nMapWidth + x].fLocalGoal = INFINITY;							// Default is also Infinity so we can consider it when searching
			nodes[y*nMapWidth + x].parent = nullptr;								// All nodes begin with no parent
		}

	/* ------------------------------------------------------------------------------------------
	 *	Since we do not know the distances between nodes for heuristic, use Pythagoras Theorom.
	 * 
	 *  Used since it's effective for finding distance between center-points of nodes:
	 *		- (a^a) + (b^b) = (c^c)
	 *		- Example:
	 *			Node a = (15, 8)
	 *			Node b = (14, 8)
	 *			(15 - 14)^2 + (8 - 8)^2 = 1 
	 *			sqrt(1) = 1
	 *			c = 1
	 * ------------------------------------------------------------------------------------------
	 */
	auto distance = [](sNode* a, sNode* b)
	{
		/*std::cout << "Distannce between A: (" << a->x << ", " << a->y << ") and B: (" << b->x << ", " << b->y << ") is: " 
		<< sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y)) << std::endl;*/
		return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
	};// end distance

	auto heuristic = [distance](sNode* a, sNode* b)									// Heuristic is the distance between points so use distance function
	{
		return distance(a, b);
	};// end heuristic

	sNode *nodeCurrent = nodeStart;													// Set the current node to the beginning
	nodeStart->fLocalGoal = 0.0f;													// The local goal is 0 since we didn't start searching yet
	nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);							// Global goal = distance between start node and end node

	/* ------------------------------------------------------------------------------------------
	 *	While the algorithm is searching, newly discovered nodes are added to the not-tested list,
	 *	which queues them for testing for a potential path.
	 * 
	 *  The first node we want to test if the start node so we queue that node first.
	 *
	 *	NOTE TO SELF: Attempt to change list to priority queue to avoid manual sorting.
	 * ------------------------------------------------------------------------------------------
	 */
	std::list<sNode*> listNotTestedNodes;											// Not tested list where new nodes are queded for testing
	listNotTestedNodes.push_back(nodeStart);										// Add the start node for testing

	/* ------------------------------------------------------------------------------------------
	*	OPTION 1: (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
	*		- While there are nodes to test && the end node has not been reached run A*
	*		- This will run until the end node is found, which means that it will find
	*		  A path but it MAY NOT BE THE SHORTEST PATH.
	*	
	*	OPTION 2: (!listNotTestedNodes.empty())
	*		- While there there are nodes to test, run A*.
	*		- This WILL FIND THE SHORTEST PATH even if end node has been reached.
			- Considers all nodes.
	* ------------------------------------------------------------------------------------------
	*/
	//while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)					// Find THE SHORTEST PATH (Horrible brute force method) remove following:  "&& nodeCurrent != nodeEnd"
	while (!listNotTestedNodes.empty() && !nodeEnd->bVisited)						// Finds the shortest path - while there are nodes to test and the end node has not been visited
	{
		listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs)				
		{ return lhs->fGlobalGoal < rhs->fGlobalGoal; });							// Sort all untested nodes by shortest global goal (possible shortest path)
				
		while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)	// Potential to have already visited nodes at front of list therefore:
			listNotTestedNodes.pop_front();												// Remove them from the list as they have already been visited

		if (listNotTestedNodes.empty())												// If there are no valid nodes to test, abort.
			break;

		nodeCurrent = listNotTestedNodes.front();									// Set node at front of the list to current node for testing
		nodeCurrent->bVisited = true;												// Only need to test a node once so set it to visited

		for (auto nodeNeighbour : nodeCurrent->vecNeighbours)						// Check every neighbour of the current node
		{
			if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)			// If the neighbour has not been visited and is not an obstacle:
				listNotTestedNodes.push_back(nodeNeighbour);							// add it to the not-tested list for future testing

			float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + 
				distance(nodeCurrent, nodeNeighbour);								// Calculate the neighbours possibly lowest parent distance

			/* ------------------------------------------------------------------------------------------
			*	If we decide to continue the path through the current neighbour due to it having the
			*	lowest distance, set the neighbouring nodes parent to the current node so we can
			*	use it the current node as the source of the path from the neighbour.
			*
			*	Then update the neighbours local goal to its distance from its parent plus its parents
			*	local goal.
			*
			*	Basically update our current path.
			* ------------------------------------------------------------------------------------------
			*/
			if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
			{
				nodeNeighbour->parent = nodeCurrent;
				nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

				/* ------------------------------------------------------------------------------------------
				 *	Update the global goal of the neighbour using the heuristic to get the distance from
				 *	the neighbour to the end goal.
				 *
				 *	When sort the nodes-to-test list, it may see that the current path has become too long
				 *	and ditch it in favour of a new shorter path. This bias is what I found to be the
				 *	most interesting part of A*.
				 * ------------------------------------------------------------------------------------------
				 */
				nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
			}// end if
		}// end for
	}// end while

	sNode *p = nodeEnd;																// Create a temp node to store current node
	while (p != NULL) {																// Loop until we reach a node with null parent (Start node)
		pathToFollow.push(p);														// Push the node onto the stack
		p = p->parent;																// Set current node to current nodes parent
	}// end while

	return true;
}// end AStarSolver


//--------------------------------------------------------------------------------------------------------------

bool Game::followPath() {
	//-------------------- MAKE OBJECT FOLLOW THE A* PATH --------------------
	if (nodeEnd != nullptr && !pathToFollow.empty())											// If the end node is not a null pointer perform the following
	{
		sNode *currentPoint = pathToFollow.top();												// Current point to move towards is set to the top node on the stack (closest node)	
			
		player1->moveTowardPoint(currentPoint->x*nodeSize, currentPoint->y*nodeSize);			// Move player towards top node in stack
		
		if (player1->getX() == currentPoint->x*nodeSize)										// If the players X-position matches the X-position of the node...
		{
			if (player1->getY() == currentPoint->y*nodeSize)									// If the players Y-position matches the Y-position of the node...
			{
					pathToFollow.pop();																// Pop the node from the stack because Player has reached its position
			}// end if
		}// end if
	}// end if

	return true;
}// end followPath

//bool Game::followPath() {
//	//-------------------- MAKE OBJECT FOLLOW THE A* PATH --------------------
//	for (std::vector<GameObject*>::size_type i = 0; i != m_gameObjects.size(); i++)	// Loop through objects and perform the following
//	{
//		if (nodeEnd != nullptr && !pathToFollow.empty())													// If the end node is not a null pointer perform the following
//		{
//			sNode *currentPoint = pathToFollow.top();														// Current point to move towards is set to the top node on the stack (closest node)	
//		
//			m_gameObjects[i]->moveTowardPoint(currentPoint->x*nodeSize, currentPoint->y*nodeSize);			// Move object toward current node on stack
//		
//			if (m_gameObjects[i]->getX() == currentPoint->x*nodeSize)										// If the objects X-position matches the X-position of the node...
//			{
//				if (m_gameObjects[i]->getY() == currentPoint->y*nodeSize)									// If the objects Y-position matches the Y-position of the node...
//				{
//					pathToFollow.pop();																			// Pop the node from the stack because object has reached its position
//				}// end if
//			}// end if
//		}// end if
//	}// end for
//
//	return true;
//}// end followPath


int main(int argc, char* argv[])
{
	std::cout << "game init attempt...\n";
	if (TheGame::Instance()->init("Seans Game Framework 2 - v0.2", 100, 100, dynamicWindowSize_W, dynamicWindowSize_H, false))
	{
		std::cout << "game init success!\n";
		while (TheGame::Instance()->running() && quit == false)						// If the game is running, peform the following
		{

			TheGame::Instance()->handleEvents();									// Poll to see if user closed the window
			TheGame::Instance()->update();											// Update Nodes and Game Objects
			TheGame::Instance()->render();											// Render Nodes and Game Objects to the screen

			TheGame::Instance()->setClickEvent(false);								// Set m_bClickEvent flag to false so a new click can be registered
			TheGame::Instance()->setDLClickEvent(false);								// Set m_bDLClickEvent flag to false so a new click can be registered
			TheGame::Instance()->setDRClickEvent(false);								// Set m_bDRClickEvent flag to false so a new click can be registered


			SDL_Delay(10);															// add the delay

		}// end while
	}// end if
	else																				// Else the program did not initalize properly
	{
		std::cout << "game init failure - " << SDL_GetError() << "\n";						// Print error
		return -1;																			// Return with exit status -1 (error encountered)
	}// end else

	std::cout << "game closing...\n";												// Loop has been broken, closing the game
	TheGame::Instance()->clean();													// Perform shut-down operation clean()

	//system("pause");																// Keep console open after quitting for debugging purposes
	return 0;
}// end main