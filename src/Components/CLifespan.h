#pragma once

#include "Component.h"

class CLifespan : public Component
{
public:
	int lifespan = 0;
	int remaining = 0;

	CLifespan() = default;
	CLifespan(int totalLifespan) : lifespan(totalLifespan), remaining(totalLifespan) {}
};