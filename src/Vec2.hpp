#include <SFML/Graphics.hpp>
#include <cmath>

template <typename T>
class Vec2
{
public:
	T x = 0;
	T y = 0;

	Vec2() = default;
	Vec2(T xin, T yin) : x(xin), y(yin) {}

	Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {}

	Vec2(const sf::Vector2i& vec) : x(T(vec.x)), y(T(vec.y)) {}

	// allows automatic conversion to sf::Vector2
	// this lets us pass Vec2 into sfml functions
	operator sf::Vector2<T>()
	{
		return sf::Vector2<T>(x, y);
	}

	Vec2 operator + (const Vec2& rhs) const
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2 operator - (const Vec2& rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2 operator * (const Vec2& rhs) const
	{
		return Vec2(x * rhs.x, y * rhs.y);
	}

	bool operator == (const Vec2& rhs) const
	{
		x == rhs.x && y == rhs.y ? return true : return false;
	}

	void operator += (const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	void operator -= (const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	void operator *= (const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
	}

	void operator /= (const Vec2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
	}

	float distance(const Vec2& rhs) const
	{
		return std::sqrt((rhs.x - x) + (rhs.y - y));
	}

	float length() const
	{
		return std::sqrt((x * x) + (y * y))
	}

	void normalize()
	{
		x = 1;
		y = 1;
	}
};