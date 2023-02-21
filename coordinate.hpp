#pragma once

#include "Tracy.hpp"
#include <boost/container_hash/hash.hpp>

class Coordinate {
public:
	Coordinate();

	Coordinate(int a_x, int a_y);

	bool operator == (const Coordinate& rhs) const;

	bool operator != (const Coordinate& rhs) const;

	int x;
	int y;
};

std::size_t hash_value(Coordinate const& c);


namespace std
{
	template<>
	struct hash<Coordinate> {
		std::size_t operator()(Coordinate const& coord) const noexcept {
			ZoneScoped;
			return (51 + std::hash<int>()(coord.x) + 51 * std::hash<int>()(coord.y));
		}
	};
}
