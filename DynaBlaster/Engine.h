#pragma once

#include "Player.h"
#include <vector>

#define MAX_PLAYERS 4
#define TURN_PER_SEC 20

enum PacketActions
{
	ACTION_NONE = 0,
	ACTION_EXIT_GAME,
	ACTION_MOVE,
	ACTION_PLACE_BOMB
};

enum ActionMoveParam
{
	MOVE_LEFT = 0,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_STOP_VERTICAL,
	MOVE_STOP_HORIZONTAL,
	MOVE_STOP
};

struct Packet
{
	int action;
	int param;
	unsigned long game_turn;
};

struct NetClients
{
	unsigned char ip_adress[4];
	int player_id;
	int socket_fd;
};

class Engine
{
public:

	Engine(sf::RenderWindow &win, int mp, NetClients *nc);
	~Engine(void);

	void runEngine();
	void getInput(Packet &packet);
	void updateWorld();
	void drawWorld();
	void updatePlayer(Packet &packet, Player &player);
	void addNetClient(const unsigned char *ip_adress, int  player_id);
	void exchangePackets();
	void loadGfxData(void);

	void createBomb(sf::Vector2f pos);
	void createExplosion(sf::Vector2f pos, Direction dir, int sub_left);

	std::vector<EngineObject*> objs;
private:
	std::vector<EngineObject*> objs_add;

	sf::Texture texture;
	bool menu;
	struct NetClients *net_clients;
	Packet packets[MAX_PLAYERS];
	int my_player;
	sf::Clock turn_clock;
	unsigned long game_turn;

	Player players[MAX_PLAYERS];
	sf::RenderWindow *window;
};