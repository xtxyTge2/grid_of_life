#include "coordinate.hpp"

Coordinate::Coordinate() : 
	x(0),
	y(0)
{
	ZoneScoped;
}

Coordinate::Coordinate(int a_x, int a_y) :
	x(a_x),
	y(a_y)
{
	ZoneScoped;
}

bool Coordinate::operator == (const Coordinate& rhs) const {
	ZoneScoped;
	return x == rhs.x && y == rhs.y;
}

bool Coordinate::operator != (const Coordinate& rhs) const {
	ZoneScoped;
	return !(*this == rhs);
}

std::size_t hash_value(Coordinate const& c)
{
	ZoneScoped;
	return 51 + boost::hash < int >()(c.x) + 51 * boost::hash < int >()(c.y);
}