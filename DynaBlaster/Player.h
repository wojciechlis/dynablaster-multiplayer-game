#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "EngineObject.h"

#define WINDOW_WIDTH 768
#define WINDOW_HEIGHT 1024

class Player : public EngineObject
{
public:
	Player(void);
	~Player(void);

	bool active;

	void update(unsigned long game_turn, Engine * e);
	void setInitialState(int player_number, sf::Texture &texture);
	void updateTexture(unsigned long game_turn);

	void stop();
	void stop_go_up();
	void stop_go_down();
	void stop_go_right();
	void stop_go_left();
	void move();
	void go_right();
	void go_left();
	void go_up();
	void go_down();

private:
	int top_coordinate;
	int left_coordinate;
	
	size_t frame;
	int frame_number;
	float speed;
};