#pragma once
#include "EngineObject.h"

class Bomb : public EngineObject
{
public:
	Bomb(unsigned long game_turn, sf::Vector2f &pos, sf::Texture &texture);
	~Bomb();

	void update(unsigned long game_turn, Engine * e);
};

