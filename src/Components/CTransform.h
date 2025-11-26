#pragma once

#include "Component.h"

class CTransform : public Component
{
public:
	Vec2<float> pos = { 0.0, 0.0 };
	Vec2<float> velocity = { 0.0, 0.0 };
	float angle = 0;

	CTransform() = default;
	CTransform(const Vec2<float>& p, const Vec2<float>& v, float a) : pos(p), velocity(v), angle(a) {}
};