#include "Component.h"

class CCollision : public Component
{
public:
	float radius = 0;

	CCollision() = default;
	CCollision(float r) : radius(r) {}
};