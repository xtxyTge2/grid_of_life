#pragma once

#include <Eigen/Core>
#include "cube.hpp"

struct Coordinate {
	int i;
	int j;
};

class Chunk {
public:
	Chunk();

	void update_neighbour_count_inside();

	void update_cells();

	void add_cube(std::pair<int, int> coord, bool is_border);

	void create_cubes_for_alive_grid_cells();

	std::pair<int, int> transform_to_world_coordinate(std::pair<int, int> chunk_coord);

	constexpr static int rows = 128;
	constexpr static int columns = 128;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;
	Eigen::Array < bool, rows, columns > cells;
	Eigen::Array < unsigned int, rows, columns > neighbour_count;
	std::vector<std::pair<int, int>> chunk_coordinates;
	std::vector<Cube> cubes;
};

//--------------------------------------------------------------------------------
struct Grid_Info {
	int rows;
	int columns;

	int origin_row; 
	int origin_column;
	int number_of_alive_cells;

	int iteration;
};

//--------------------------------------------------------------------------------
class Grid_Render_Data {
public:
	Grid_Render_Data();

	Grid_Info grid_info;
	std::vector<Cube_Render_Data> cubes_render_data;
};

//--------------------------------------------------------------------------------
class Grid {
public:
	Grid();

	~Grid();

	void create_new_chunk(int i, int j);

	void create_cubes_for_alive_grid_cells();

	void update_neighbour_count();

	void update();

	void add_cube(int x, int y, bool is_border);

	Grid_Render_Data* create_render_data();

	void next_iteration();

	void resize_if_needed();

	void update_neighbour_count_top();

	void update_neighbour_count_bottom();

	void update_neighbour_count_left();

	void update_neighbour_count_right();

	void update_neighbour_count_corners();
	//--------------------------------------------------------------------------------
	// data
	int number_of_alive_cells;
	int iteration;

	std::vector<Chunk> chunks;
};

