#include "EngineObject.h"
#include "Engine.h"

EngineObject::EngineObject(void)
{
	state = STATE_STOP;
	direction = DIR_NONE;
	birth_turn = 0;
	toxic = false;
}

EngineObject::~EngineObject(void)
{
}

void EngineObject::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(sprite);
}

const sf::Vector2f& EngineObject::getPosition()	const
{
	return sprite.getPosition();
}

State EngineObject::getState()
{
	return state;
}
bool EngineObject::intersects(const EngineObject &eo) const
{
	sf::FloatRect intersection;
	sf::FloatRect bounds = sprite.getGlobalBounds();
	if (bounds.intersects(eo.sprite.getGlobalBounds(), intersection))
	{
		if ((intersection.height + intersection.width) > ((bounds.height + bounds.height)/1.7))
			return true;
	}
	return false;
}

bool EngineObject::isToxic()
{
	return toxic;
}
