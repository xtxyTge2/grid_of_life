#pragma once

#include "Tracy.hpp"

#include <omp.h>
#include "cube.hpp"
#include "ui_state.hpp"
#include "opencl_context.hpp"
#include "chunk.hpp"

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>







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

	void update_neighbour_count_and_set_info_of_all_chunks();

	void next_iteration();
	
	void update_neighbours_of_all_chunks();

	void create_needed_neighbours_of_all_chunks();

	void set_chunk_neighbour_info(std::shared_ptr<Chunk> chunk);
	//--------------------------------------------------------------------------------
	// data
	int iteration;

	boost::unordered_flat_map<Coordinate, std::shared_ptr<Chunk>> chunk_map;

	std::vector<ChunkSideUpdateInfo> chunks_left_side_update_infos;
	std::vector<ChunkSideUpdateInfo> chunks_right_side_update_infos;
	std::vector<ChunkSideUpdateInfo> chunks_top_side_update_infos;
	std::vector<ChunkSideUpdateInfo> chunks_bottom_side_update_infos;

	std::vector<Coordinate> top_left_corner_update_infos;
	std::vector<Coordinate> top_right_corner_update_infos;
	std::vector<Coordinate> bottom_left_corner_update_infos;
	std::vector<Coordinate> bottom_right_corner_update_infos;
	
	boost::unordered_flat_set<Coordinate> coordinates_of_chunks_to_create;

	std::vector<std::pair<int, int>> grid_coordinates;
	std::vector<std::pair<int, int>> border_coordinates;
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