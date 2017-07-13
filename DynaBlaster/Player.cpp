#include "Player.h"
#include "Engine.h"

#include <math.h>
#include <iostream>


#define PLAYER_SPRITE_WIDTH 24
#define PLAYER_SPRITE_HEIGHT 24

using namespace sf;

Player::Player(void)
{
	active = false;

	frame_number = 0;

	frame = 0;
	speed = 3.2;
}

Player::~Player(void)
{
}

void Player::setInitialState(int player_number, sf::Texture &texture)
{
	sprite.setTexture(texture);
	sprite.setTextureRect(IntRect(0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));

	sprite.setScale(2, 2);

	switch (player_number)
	{
	case 0:
		sprite.setPosition(0, 0);
		//white
		top_coordinate = 196;
		left_coordinate = 0;
		break;
	case 1:
		sprite.setPosition(0, WINDOW_WIDTH - 2 * PLAYER_SPRITE_WIDTH);
		//green
		top_coordinate = 196 + PLAYER_SPRITE_HEIGHT;
		left_coordinate = 7 * PLAYER_SPRITE_WIDTH;
		break;
	case 2:
		sprite.setPosition(WINDOW_HEIGHT - 2 * PLAYER_SPRITE_HEIGHT, 0);
		//red
		top_coordinate = 196 + 3 * PLAYER_SPRITE_HEIGHT;
		left_coordinate = PLAYER_SPRITE_WIDTH;
		break;
	case 3:
		sprite.setPosition(WINDOW_HEIGHT - 2 * PLAYER_SPRITE_HEIGHT, WINDOW_WIDTH - 2 * PLAYER_SPRITE_WIDTH);
		//blue
		top_coordinate = 196 + 4 * PLAYER_SPRITE_HEIGHT;
		left_coordinate = 8 * PLAYER_SPRITE_WIDTH;
		break;
	}
}

void Player::updateTexture(unsigned long game_turn)
{
	if ((game_turn % 3) == 0)
	{
		if (state == STATE_MOVE)
		{
			switch (direction)
			{
			case DIR_RIGHT:
				frame = 4;
				break;
			case DIR_LEFT:
				frame = 7;
				break;
			case DIR_UP:
				frame = 10;
				break;
			case DIR_DOWN:
				frame = 1;
				break;
			}

			int frame_shift[] = { 0, -1, 1, -1 };
			frame += frame_shift[frame_number];

			if (frame_number < 3)
				frame_number++;
			else
				frame_number = 0;
		}

		if ((left_coordinate / PLAYER_SPRITE_WIDTH + frame) >= 13)
		{
			sprite.setTextureRect(IntRect(left_coordinate + (frame - 13)*PLAYER_SPRITE_WIDTH, top_coordinate + PLAYER_SPRITE_HEIGHT, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
		}
		else
		{
			sprite.setTextureRect(IntRect(left_coordinate + frame*PLAYER_SPRITE_WIDTH, top_coordinate, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
		}
	}
}

void Player::update(unsigned long game_turn, Engine * e)
{
	for (auto iter = e->objs.begin(); iter != e->objs.end(); iter++)
	{
		if ((*iter)->isToxic() && (*iter)->intersects(*this))
		{
			state = STATE_HIDDEN;
		}
	}
	updateTexture(game_turn);
	move();
}

void Player::stop()
{
	state = STATE_STOP;
	frame_number = 0;
}

void Player::stop_go_up()
{
	if (direction == DIR_UP)
	{
		frame = 9;
		stop();
	}
}

void Player::stop_go_down()
{
	if (direction == DIR_DOWN)
	{
		frame = 0;
		stop();
	}
}

void Player::stop_go_left()
{
	if (direction == DIR_LEFT)
	{
		frame = 6;
		stop();
	}
}

void Player::stop_go_right()
{
	if (direction == DIR_RIGHT)
	{
		frame = 3;
		stop();
	}
}


void Player::move()
{

	float vx = 0;
	float vy = 0;

	if (state == STATE_MOVE)
	{
		switch (direction)
		{
		case DIR_RIGHT:
			vx = 1;
			break;
		case DIR_LEFT:
			vx = -1;
			break;
		case DIR_UP:
			vy = -1;
			break;
		case DIR_DOWN:
			vy = 1;
			break;
		}
	}

	sprite.move(speed*vx, speed*vy);
}

void Player::go_right()
{
	direction = DIR_RIGHT;
	state = STATE_MOVE;
}
void Player::go_left()
{
	direction = DIR_LEFT;
	state = STATE_MOVE;
}
void Player::go_up()
{
	direction = DIR_UP;
	state = STATE_MOVE;
}
void Player::go_down()
{
	direction = DIR_DOWN;
	state = STATE_MOVE;
}