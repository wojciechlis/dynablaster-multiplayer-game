#include "Engine.h"
#include "Bomb.h"
#include "Explosion.h"
#include <string.h>
#include <fcntl.h>
#include <winsock.h>
#include<cassert>

using namespace sf;

Engine::Engine(RenderWindow &win, int mp, NetClients *nc)
{
	window = &win;
	my_player = mp;
	net_clients = nc;
	game_turn = 0;
}

Engine::~Engine(void)
{
}

void Engine::loadGfxData(void)
{
	sf::Image img;
	img.loadFromFile("data/images/big_dyna.png");
	sf::Color color(128, 128, 255);
	img.createMaskFromColor(color);

	texture.loadFromImage(img);
}

void Engine::addNetClient(const unsigned char *ip_adress, int  player_id)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (net_clients[i].player_id == -1)
		{
			memcpy(net_clients[i].ip_adress, ip_adress, sizeof(net_clients[i].ip_adress));
			net_clients[i].player_id = player_id;
			return;
		}
	}
}

void Engine::getInput(Packet &packet)
{
	packet.action = ACTION_NONE;
	packet.param = 0;

	Event event;
	while (window->pollEvent(event) && packet.action == ACTION_NONE)
	{
		if (event.type == Event::Closed)
		{
			packet.action = ACTION_EXIT_GAME;
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::Escape)
			{
				packet.action = ACTION_EXIT_GAME;
			}

			if (event.key.code == Keyboard::W || event.key.code == Keyboard::S || event.key.code == Keyboard::Up || event.key.code == Keyboard::Down)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_STOP_VERTICAL;
			}
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D || event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_STOP_HORIZONTAL;
			}
		}
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code != Keyboard::Down && event.key.code != Keyboard::Up && event.key.code != Keyboard::Left && event.key.code != Keyboard::Right)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_STOP;
			}
			if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_UP;
			}
			if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_DOWN;
			}
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_LEFT;
			}
			if (event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
			{
				packet.action = ACTION_MOVE;
				packet.param = MOVE_RIGHT;
			}
			if (event.key.code == Keyboard::Space)
			{
				packet.action = ACTION_PLACE_BOMB;
			}
		}
	}
}

void Engine::updatePlayer(Packet &packet, Player &player)
{
	switch (packet.action)
	{
	case ACTION_NONE:
		break;
	case ACTION_EXIT_GAME:
		menu = true;
		break;
	case ACTION_MOVE:
		switch (packet.param)
		{
		case MOVE_LEFT:
			player.go_left();
			break;
		case MOVE_RIGHT:
			player.go_right();
			break;
		case MOVE_UP:
			player.go_up();
			break;
		case MOVE_DOWN:
			player.go_down();
			break;
		case MOVE_STOP_VERTICAL:
			player.stop_go_up();
			player.stop_go_down();
			break;
		case MOVE_STOP_HORIZONTAL:
			player.stop_go_left();
			player.stop_go_right();
			break;
		case MOVE_STOP:
			player.stop();
			break;
		}
		break;
	case ACTION_PLACE_BOMB:
		createBomb(player.getPosition());
		break;
	}
}

void Engine::updateWorld()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (players[i].active)
		{
			updatePlayer(packets[i], players[i]);
		}
	}

	for (auto iter = objs.begin(); iter != objs.end(); iter++)
	{
		(*iter)->update(game_turn, this);
	}
	if (!objs_add.empty())
	{
		objs.insert(objs.end(), objs_add.begin(), objs_add.end());
		objs_add.clear();
	}
	for (auto iter = objs.begin(); iter != objs.end(); )
	{
		if ((*iter)->getState() == STATE_DESTROY)
		{
			EngineObject *obj = *iter;
			iter = objs.erase(iter);
			delete obj;
		}
		else
			iter++;
	}
}

void Engine::drawWorld()
{
	window->clear(sf::Color(20, 100, 20));;
	for (auto iter = objs.rbegin(); iter != objs.rend(); iter++)
	{
		if ((*iter)->getState() != STATE_HIDDEN)
		{
			window->draw(**iter);
		}
	}
	window->display();
}

void Engine::exchangePackets()
{
	packets[my_player].game_turn = game_turn;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		NetClients *nc = &net_clients[i];
		if (nc->player_id == -1)
			continue;

		memset(&packets[nc->player_id], 0, sizeof(Packet));
		int nbytes = send(nc->socket_fd, (char *)&packets[my_player], sizeof(Packet), 0);
		if (nbytes != sizeof(Packet))
		{
			assert(!"Send failed.");
		}

		nbytes = recv(nc->socket_fd, (char *)&packets[nc->player_id], sizeof(Packet), 0);
		if (nbytes != sizeof(Packet))
		{
			printf("Server: recv() error %ld.\n", WSAGetLastError());
			assert(!"Receive failed.");
		}
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		NetClients *nc = &net_clients[i];
		if (nc->player_id == -1)
			continue;

		if (packets[nc->player_id].game_turn != game_turn)
		{
			assert(!"Packet order error.");
		}

	}
}

void Engine::runEngine()
{
	menu = false;
	loadGfxData();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		NetClients *nc = &net_clients[i];
		if (nc->player_id == -1)
			continue;
		players[nc->player_id].active = true;
		players[nc->player_id].setInitialState(nc->player_id, texture);

	}
	players[my_player].active = true;
	players[my_player].setInitialState(my_player, texture);

	for (int player_id = 0; player_id < MAX_PLAYERS; player_id++)
	{
		if (!players[player_id].active)
			continue;
		objs.push_back(&players[player_id]);
	}

	while (!menu)
	{
		turn_clock.restart();

		getInput(packets[my_player]);
		exchangePackets();
		updateWorld();
		drawWorld();

		Time time_elapsed = turn_clock.getElapsedTime();
		if (time_elapsed < milliseconds(1000 / TURN_PER_SEC))
		{
			sleep(milliseconds(1000 / TURN_PER_SEC) - time_elapsed);
		}
		game_turn++;
	}
}

void Engine::createBomb(sf::Vector2f pos)
{
	Bomb *b = new Bomb(game_turn, pos, texture);
	objs_add.push_back(b);

}

void Engine::createExplosion(sf::Vector2f pos, Direction dir, int sub_left)
{
	Explosion *e = new Explosion(dir, game_turn, pos, texture, sub_left);
	objs_add.push_back(e);
}