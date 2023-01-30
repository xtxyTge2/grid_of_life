#pragma once

#include <Eigen/Core>
#include "cube.hpp"
#include "ui_state.hpp"


class Chunk {
public:
	Chunk(int coordinate_row, int coordinate_column);

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

	void add_cube(std::pair<int, int> coord, bool is_border);

	void create_cubes_for_alive_grid_cells();

	bool has_to_update_right();

	bool has_to_update_left();

	bool has_to_update_top();

	bool has_to_update_bottom();

	bool has_to_update_corners();
	
	bool has_to_update_neighbours();

	std::pair<int, int> transform_to_world_coordinate(std::pair<int, int> chunk_coord);

	constexpr static int rows = 32;
	constexpr static int columns = 32;

	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;

	Eigen::Array < bool, rows, columns > cells;
	Eigen::Array < unsigned int, rows, columns > neighbour_count;

	bool has_to_still_update_neighbours;

	std::vector<std::pair<int, int>> chunk_coordinates;
	std::vector<Cube> cubes;

	std::vector<int> left_row_indices_to_update;
	std::vector<int> right_row_indices_to_update;
	std::vector<int> top_column_indices_to_update;
	std::vector<int> bottom_column_indices_to_update;

	bool has_to_update_top_left_corner;
	bool has_to_update_top_right_corner;
	bool has_to_update_bottom_right_corner;
	bool has_to_update_bottom_left_corner;
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

	Chunk* get_chunk_if_it_exists(int grid_row,
	                                       int grid_column);
	void create_new_chunk(int i, int j);

	void create_cubes_for_alive_grid_cells();

	void update();

	void update_neighbours_of_chunk(Chunk& chunk);

	Grid_Render_Data* create_render_data();

	void next_iteration();

	void resize_if_needed();
	//--------------------------------------------------------------------------------
	// data
	int number_of_alive_cells;
	int iteration;

	std::vector<Chunk> chunks;
};

//--------------------------------------------------------------------------------
struct Grid_Execution_State {
	bool is_running = false;
	bool run_manual_next_iteration = false;
	float time_since_last_iteration = 0.0f;
	float grid_speed = 1.0f;
};

//--------------------------------------------------------------------------------
class Grid_Manager {
public:
	Grid_Manager();

	void initialise();

	void update(double dt, Grid_UI_Controls_Info grid_ui_controls_info);

	void create_new_grid();

	Grid* grid;
	Grid_Execution_State grid_execution_state;
};