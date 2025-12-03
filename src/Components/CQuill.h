#pragma once

#include "Component.h"

class CQuill : public Component
{
public:
	sf::ConvexShape quill;

	CQuill() = default;
	CQuill(float length, float width, sf::Color color)
	{
		quill.setPointCount(4);

		quill.setPoint(0, { 0, -width / 2 });
		quill.setPoint(1, { length, 0 });
		quill.setPoint(2, { 0, width / 2 });
		quill.setPoint(3, { -length, 0 });

		quill.setFillColor(color);
	}
};