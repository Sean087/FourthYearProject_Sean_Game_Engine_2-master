//
//	Name:			Game.cpp
//
//	Description:	Defines the code making up the game loop. Creates a simple scene with
//					a grid of Nodes and Player object. The A* pathfindinglibrary is used
//					to find potentially optimal paths between two nodes in the grid.
//					Handles input events, rendering, updating, initialisation, etc.
//

#include "Game.h"
#include "TextureManager.h"
#include "Player.h"
#include "Enemy.h"
#include "PathAlgorithm.h"															// Include path finding algorithm library

Game* Game::s_pInstance = 0;														// Game singleton
SDL_Event the_event;																// SDL_Event for handling I/O events
Player *player1;																	// Create a new player object

//----------------------------------------------PATHFINDING STUFF-----------------------------------------------


bool bRenderConnections = false;													// Render underlying node connections (Toggled during runtime with 'R' key)
bool bFollowPath = false;															// Begin following a path (or pause path following in progress)

int nodeSize = 40;																	// Size of the node (SDL_Rect) in pixels
int nMapWidth = 16;																	// Width of the map in number of nodes
int nMapHeight = 16;																// Height of the map in numver of nodes
int dynamicWindowSize_W = (nMapWidth*nodeSize);										// Dynamic size of the windows width - adjusts to width of map
int dynamicWindowSize_H = (nMapHeight*nodeSize);									// Dynamic size of the windows height - adjusts to height of map

Node *nodes = nullptr;																// Null pointer - Will be an array of sNode's
Node *startNode = nullptr;															// The start node - null pointer as it hasn't been set yet
Node *endNode = nullptr;															// The end node - null pointer as it hasn't been set yet

std::stack<Node*> pathToFollow;														// Stack to contain each node in the path for path following


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

	std::cout << "INIT STATUS: Game init success!\n";
	m_bRunning = true;																// Everything initialised successfully, start the main loop

	if (!TextureManager::Instance()->load("assets/Ghost2.png",
		"Player1", m_pRenderer))													// Load the Player image asset
	{
		return false;
	}

	player1 = new Player(new Loader(20, 20, 40, 40, "Player1"));					// Instanciate player1 object

	/* ------------------------------------------------------------------------------------------
	*	Create an array of nodes to act as the map.
	*	Once created, cycle through each element in the map and
	*	populate it with a new node and some default values.
	* ------------------------------------------------------------------------------------------
	*/
	nodes = new Node[nMapWidth * nMapHeight];
	for (int x = 0; x < nMapWidth; x++)
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y * nMapWidth + x].x = x;											// Set nodes X-position
			nodes[y * nMapWidth + x].y = y;											// Set nodes Y-position
			nodes[y * nMapWidth + x].bObstacle = false;								// Flag it as non-onstacle (traversable)
			nodes[y * nMapWidth + x].parent = nullptr;								// Set its parent to null for now
			nodes[y * nMapWidth + x].bVisited = false;								// Initially won't be visited, set vistsed to false
			nodes[y * nMapWidth + x].box.x = x * nodeSize;							// Set the position of the SDL_Rect's X-position
			nodes[y * nMapWidth + x].box.y = y * nodeSize;							// Set the position of the SDL_Rect's Y-position
			nodes[y * nMapWidth + x].box.w = nodeSize;								// Set the SDL_Rect's width
			nodes[y * nMapWidth + x].box.h = nodeSize;								// Set the SDL_Rect's height
		}// end for

	for (int x = 0; x < nMapWidth; x++)												// Loop through nodes so connections can be created
		for (int y = 0; y < nMapHeight; y++)
		{
			/*------------------------------- SET UP N, S, E, W CONECTIONS -------------------------------*/
			if (y>0)																// If node is not on the top row perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1)
					* nMapWidth + x]);												// Push the node NORTH of current node into its vector of neighbours

			if (y<nMapHeight - 1)													// If node is not on the bottom row perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1)
					* nMapWidth + x]);												// Push the node SOUTH of the current node into its	vector of neighbours

			if (x>0)																// If node is not on the left-most collumn perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[y*nMapWidth 
					+ (x - 1)]);													// Push the node WEST of the current node into its vector of neighbours

			if (x<nMapWidth - 1)													// If node is not on the right-most collumn perform the following
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[y*nMapWidth
					+ (x + 1)]);													// Push the node EAST of the current node into its vector of neighbours

			/*------------------------------- SET UP NE, SE, NW, SW CONECTIONS -------------------------------*/
			// Comment out the below four if statements for 4-direction connections
			if (y>0 && x>0)															// If node is not on the outter perimeter of the grid
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1)
					* nMapWidth + (x - 1)]);										// Push the node SOUTH-WEST of current node into its vector of neighbours

			if (y<nMapHeight - 1 && x>0)
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1)
					* nMapWidth + (x - 1)]);										// Push the node NORTH-WEST of current node into its vector of neighbours

			if (y>0 && x<nMapWidth - 1)
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1)
					* nMapWidth + (x + 1)]);										// Push the node SOUTH-EAST of current node into its vector of neighbours

			if (y<nMapHeight - 1 && x<nMapWidth - 1)
				nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1)
					* nMapWidth + (x + 1)]);										// Push the node NORTH-EAST of current node into its vector of neighbours

		}// end for

	startNode = &nodes[(nMapHeight / 2) * nMapWidth + 1];							// Manually position the start node so it's not a null pointer
	endNode = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];					// Manually position the end node so it's not a null pointer
	player1->setPosition(startNode->x*nodeSize, startNode->y*nodeSize);				// Set player position to Start node
	
	std::cout << "MAP INIT STATUS: Map and connections successfully initialized!\n";

	return true;
}// end init


