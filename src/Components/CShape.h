#pragma once

#include "Component.h"

class CShape : public Component
{
public:
	sf::CircleShape circle;
	
	CShape() = default;
	CShape(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness) : circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin({ radius, radius });
	}
};