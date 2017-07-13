#include "Game.h"
#include <string.h>
#include <fcntl.h>
#include<cassert>
#include <winsock.h> 

#pragma comment(lib, "ws2_32.lib")

RenderWindow window(VideoMode(WINDOW_HEIGHT, WINDOW_WIDTH), "Dyna Blaster", Style::Default);

struct StartPacket
{
	int my_player_id;
	NetClients client_list[MAX_PLAYERS - 2];
};

Game::Game(void)
{
	state = END;

	if (!font.loadFromFile("data/Mecha.ttf"))
	{
		printf("Font not found.\n");
		return;
	}
	socket_desc = -1;

	port_number = 10080;
	ip_adress = inet_addr("127.0.0.1"); // server ip address

	state = MENU;
}


Game::~Game(void)
{
}

void Game::initializeSockets()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		fprintf(stderr, "WSAStartup failed.\n");
	}

	socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_desc == -1)
	{
		fprintf(stderr, "Could not create socket.\n");
	}
}

void Game::finalizeSockets()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		closesocket(net_clients[i].socket_fd);
	}

	WSACleanup();
}

void Game::runGame()
{
	while (state != END)
	{
		switch (state)
		{
		case MENU:
			menu();
			break;
		case GAME_SINGLE:
			single();
			break;
		case GAME_MULTIPLAYER:
			multiPlayerGame();
			break;
		case MULTI_SERVER:
			multiServer();
			break;
		case MULTI_CLIENT:
			multiClient();
			break;
		case GAME_OVER:
			break;
		}
	}
}

void Game::menu()
{
	Text title("Dyna Blaster", font, 80);
	title.setStyle(Text::Bold);

	title.setPosition(1024 / 2 - title.getGlobalBounds().width / 2, 20);

	const int buttons_count = 4;
	Text tekst[buttons_count];

	string str[] = { "Single player", "Multiplayer server", "Multiplayer client", "Exit" };
	for (int i = 0; i < buttons_count; i++)
	{
		tekst[i].setFont(font);
		tekst[i].setCharacterSize(65);
		tekst[i].setString(str[i]);
		tekst[i].setPosition(1024 / 2 - tekst[i].getGlobalBounds().width / 2, 250 + i * 120);
	}

	while (state == MENU)
	{
		Vector2f mouse(Mouse::getPosition(window));
		Event event;

		while (window.pollEvent(event))
		{
			//Single player
			if (tekst[0].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left || event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Num1)
			{
				state = GAME_SINGLE;
			}

			//Multiplayer server
			else if (tekst[1].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left || event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Num2)
			{
				state = MULTI_SERVER;
			}

			//Multiplayer client
			else if (tekst[2].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left || event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Num3)
			{
				state = MULTI_CLIENT;
			}

			//Exit
			else if (tekst[3].getGlobalBounds().contains(mouse) && event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left ||
				event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
			{
				state = END;
			}
		}
		for (int i = 0; i < buttons_count; i++)
			if (tekst[i].getGlobalBounds().contains(mouse))
				tekst[i].setColor(Color::Cyan);
			else tekst[i].setColor(Color::White);

			window.clear();

			window.draw(title);
			for (int i = 0; i < buttons_count; i++)
				window.draw(tekst[i]);

			window.display();
	}
}

void Game::initiateNetClient()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		memset(&net_clients[i], 0, sizeof(net_clients[i]));
		net_clients[i].player_id = -1;
	}
}

void Game::addNetClient(const unsigned char *ip_adress, int  player_id, int client_sock)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (net_clients[i].player_id == -1)
		{
			memcpy(net_clients[i].ip_adress, ip_adress, sizeof(net_clients[i].ip_adress));
			net_clients[i].player_id = player_id;
			net_clients[i].socket_fd = client_sock;
			return;
		}
	}
	assert(!"no slots left");
}

void Game::single()
{
	initiateNetClient();

	Engine *engine = new Engine(window, 0, net_clients);
	engine->runEngine();
	delete engine;
	state = MENU;
}

void Game::multiPlayerGame()
{
	Engine *engine = new Engine(window, my_player, net_clients);
	engine->runEngine();
	delete engine;
	state = MENU;
}