//----------------------------------------------UPDATING-----------------------------------------------


void Game::update()
{
	/* ____ NODE UPDATES ____ */
	int nSelectedNodeX = m_mousePosX / nodeSize;										// Get the node X-position of the click through integer division
	int nSelectedNodeY = m_mousePosY / nodeSize;										// Get the node Y-position of the click through integer division

    // SET OBSTACLE
	if (m_bClickEvent && !bFollowPath) {												// If left mouse button has been clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the grid wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the grid height) 
			{
				if (&nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX] != startNode &&
					&nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX] != endNode)		// If the node clicked on is not the start or end node
				{
					nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle =
						!nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle;	// Set selected nodes obstacle flag to the inverse of what it currently is

					AStarSolver();														// Recalculate the path by calling the solver
				}// end if
				else
				{
					std::cout << "ERROR: Cannot make this node an obstacle!\n";
				}// end else
			}// end if
	}// end if
	else if (m_bClickEvent && bFollowPath)
	{
		std::cout << "ERROR: Cannot place obstacle while object is moving.\n";
	}// end else

	 // SET START NODE
	if (m_bDLClickEvent && !bFollowPath) {												// If left mouse button has been double-clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the map wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the map height) 
			{
				if (&nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX] != endNode)		// If the node clicked on is not the end node
				{
					startNode = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];	// Set the start node to the node that has been double left-clicked

					AStarSolver();														// Recalculate the path by calling the solver
				}
				else
				{
					std::cout << "ERROR: Cannot make end node the start!\n";
				}// end else
			}// end if
	}// end if
	else if (m_bDLClickEvent && bFollowPath)
	{
		std::cout << "ERROR: Cannot place start node while object is moving.\n";
	}// end else

	 // SET END NODE
	if (m_bDRClickEvent && !bFollowPath) {												// If right mouse button has been double-clicked perform the following
		if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)							// If the mouse click is in a valid location (inside the map wdith)
			if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)						// If the mouse click is in a valid location (inside the map height) 
			{
				if (&nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX] != startNode)	// If the node clicked on is not the start node
				{
					endNode = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];		// Set the end node to the node that has been double right-clicked

					AStarSolver();														// Recalculate the path by calling the solver
				}
				else
				{
					std::cout << "ERROR: Cannot make start node the end!\n";
				}// end else
			}// end if
	}// end if
	else if (m_bDRClickEvent && bFollowPath)
	{
		std::cout << "ERROR: Cannot place end node while object is moving.\n";
	}// end else

	if (bFollowPath)																	// If an object is following the path
		followPath();																	// Begin/Continue to follow the path

}// end update


