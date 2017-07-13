#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include "Engine.h"

using namespace std;
using namespace sf;

class Game
{
public:
	Game(void);
	~Game(void);
	int socket_desc;
	void runGame();

	struct NetClients net_clients[MAX_PLAYERS];
	void initializeSockets();
	void finalizeSockets();
	void initiateNetClient();
	void addNetClient(const unsigned char *ip_adress, int  player_id, int client_sock);
protected:
	enum GameState { MENU, GAME_SINGLE, GAME_MULTIPLAYER, MULTI_SERVER, MULTI_CLIENT, GAME_OVER, END };
	GameState state;

private:
	Font font;

	int my_player;
	unsigned long ip_adress;
	int port_number;

	void menu();
	void single();
	void multiPlayerGame();
	void multiServer();
	void multiClient();

	void connectClient(int client_number);
	int listenClient();
	void acceptClient(int client_number, int new_socket);

	void connectClient(int client_number, int port);
	void listenClient(int client_number, int port);

};