void Game::multiClient()
{
	initiateNetClient();

	Text title("Dyna Blaster", font, 80);
	title.setStyle(Text::Bold);
	title.setPosition(1024 / 2 - title.getGlobalBounds().width / 2, 20);

	int net_status = 0;
	Text tekst;
	string str[] = { "Connecting to server", "Failed to connect", "Waiting for game start", "Starting game", "Failed to start" };
	tekst.setFont(font);
	tekst.setCharacterSize(65);
	tekst.setString(str[net_status]);
	tekst.setPosition(1024 / 2 - tekst.getGlobalBounds().width / 2, 250);

	struct sockaddr_in server;
	server.sin_addr.s_addr = ip_adress;
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number);

	addNetClient((unsigned char*)&(server.sin_addr.s_addr), 0, socket_desc);


	int ReceiveTimeout = 2000;
	int e = setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char*)&ReceiveTimeout, sizeof(int));//2 second timeout for connect

	sf::Clock turn_clock;
	int counter = 0;
	while (state == MULTI_CLIENT)
	{
		turn_clock.restart();

		window.clear();
		window.draw(title);
		window.draw(tekst);
		window.display();

		switch (net_status)
		{
		case 0: //connecting
			if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) >= 0)
			{
				net_status = 2;
			}
			else
			{
				net_status = 1;
			}
			break;
		case 1: //failed to connect
		case 4: //failed to start
			counter++;
			if (counter > 60)
				state = MENU;
			break;
		case 2: //connected waiting for server
		{
			unsigned long iMode = 1;
			ioctlsocket(socket_desc, FIONREAD, &iMode);

			if (iMode >= sizeof(StartPacket))
			{
				net_status = 3;
			}
		}
			break;
		case 3: //starting game
			StartPacket start_packet;
			counter = recv(socket_desc, (char *)&start_packet, sizeof(start_packet), 0);
			if (counter != sizeof(start_packet))
			{
				fprintf(stderr, "Receive failed.\n");
				net_status = 4;
				counter = 0;
				break;
			}
			int numer_of_players = 2;
			my_player = start_packet.my_player_id;
			for (int i = 0; i < MAX_PLAYERS - 2; i++)
			{
				addNetClient((unsigned char*)&(start_packet.client_list[i].ip_adress), start_packet.client_list[i].player_id, -1);
				if (start_packet.client_list[i].player_id > 0)
				{
					numer_of_players++;
				}
			}

			//connection with other clients

			int new_socket;
			if (my_player != 0 && my_player != numer_of_players - 1)
			{
				new_socket = listenClient();
			}

			Sleep(1000);

			for (int i = 1; i < my_player; i++)
			{
				connectClient(i);
			}

			for (int i = my_player+1; i < numer_of_players; i++)
			{
				acceptClient(i, new_socket);
			}


			state = GAME_MULTIPLAYER;
			break;
		}
		tekst.setString(str[net_status]);
		tekst.setPosition(1024 / 2 - tekst.getGlobalBounds().width / 2, 250);

		Vector2f mouse(Mouse::getPosition(window));
		Event event;
		while (window.pollEvent(event))
		{
			//ESC or X
			if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
			{
				state = END;
			}
		}

		Time time_elapsed = turn_clock.getElapsedTime();
		if (time_elapsed < sf::milliseconds(1000 / TURN_PER_SEC))
		{
			sleep(sf::milliseconds(1000 / TURN_PER_SEC) - time_elapsed);
		}
	}
}

