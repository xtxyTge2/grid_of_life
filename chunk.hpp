#pragma once

#include "Tracy.hpp"
#include <immintrin.h>

#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>

constexpr static int CHUNK_ROWS = 32;
constexpr static int CHUNK_COLUMNS = 32;


__m256i _mm256_custom_shift_left_epi256(__m256i a, const int imm8);
__m256i _mm256_custom_shift_right_epi256(__m256i a, const int imm8);

class Coordinate {
public:
	Coordinate() : x(0), y(0) 
	{}

	Coordinate(int a_x, int a_y) : 
		x(a_x), 
		y(a_y) 
	{
		ZoneScoped;
	}

	~Coordinate() 
	{}

	bool operator == (const Coordinate& rhs) const{
		ZoneScoped;
		return x == rhs.x && y == rhs.y;
	}

	bool operator != (const Coordinate& rhs) const{
		ZoneScoped;
		return !(*this == rhs);
	}

	int x;
	int y;
};

// DONT CHANGE THE VALUES BELOW, THEY ACT AS ARRAY INDICES FOR THE UPDATE NEIGHBBOUR INFO ON CHUNKS! THERE CANT BE HOLES IN THIS ENUM! DIRECTION_COUNT IS ALWAYS LAST AND KEEPS TRACK OF THE SIZE OF THE ENUM. 
//SRY FOR CAPS...
enum ChunkUpdateInfoDirection {
	LEFT = 0,
	TOP,
	RIGHT,
	BOTTOM,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT,
	DIRECTION_COUNT
};


namespace std
{
	template<>
	struct hash<Coordinate> {
		std::size_t operator()(Coordinate const &coord) const noexcept {
			ZoneScoped;
			return (51 + std::hash<int>()(coord.x) + 51 * std::hash<int>()(coord.y));
		}
	};
}


class Chunk {
public:

	constexpr static int rows = CHUNK_ROWS;
	constexpr static int columns = CHUNK_COLUMNS;

	Chunk(const Coordinate& coord, Coordinate origin_coord, const std::vector<std::pair<int, int>>& alive_cells_coordinates);

	void update_cells();

	void update_neighbour_count_inside();

	void update_neighbour_count_left_side(const std::array<unsigned char, rows>& data);

	void update_neighbour_count_right_side(const std::array<unsigned char, rows>& data);

	void update_neighbour_count_top_side(const std::array<unsigned char, columns>& data);

	void update_neighbour_count_bottom_side(const std::array<unsigned char, columns>& data);

	void update_neighbour_count_top_left_corner();

	void update_neighbour_count_top_right_corner();

	void update_neighbour_count_bottom_left_corner();

	void update_neighbour_count_bottom_right_corner();

	Coordinate transform_to_world_coordinate(Coordinate chunk_coord);


	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	bool has_alive_cells;
	
	// the rows of the array are 32 * 8 = 256 bits big, ie each row fits into a AVX2 __mm256i simd register.
	// we operate on complete rows in our main computation using simd functions and the used intrinsics
	// assume that the data is aligned by 32!
	alignas(32) std::array<unsigned char, rows*columns> cells_data;
	alignas(32) std::array<unsigned char, rows*columns> neighbour_count_data;
};

// assume Chunk::rows == Chunk::columns!
struct ChunkSideUpdateInfo {
	std::array<unsigned char, Chunk::rows> data;
	Coordinate chunk_to_update_coordinate;
};