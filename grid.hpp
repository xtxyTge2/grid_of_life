#pragma once

#include <Eigen/Core>
#include "cube.hpp"
#include "ui_state.hpp"
#include <unordered_set>
#include "Tracy.hpp"
#include "opencl_context.hpp"
#include "chunk.hpp"

//--------------------------------------------------------------------------------
class Grid {
public:
	Grid(std::shared_ptr<OpenCLContext> context);

	void create_new_chunk_and_set_alive_cells(const Coordinate& coord, const std::vector<std::pair<int, int>>& coordinates);

	void update_coordinates_for_alive_grid_cells();

	void update_coordinates_for_chunk_borders();

	void update_cells_of_all_chunks();

	void remove_empty_chunks();

	void create_new_chunk(const Coordinate& coord);

	void update();

	void update_neighbours_of_chunk(std::shared_ptr<Chunk> chunk);

	void next_iteration();
	
	void update_all_neighbours_of_all_chunks();

	void create_all_needed_neighbour_chunks();
	//--------------------------------------------------------------------------------
	// data
	int number_of_alive_cells;
	int iteration;

	std::unordered_map<Coordinate, std::shared_ptr<Chunk>> chunk_map;

	std::unordered_set<Coordinate> grid_coordinates;
	std::unordered_set<Coordinate> border_coordinates;
	std::shared_ptr<OpenCLContext> opencl_context;
};

//--------------------------------------------------------------------------------
struct Grid_Execution_State {
	bool use_opencl_kernel = false;
	int number_of_iterations_per_single_frame = 1;
	bool updated_grid_coordinates = false;
	bool updated_border_coordinates = false;
	bool is_running = true;
	bool run_manual_next_iteration = false;
	float time_since_last_iteration = 0.0f;
	float grid_speed = 1.0f;
	bool show_chunk_borders = false;
	bool have_to_update_chunk_borders = false;
	bool should_run_at_max_possible_speed = true;
};

//--------------------------------------------------------------------------------
class Grid_Manager {
public:
	Grid_Manager();

	void update(double dt, const Grid_UI_Controls_Info& grid_ui_controls_info);

	void update_grid_execution_state(const Grid_UI_Controls_Info& ui_info);

	void create_new_grid();
	
	void update_grid_info();

	std::shared_ptr<Grid_Info> grid_info;
	std::unique_ptr<Grid> grid;
	Grid_Execution_State grid_execution_state;

	std::shared_ptr<OpenCLContext> opencl_context;
};