void Game::multiServer()
{
	my_player = 0;
	initiateNetClient();

	Text title("Dyna Blaster", font, 80);
	title.setStyle(Text::Bold);
	title.setPosition(1024 / 2 - title.getGlobalBounds().width / 2, 20);

	Text tekst("Waiting for clients", font, 55);
	tekst.setPosition(1024 / 2 - tekst.getGlobalBounds().width / 2, 130);

	const int players_count = 3;
	Text players[players_count];
	for (int i = 0; i < players_count; i++)
	{
		players[i].setFont(font);
		players[i].setCharacterSize(45);
	}

	const int buttons_count = 2;
	Text buttons[buttons_count];
	string buttons_names[] = { "Start game", "Back to menu" };
	for (int i = 0; i < buttons_count; i++)
	{
		buttons[i].setFont(font);
		buttons[i].setCharacterSize(50);
		buttons[i].setString(buttons_names[i]);
		buttons[i].setPosition(1024 / 2 - buttons[i].getGlobalBounds().width / 2, 600 + i * 60);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port_number);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		fprintf(stderr, "Bind failed.\n");
	}

	int client_number = 1;
	char buffor[256];
	while (state == MULTI_SERVER)
	{
		struct sockaddr_in client;
		int c = sizeof(struct sockaddr_in);

		unsigned long iMode = 1;
		int ret = ioctlsocket(socket_desc, FIONBIO, &iMode);
		if (ret == SOCKET_ERROR)
		{
			fprintf(stderr, "Failed to set into non blocking mode.\n");
		}

		listen(socket_desc, 3);

		int client_sock = accept(socket_desc, (struct sockaddr*)&client, (int*)&c);
		if (client_sock >= 0)
		{
			unsigned long iMode = 0;
			int ret = ioctlsocket(client_sock, FIONBIO, &iMode);
			if (ret == SOCKET_ERROR)
			{
				fprintf(stderr, "Failed to set into blocking mode.\n");
			}
			addNetClient((unsigned char*)&(client.sin_addr.s_addr), client_number, client_sock);
			client_number++;
		}

		for (int i = 0; i < players_count; i++)
		{
			sprintf(buffor, "%d.%d.%d.%d", (int)net_clients[i].ip_adress[0], (int)net_clients[i].ip_adress[1], (int)net_clients[i].ip_adress[2], (int)net_clients[i].ip_adress[3]);
			players[i].setString(buffor);
			players[i].setPosition(1024 / 2 - players[i].getGlobalBounds().width / 2, 250 + i * 80);
		}

		Vector2f mouse(Mouse::getPosition(window));
		Event event;
		while (window.pollEvent(event))
		{
			//ESC or X
			if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
			{
				state = END;
			}

			//Start game
			else if (buttons[0].getGlobalBounds().contains(mouse) && event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				bool send_succeded = true;
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					StartPacket sp;
					memset(&sp, 0, sizeof(sp));
					for (int k = 0; k < MAX_PLAYERS - 2; k++)
					{
						sp.client_list[k].player_id = -1;
						sp.client_list[k].socket_fd = -1;
					}

					NetClients *send_client = &net_clients[i];
					if (send_client->player_id < 0)
						continue;
					sp.my_player_id = send_client->player_id;

					int m = 0;
					for (int k = 0; k < MAX_PLAYERS; k++)
					{
						NetClients *nc = &net_clients[k];
						if (nc->player_id == sp.my_player_id || nc->player_id < 0)
							continue;
						memcpy(sp.client_list[m].ip_adress, nc->ip_adress, sizeof(nc->ip_adress));
						sp.client_list[m].player_id = nc->player_id;
						m++;
					}
					if (send(send_client->socket_fd, (char *)&sp, sizeof(sp), 0) < 0)
					{
						fprintf(stderr, "Send failed.\n");
						send_succeded = false;
					}
				}
				if (send_succeded)
				{
					unsigned long iMode = 0;
					int ret = ioctlsocket(socket_desc, FIONBIO, &iMode);
					state = GAME_MULTIPLAYER;
				}
			}

			//Back to menu
			else if (buttons[1].getGlobalBounds().contains(mouse) && event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				state = MENU;
			}
		}

		for (int i = 0; i < buttons_count; i++)
		{
			if (buttons[i].getGlobalBounds().contains(mouse))
				buttons[i].setColor(Color::Cyan);
			else
				buttons[i].setColor(Color::White);
		}

		window.clear();
		window.draw(title);
		window.draw(tekst);
		for (int i = 0; i < players_count; i++)
			window.draw(players[i]);
		for (int i = 0; i < buttons_count; i++)
			window.draw(buttons[i]);
		window.display();
	}
}

void Game::connectClient(int client_number)
{
	int new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
	{
		//could not create socket
	}

	struct sockaddr_in server;
	for (int i = 0; i < MAX_PLAYERS - 1; i++)
	{
		if (net_clients[i].player_id == client_number)
		{
			memcpy(&server.sin_addr.s_addr, &net_clients[i].ip_adress, sizeof(net_clients[i].ip_adress));
			break;
		}
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number + 1);
	if (connect(new_socket, (struct sockaddr *)&server, sizeof(server)) >= 0)
	{
		//connected
		for (int i = 0; i < MAX_PLAYERS - 1; i++)
		{
			if (net_clients[i].player_id == client_number)
			{
				net_clients[i].socket_fd = new_socket;
			}
		}
	}
}

int Game::listenClient()
{
	int new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1)
	{
		//could not create socket
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port_number + 1);
	if (bind(new_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		//print the error message
	}

	listen(new_socket, 1);
	return new_socket;
}

void Game::acceptClient(int client_number, int new_socket)
{
	struct sockaddr_in client;
	int c = sizeof(struct sockaddr_in);

	int client_sock = accept(new_socket, (struct sockaddr *)&client, &c);
	if (client_sock >= 0)
	{
		//connected
		for (int i = 0; i < MAX_PLAYERS - 1; i++)
		{
			if (net_clients[i].player_id == client_number)
			{
				net_clients[i].socket_fd = client_sock;
			}
		}
	}
}