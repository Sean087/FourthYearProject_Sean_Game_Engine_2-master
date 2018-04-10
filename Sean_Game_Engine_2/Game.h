//
//	Game.h
//
//

#ifndef __Game__
#define __Game__

#include <vector>
#include <iostream>
#include <list>
#include <stack>
#include "GameObject.h"

class Game
{
public:

	Game() {}
	~Game() {}

	// Make Game a singleton
	// Create the public instance function
	static Game* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new Game();
			return s_pInstance;
		}

		return s_pInstance;
	}

	// Set running variable to true
	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);	// initialise the game
	void render();													// rendering function
	void update();													// handles all game updates
	void handleEvents();											// handles input events
	void clean();													// clears stuff from memory
	bool AStarSolver();												// Sets up the grid for A* library and calls the library's A* solver
	void followPath();												// Controls how an object follows the A*-generated path

	bool running() { return m_bRunning; }							// returns the running status of the engine

	SDL_Renderer* getRenderer() const { return m_pRenderer; }		// returns a pointer to the renderer
	
	void setMouseClickX(int clickPosX) { m_mousePosX = clickPosX; }	// set mouse click x value after click happens
	void setMouseClickY(int clickPosY) { m_mousePosY = clickPosY; }	// set mouse click y value after click happens

	int getMouseClickPosX() { return m_mousePosX; }					// get mouse click x
	int getMouseClickPosY() { return m_mousePosY; }					// get mouse click y

	void setClickEvent(bool click) { m_bClickEvent = click; }		// sets the click event boolean
	bool getClickEvent() { return m_bClickEvent; }					// get the state of the click event boolean

	void setDLClickEvent(bool click) { m_bDLClickEvent = click; }	// sets the double-left click boolean
	bool getDLClickEvent() { return m_bDLClickEvent; }				// returns the double-left click boolean

	void setDRClickEvent(bool click) { m_bDRClickEvent = click; }	// sets the double-right click boolean
	bool getDRClickEvent() { return m_bDRClickEvent; }				// returns the double-right click boolean

	bool getExitStatus() { return m_bQuit; }// returns the exit status

	int getWinSizeW();						// return window width
	int getWinSizeH();						// return window height

private:

	SDL_Window * m_pWindow;					// Window to display the game
	SDL_Renderer* m_pRenderer;				// Render target

	std::vector<GameObject*> m_gameObjects;	// Vector to store game object pointers

	int m_currentFrame;						// Used for animated textures

	bool m_bRunning;						// Is the game running or not

	static Game* s_pInstance;				// Game instance for singleton

	int m_mousePosX;						// Position of mouse X upon click event
	int m_mousePosY;						// Position of mouse Y upon click event

	bool m_bClickEvent = false;				// Has a mouse click happened?

	bool m_bDLClickEvent = false;			// Has Double left click has happened?

	bool m_bDRClickEvent = false;			// Has Double right click has happened?

	bool m_bQuit = false;					// Flag for quitting the application
};

typedef Game TheGame;

#endif // !__Game__
