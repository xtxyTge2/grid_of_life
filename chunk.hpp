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


class ChunkUpdateInfo {
public:
	ChunkUpdateInfo();

	void initialise(ChunkUpdateInfoDirection dir, Coordinate chunk_grid_coordinate);

	void add_coordinate(int value);

	ChunkUpdateInfoDirection direction;
	Coordinate chunk_offset_coordinate;
	Coordinate neighbour_grid_coordinate;
	int data_max_value;
	std::vector<Coordinate> data;
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
	Chunk(const Coordinate& coord, Coordinate origin_coord);

	void update_neighbour_count_in_direction(ChunkUpdateInfoDirection direction);

	void update_cells();

	void update_neighbour_count_inside();

	void clear_neighbour_update_info();

	void update_neighbour_count_and_set_info();

	void update_chunk_coordinates();

	bool has_to_update_in_direction(ChunkUpdateInfoDirection direction);

	Coordinate transform_to_world_coordinate(Coordinate chunk_coord);

	constexpr static int rows = 32;
	constexpr static int columns = 32;

	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;

	Eigen::Array < bool, rows, columns, Eigen::RowMajor > cells;
	Eigen::Array < unsigned int, rows, columns, Eigen::RowMajor > neighbour_count;

	std::array<ChunkUpdateInfo, ChunkUpdateInfoDirection::DIRECTION_COUNT> update_info;

	std::vector<std::pair<int, int>> chunk_coordinates;
	std::vector<std::pair<int, int>> border_coordinates;
};