#pragma once

#include "Tracy.hpp"
#include <immintrin.h>

#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

#include "coordinate.hpp"


__m256i _mm256_custom_shift_left_epi256(__m256i a, const int imm8);
__m256i _mm256_custom_shift_right_epi256(__m256i a, const int imm8);
bool _mm256_is_zero(__m256i a);


class Chunk {
public:
	constexpr static int rows = 32;
	constexpr static int columns = 32;

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

	void update_coordinates_of_alive_cells();
	
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

	alignas(32) std::array<std::pair<int, int>, Chunk::rows*Chunk::columns> coordinates_of_alive_cells;
	unsigned int number_of_alive_cells;
};

// assume Chunk::rows == Chunk::columns!
struct ChunkSideUpdateInfo {
	std::array<unsigned char, Chunk::rows> data;
	Coordinate chunk_to_update_coordinate;
};