#pragma once
#include "EngineObject.h"

class Explosion : public EngineObject
{
public:
	Explosion(Direction dir, unsigned long game_turn, sf::Vector2f &pos, sf::Texture &texture, int sub_left);
	~Explosion();

	void update(unsigned long game_turn, Engine * e);
	void updateTexture(unsigned long game_turn);
	void createSubExplosion(unsigned long game_turn, Engine * e);
protected:
	int subexplosions_left;
};

