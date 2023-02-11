#pragma once

#include "Tracy.hpp"

#include <Eigen/Core>
#include <iostream>
#include <vector>
#include <unordered_set>

constexpr static int CHUNK_ROWS = 32;
constexpr static int CHUNK_COLUMNS = 32;


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

	void add_coordinate(char value);

	ChunkUpdateInfoDirection direction;
	Coordinate chunk_offset_coordinate;
	Coordinate neighbour_grid_coordinate;
	int data_max_value;
	// assume that CHUNK_ROWS == CHUNK_COLUMNS!
	constexpr static int MAX_NUMBER_OF_VALUES = CHUNK_ROWS;
	int current_number_of_values;
	std::array<std::pair<char, char>, MAX_NUMBER_OF_VALUES> data;
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

	bool has_to_update_in_direction(ChunkUpdateInfoDirection direction);

	Coordinate transform_to_world_coordinate(Coordinate chunk_coord);

	constexpr static int rows = CHUNK_ROWS;
	constexpr static int columns = CHUNK_COLUMNS;

	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;

	std::array<unsigned char, rows*columns> cells_data;
	std::array<unsigned char, rows*columns> neighbour_count_data;

	std::array<ChunkUpdateInfo, ChunkUpdateInfoDirection::DIRECTION_COUNT> update_info;
};