//----------------------------------------------RENDERING-----------------------------------------------


void Game::render()
{
	SDL_RenderClear(m_pRenderer);														// Clear the renderer to the draw colour

	/* ____ NODE, CONNECTION, & PATH RENDERING ____ */

	// * CONNECTIONS *
	SDL_SetRenderDrawColor(getRenderer(), 255, 100, 100, 255);							// Set draw color to green for drawing the nodes

	if (bRenderConnections) {															// If renderConnections is enabled perform the following
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
	SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);							// Set draw color to muted purple for drawing the nodes

	for (int x = 0; x < nMapWidth; x++) {
		for (int y = 0; y < nMapHeight; y++)
		{
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			if (nodes[y * nMapWidth + x].bObstacle == true) {							// If the current node is flagged as an obstacle perform the following
				SDL_SetRenderDrawColor(getRenderer(), 155, 155, 155, 255);				// Set draw color to grey to make the node stand out from the green
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with grey color so it is visably an obstacle
				SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);				// Set draw color back to muted purple for drawing outlne of node
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Render the nodes outline
			}// end if
			else if (&nodes[y * nMapWidth + x] == startNode) {							// Else if the node has been flagged as the start node perform the following
				SDL_SetRenderDrawColor(getRenderer(), 0, 255, 0, 255);					// Set draw colour to yellow
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with yellow color to signify the strat node
				SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);				// Set draw color back to muted purple for drawing outline of node
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Render the nodes outline
			}// end else if
			else if (&nodes[y * nMapWidth + x] == endNode) {							// Else if the node has been flagged as the end node perform the following
				SDL_SetRenderDrawColor(getRenderer(), 255, 0, 0, 255);					// Set draw colour to red
				SDL_RenderFillRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Fill the node with red color to signify the end node
				SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);				// Set draw color back to muted purple for drawing outline of node
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Render the nodes outline
			}// end else if
			else if (nodes[y * nMapWidth + x].bVisited) {								// Else if the node has been flagged as visited perform the following
				SDL_SetRenderDrawColor(getRenderer(), 255, 155, 0, 255);				// Set draw colour to orange
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Draw the node (outline)
			}// end else if
			else {																		// Else it is a traversable node and not an obstacle, perform the following
				SDL_SetRenderDrawColor(getRenderer(), 116, 86, 107, 255);				// Set draw color back to muted purple for drawing traversable nodes
				SDL_RenderDrawRect(getRenderer(), &nodes[y * nMapWidth + x].box);		// Render the node as a regular purple node (traversable)
			}// end else

			SDL_SetRenderDrawColor(getRenderer(), 0, 255, 0, 255);						// Set draw colour to green
			SDL_RenderDrawRect(getRenderer(), &nodes[mouseY / nodeSize *
				nMapWidth + mouseX / nodeSize].box);									// Draw the outline of node to green to indicate cursor is over it

			
		}// end for
	}// end for

	// * PATH *	
	if (endNode != nullptr)																// If the end node is not a null pointer perform the following
	{
		Node *p = endNode;																// Create a new temp node to store current, from which the parent can be referenced
		while (p->parent != nullptr)													// Loop until we reach a node with no parent (Start Node)
		{
			SDL_SetRenderDrawColor(getRenderer(), 0, 255, 255, 255);					// Set draw colour to Cyan for path line
			SDL_RenderDrawLine(getRenderer(), p->x*nodeSize + nodeSize / 2,
				p->y*nodeSize + nodeSize / 2,
				p->parent->x*nodeSize + nodeSize / 2,
				p->parent->y*nodeSize + nodeSize / 2);									// Draw a line from current node p (x1, y1) to p's parent (x2, y2)

			p = p->parent;																// Set next node to this node's parent
		}// end while
	}// end if

	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);								// Set draw colour to black for background

	player1->draw();

	SDL_RenderPresent(m_pRenderer);														// Draw to the screen
}// end render


