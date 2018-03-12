#include "Game.h"

int main(int argc, char* argv[])
{
	int winW = TheGame::Instance()->getWinSizeW();
	int winH = TheGame::Instance()->getWinSizeH();

	std::cout << "INIT STATUS: Game init attempt...\n";
	if (TheGame::Instance()->init("Seans Game Framework 2 - v0.3 (Path Following!)", 100, 100, winW, winH, false))
	{
		std::cout << "GAME STATUS: Starting main loop...\n";
		while (TheGame::Instance()->running() && TheGame::Instance()->getExitStatus() == false)	// If the game is running, peform the following
		{

			TheGame::Instance()->handleEvents();												// Poll to see if user closed the window
			TheGame::Instance()->update();														// Update Nodes and Game Objects
			TheGame::Instance()->render();														// Render Nodes and Game Objects to the screen

			TheGame::Instance()->setClickEvent(false);											// Set m_bClickEvent flag to false so a new click can be registered
			TheGame::Instance()->setDLClickEvent(false);										// Set m_bDLClickEvent flag to false so a new click can be registered
			TheGame::Instance()->setDRClickEvent(false);										// Set m_bDRClickEvent flag to false so a new click can be registered


			SDL_Delay(10);																		// add the delay

		}// end while
	}// end if
	else																						// Else the program did not initalize properly
	{
		std::cout << "INIT STATUS: Game init failure - " << SDL_GetError() << "\n";				// Print error
		return -1;																				// Return with exit status -1 (error encountered)
	}// end else

	std::cout << "TERMINATION STATUS: Game closing...\n";										// Loop has been broken, closing the game
	TheGame::Instance()->clean();																// Perform shut-down operation clean()

	//system("pause");																			// Keep console open after quitting for debugging purposes
	return 0;
}// end main