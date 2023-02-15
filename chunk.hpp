#pragma once

#include "Tracy.hpp"
#include <immintrin.h>

#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/container_hash/hash.hpp>

constexpr static int CHUNK_ROWS = 32;
constexpr static int CHUNK_COLUMNS = 32;

__m256i _mm256_custom_shift_left_epi256(__m256i a, const int imm8);
__m256i _mm256_custom_shift_right_epi256(__m256i a, const int imm8);
bool _mm256_is_zero(__m256i a);


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

std::size_t hash_value(Coordinate const& c);


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

	Chunk();

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
	
	ChunkSideUpdateInfo(const Coordinate& c) :
		data({}),
	chunk_to_update_coordinate(c) 
	{

	};
	
	ChunkSideUpdateInfo(const std::array<unsigned char, Chunk::rows> & a, const Coordinate& c) :
		data(a),
	chunk_to_update_coordinate(c) 
	{

	};
	
	std::array<unsigned char, Chunk::rows> data;
	Coordinate chunk_to_update_coordinate;
};

/*

	Coordinate top_coord = Coordinate(chunk.grid_coordinate_row - 1, chunk.grid_coordinate_column);
	ChunkSideUpdateInfo& top_info = chunks_bottom_side_update_infos.emplace_back( top_coord);
	std::copy_n(std::begin(cells_data), Chunk::columns, std::begin(top_info.data));

	if (!chunk_map.contains(top_coord)) {
	coordinates_of_chunks_to_create.insert(top_coord);
	}
*/