//----------------------------------------------EVENT HANDLING-----------------------------------------------


void Game::handleEvents()
{
	//-------------------- HANDLE EVENTS FROM KEYS AND MOUSE BUTTONS --------------------
	while (SDL_PollEvent(&the_event))												// Poll for specific events
	{
		//if (the_event.key.keysym.sym == SDLK_ESCAPE) quit = true;					// CAUSE OF WEIRD CRASH WHEN CURSOR ON LEFT SIDE OF WINDOW
		if (the_event.type == SDL_KEYDOWN &&
			the_event.key.keysym.sym == SDLK_ESCAPE) m_bQuit = true;				// If escape key is hit, shut down the program
		else if (the_event.type == SDL_QUIT) m_bQuit = true;						// If user press the close button shut down the program

		if (the_event.type == SDL_MOUSEBUTTONDOWN &&
			the_event.button.button == SDL_BUTTON_LEFT) {							// If mouse button is pressed perform the following
			int clickX, clickY;														// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&clickX, &clickY);									// Get the location of the mouse click, store in temp variables
			setMouseClickX(clickX);													// Store X-position in variables used by nodes
			setMouseClickY(clickY);													// Store Y-position in variables used by nodes
			setClickEvent(true);													// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN &&
			the_event.button.button == SDL_BUTTON_RIGHT) {							// If mouse button is pressed perform the following
			if (!bFollowPath)														// If an object is not following a path
				AStarSolver();														// Run A* solver
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN &&
			the_event.button.button == SDL_BUTTON_LEFT &&
			the_event.button.clicks == 2) {											// If mouse button is pressed perform the following
			int DLClickX, DLClickY;													// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&DLClickX, &DLClickY);								// Get the location of the mouse click, store in temp variables
			setMouseClickX(DLClickX);												// Store X-position in variables used by nodes
			setMouseClickY(DLClickY);												// Store Y-position in variables used by nodes
			setDLClickEvent(true);													// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_MOUSEBUTTONDOWN &&
			the_event.button.button == SDL_BUTTON_RIGHT &&
			the_event.button.clicks == 2) {											// If mouse button is pressed perform the following
			int DRClickX, DRClickY;													// Set up temp variables to hold positoin of click
			SDL_GetMouseState(&DRClickX, &DRClickY);								// Get the location of the mouse click, store in temp variables
			setMouseClickX(DRClickX);												// Store X-position in variables used by nodes
			setMouseClickY(DRClickY);												// Store Y-position in variables used by nodes
			setDRClickEvent(true);													// Set m_bClickEvent to true so the click can be handled in update()
		}// end if

		if (the_event.type == SDL_KEYDOWN) {										// If a keyboard button is pressed performt he following
			switch (the_event.key.keysym.sym) {										// Switch for keyboard event to check key
			case SDLK_r:															// 'R' key is pressed, perform the following
				bRenderConnections = !bRenderConnections;							// Set bRenderConnections to its inverse (flag for rendering connections between nodes)
				break;
			case SDLK_SPACE:														// If space bar is pressed, perform the following
				Node * p = endNode;												// Create a new temp node to store endNode, from which the parent can be referenced
				if (p->parent != nullptr)											// If the end nodes parent is not null, a path exists
				{
					if (player1->getX() == endNode->x*nodeSize &&
						player1->getY() == endNode->y*nodeSize)						// If the player has reached the end node, they cannot initiate path following
					{
						std::cout << "ERROR: Cannot being path following if player is not at beginning!\n";
					}
					else
					{
						bFollowPath = !bFollowPath;									// Set bFollowPath to its inverse (Start following a path or pause it)
						break;
					}
				}
				else
				{
					std::cout << "ERROR: No path exists!\n";						// Path does not exist, object cannot follow the path
				}
			}// end swtitch
		}// end if
	}// end while
}// end handleEvents


