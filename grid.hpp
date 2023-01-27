#pragma once

#include <Eigen/Core>
#include "cube.hpp"

class Chunk {
	constexpr static int rows = 128;
	constexpr static int columns = 128;

	Eigen::Array < bool, rows, columns > cells;
	Eigen::Array < unsigned int, rows, columns > neighbour_count;
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
	Grid(int r, int c, int origin_r, int origin_c);

	~Grid();

	void create_cubes_for_alive_grid_cells();

	void update_neighbour_count();

	void update();

	void add_cube(int r, int c);

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

	int rows;
	int columns;

	int origin_row; 
	int origin_column;
	
	Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> cells;
	Eigen::Array<unsigned int, Eigen::Dynamic, Eigen::Dynamic> neighbour_count;

	int number_of_alive_cells;
	int iteration;

	std::vector<Chunk> chunks;
	std::vector<Cube> cubes;
	std::vector<std::pair<int, int>> coordinates;
};