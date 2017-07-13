#include "Bomb.h"
#include "Engine.h"

#define BOMB_SPRITE_WIDTH 16
#define BOMB_SPRITE_HEIGHT 16

Bomb::Bomb(unsigned long game_turn, sf::Vector2f &pos, sf::Texture &texture)
{
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(470, 0, BOMB_SPRITE_WIDTH, BOMB_SPRITE_HEIGHT));
	sprite.setScale(2, 2);
	sprite.setPosition(pos);
	birth_turn = game_turn;
}

Bomb::~Bomb()
{
}

void Bomb::update(unsigned long game_turn, Engine * e)
{
	if (game_turn - birth_turn > 30)
	{
		e->createExplosion(getPosition(), DIR_NONE, 4);
		state = STATE_DESTROY;
	}
}