//----------------------------------------------EXIT CLEANUP-----------------------------------------------


void Game::clean() {
	std::cout << "cleaning game\n";
	SDL_DestroyWindow(m_pWindow);													// Destroy the window
	SDL_DestroyRenderer(m_pRenderer);												// Destroy the renderer
	SDL_Quit();																		// Shut down SDL
}// end clean


//----------------------------------------------PATH FINDING-----------------------------------------------


bool Game::AStarSolver()
{
	/* ____ PLAYER PREPERATION ____ */

	int x = startNode->x;															// Get current nodes X position
	int y = startNode->y;															// Get current nodes Y position
	x = x * nodeSize;																// Upscale by factor node size for visualization 
	y = y * nodeSize;																// Upscale by factor node size for visualization

	player1->setPosition(x, y);														// Set players position to the new start node

	if (!pathToFollow.empty()) {													// If a path exisits...
		while (!pathToFollow.empty())
		{
			pathToFollow.pop();														// Erase the current path for the new one
		}// end while
	}// end if
	
	/* ____ MAP PREPERATION FOR PATH FINDING ____ */

	for (int x = 0; x < nMapWidth; x++)												// Resets each node in the map to default states
		for (int y = 0; y < nMapHeight; y++)
		{
			nodes[y*nMapWidth + x].bVisited = false;								// We haven't visited any nodes yet
			nodes[y*nMapWidth + x].fEstimatedTotalCost = INFINITY;					// Default is Infinity so we can consider it when searching
			nodes[y*nMapWidth + x].fCostSoFar = INFINITY;							// Default is also Infinity so we can consider it when searching
			nodes[y*nMapWidth + x].parent = nullptr;								// All nodes begin with no parent
		}// end for

	/* ____ EXECUTION ____ */

	PathAlgorithm::Functions::AStarSolver(startNode, endNode);						// Call the A-Star solver to generate a path

	/* ____ PATH PREPARATION FOR FOLLOWING ____ */

	Node *p = endNode;																// Create a temp node to store current node
	while (p != NULL) {																// Loop until we reach a node with null parent (Start node)
		pathToFollow.push(p);														// Push the node onto the stack
		p = p->parent;																// Set current node to current nodes parent
	}// end while

	return true;
}// end AStarSolver


//----------------------------------------------PATH FOLLOWING-----------------------------------------------


void Game::followPath() {
	//-------------------- MAKE OBJECT FOLLOW THE A* PATH --------------------
	if (endNode != nullptr && !pathToFollow.empty())											// If the end node is not a null pointer perform the following
	{
		Node *currentPoint = pathToFollow.top();												// Current point to move towards is set to the top node on the stack (closest node)	

		player1->moveTowardPoint(currentPoint->x*nodeSize, currentPoint->y*nodeSize);			// Move player towards top node in stack

		if (player1->getX() == currentPoint->x*nodeSize)										// If the players X-position matches the X-position of the node...
		{
			if (player1->getY() == currentPoint->y*nodeSize)									// If the players Y-position matches the Y-position of the node...
			{
				pathToFollow.pop();																// Pop the node from the stack because Player has reached its position
			}// end if
		}// end if

		if (player1->getX() == endNode->x*nodeSize && player1->getY() == endNode->y*nodeSize)	// If player has reached end node
		{
			std::cout << "PLAYER STATUS: Player has reached end of path.\n";
			bFollowPath = false;																// Set path following status back to false
		}
	}// end if
}// end followPath


//----------------------------------------------OTHER-----------------------------------------------


int Game::getWinSizeW()
{
	return dynamicWindowSize_W;																	// Return the dynamic window width for main init
}

int Game::getWinSizeH()
{
	return dynamicWindowSize_H;																	// Return the dynamic window height fot main init
}