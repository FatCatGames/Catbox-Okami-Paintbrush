#include "Game.h"

void Game::ClearMessages()
{
	for (int i = 0; i < myMessages.size(); i++)
	{
		if (myMessages[i] && !myMessages[i]->GetGuaranteed())
		{
			delete myMessages[i];
		}
	}
	myMessages.clear();
}
