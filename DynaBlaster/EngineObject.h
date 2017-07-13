#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
class Engine;

enum State {
	STATE_MOVE,
	STATE_STOP,
	STATE_HIDDEN,
	STATE_DESTROY
};


enum Direction {
	DIR_NONE,
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT
};

class EngineObject : public sf::Drawable,
	sf::Transformable
{
public:
	EngineObject(void);
	~EngineObject(void);

	const sf::Vector2f& getPosition() const;
	State getState();
	bool intersects(const EngineObject &eo) const;
	bool isToxic();

	virtual void update(unsigned long game_turn, Engine * e) = 0;
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
protected:
	bool toxic;
	sf::Sprite sprite;
	State state;
	Direction direction;
	unsigned long birth_turn;
};

