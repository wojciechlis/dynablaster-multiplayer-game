#include "Explosion.h"
#include "Engine.h"

#define EXPLOSION_SPRITE_WIDTH 16
#define EXPLOSION_SPRITE_HEIGHT 16

Explosion::Explosion(Direction dir, unsigned long game_turn, sf::Vector2f &pos, sf::Texture &texture, int sub_left)
{
	direction = dir;

	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(406, 32, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
	sprite.setScale(2, 2);
	sprite.setPosition(pos);
	
	birth_turn = game_turn;
	subexplosions_left = sub_left;
	toxic = true;
	updateTexture(game_turn);
}


Explosion::~Explosion()
{
}

void Explosion::update(unsigned long game_turn, Engine * e)
{
	updateTexture(game_turn);
	if (game_turn - birth_turn == 4)
	{
		createSubExplosion(game_turn, e);
	}

	if (game_turn - birth_turn > 30)
	{
		state = STATE_DESTROY;
	}
}

void Explosion::createSubExplosion(unsigned long game_turn, Engine * e)
{
	sf::Vector2f pos;
	if (subexplosions_left < 1)
	{
		return;
	}

	switch (direction)
	{
	case DIR_NONE:
		pos = getPosition();
		pos.y -= 2*EXPLOSION_SPRITE_HEIGHT;
		e->createExplosion(pos, DIR_UP, subexplosions_left-1);
		pos = getPosition();
		pos.x += 2*EXPLOSION_SPRITE_WIDTH;
		e->createExplosion(pos, DIR_RIGHT, subexplosions_left - 1);
		pos = getPosition();
		pos.y += 2 * EXPLOSION_SPRITE_HEIGHT;
		e->createExplosion(pos, DIR_DOWN, subexplosions_left - 1);
		pos = getPosition();
		pos.x -= 2 * EXPLOSION_SPRITE_WIDTH;
		e->createExplosion(pos, DIR_LEFT, subexplosions_left - 1);
		break;
	case DIR_UP:
		pos = getPosition();
		pos.y -= 2 * EXPLOSION_SPRITE_HEIGHT;
		e->createExplosion(pos, DIR_UP, subexplosions_left - 1);
		break;
	case DIR_RIGHT:
		pos = getPosition();
		pos.x += 2 * EXPLOSION_SPRITE_WIDTH;
		e->createExplosion(pos, DIR_RIGHT, subexplosions_left - 1);
		break;
	case DIR_DOWN:
		pos = getPosition();
		pos.y += 2 * EXPLOSION_SPRITE_HEIGHT;
		e->createExplosion(pos, DIR_DOWN, subexplosions_left - 1);
		break;
	case DIR_LEFT:
		pos = getPosition();
		pos.x -= 2 * EXPLOSION_SPRITE_WIDTH;
		e->createExplosion(pos, DIR_LEFT, subexplosions_left - 1);
		break;
	}
}
void Explosion::updateTexture(unsigned long game_turn)
{
	switch (direction)
	{
	case DIR_NONE:
		sprite.setTextureRect(sf::IntRect(406, 32, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
		break;
	case DIR_UP:
		sprite.setTextureRect(sf::IntRect(598, 16, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
		break;
	case DIR_RIGHT:
		sprite.setTextureRect(sf::IntRect(342, 32, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
		break;
	case DIR_DOWN:
		sprite.setTextureRect(sf::IntRect(598, 16, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
		break;
	case DIR_LEFT:
		sprite.setTextureRect(sf::IntRect(342, 32, EXPLOSION_SPRITE_WIDTH, EXPLOSION_SPRITE_HEIGHT));
		break;
	}
}
