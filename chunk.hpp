#pragma once

#include "Tracy.hpp"
#include <Eigen/Core>
#include <iostream>
#include <vector>
#include <unordered_set>

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
	Chunk(const Coordinate& coord);

	void update_cells_first_version();

	void update_cells_second_version();

	void update_cells_third_version();

	void print_chunk();

	void update_neighbour_count_inside();

	void update_neighbour_count_top();

	void update_neighbour_count_bottom();

	void update_neighbour_count_left();

	void update_neighbour_count_right();

	void update_neighbour_count_corners();

	void clear_neighbour_update_info();
	
	void update_cells();

	void update_neighbour_count_and_set_info();

	void update_chunk_coordinates();

	bool has_to_update_right();

	bool has_to_update_left();

	bool has_to_update_top();

	bool has_to_update_bottom();

	bool has_to_update_corners();
	
	bool has_to_update_neighbours();

	Coordinate transform_to_world_coordinate(Coordinate chunk_coord);

	constexpr static int rows = 16;
	constexpr static int columns = 16;

	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;

	Eigen::Array < bool, rows, columns, Eigen::RowMajor > cells;
	Eigen::Array < unsigned int, rows, columns, Eigen::RowMajor > neighbour_count;

	std::unordered_set<Coordinate> chunk_coordinates;
	std::vector<Coordinate> border_coordinates;

	std::vector<int> left_row_indices_to_update;
	std::vector<int> right_row_indices_to_update;
	std::vector<int> top_column_indices_to_update;
	std::vector<int> bottom_column_indices_to_update;

	bool has_to_update_top_left_corner;
	bool has_to_update_top_right_corner;
	bool has_to_update_bottom_right_corner;
	bool has_to_update_bottom_left_corner;
};