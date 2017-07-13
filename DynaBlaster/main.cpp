#include "Game.h"

int main()
{
	Game game;
	game.initializeSockets();
	game.runGame();
	game.finalizeSockets();
	return EXIT_SUCCESS;
}