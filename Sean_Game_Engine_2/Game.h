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
	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void render();
	void update();
	void handleEvents();
	void clean();
	bool AStarSolver();
	bool followPath();

	// Function to access the private running variable
	bool running() { return m_bRunning; }

	SDL_Renderer* getRenderer() const { return m_pRenderer; }

	void setMouseClickX(int clickPosX) { m_mousePosX = clickPosX; }
	void setMouseClickY(int clickPosY) { m_mousePosY = clickPosY; }

	int getMouseClickPosX() { return m_mousePosX; }
	int getMouseClickPosY() { return m_mousePosY; }

	void setClickEvent(bool click) { m_bClickEvent = click; }
	int getClickEvent() { return m_bClickEvent; }

	void setDLClickEvent(bool click) { m_bDLClickEvent = click; }
	int getDLClickEvent() { return m_bDLClickEvent; }

	void setDRClickEvent(bool click) { m_bDRClickEvent = click; }
	int getDRClickEvent() { return m_bDRClickEvent; }

	bool getExitStatus() { return m_bQuit; }

	int getWinSizeW();
	int getWinSizeH();

private:

	SDL_Window * m_pWindow;
	SDL_Renderer* m_pRenderer;

	std::vector<GameObject*> m_gameObjects;

	int m_currentFrame;

	bool m_bRunning;

	// Create the s_pInstance member variable
	static Game* s_pInstance;

	int m_mousePosX;
	int m_mousePosY;

	bool m_bClickEvent = false;

	bool m_bDLClickEvent = false;

	bool m_bDRClickEvent = false;

	bool m_bQuit = false;															// Flag for quitting the application
};

typedef Game TheGame;

#endif // !__Game__
