#pragma once

#include <tracy/Tracy.hpp>

#include "omp.h"
#include "ui_state.hpp"
#include "opencl_context.hpp"
#include "chunk.hpp"

#include <ppl.h>
#include <concurrent_vector.h>
#include <concurrent_unordered_set.h>
#include "coordinate.hpp"


//--------------------------------------------------------------------------------
class Grid {
public:
	Grid(std::shared_ptr<OpenCLContext> context);

	void create_new_chunk_and_set_alive_cells(const Coordinate& coord, const std::vector<std::pair<int, int>>& coordinates);

	void update_cells_of_all_chunks();

	void update_neighbour_count_inside_for_chunk_index_range(std::pair<std::size_t, std::size_t> start_end_index_pair);

	void update_coordinates_of_alive_cells_for_all_chunks();

	void remove_empty_chunks();

	void create_new_chunk(const Coordinate& coord);

	void update();

	void update_neighbour_count_and_set_info_of_all_chunks();

	void next_iteration();
	
	void update_neighbours_of_all_chunks();

	void create_needed_neighbours_of_all_chunks();

	void set_chunk_neighbour_info(std::size_t chunk_id);

	std::vector < std::pair<std::size_t, std::size_t> > get_partition_data_for_chunks(unsigned int number_of_workers, bool allow_small_task_sizes);
	//--------------------------------------------------------------------------------
	// data
	std::size_t iteration;

	std::size_t number_of_chunks;

	boost::unordered_flat_map<Coordinate, std::size_t> chunk_map;
	std::vector<Chunk> chunks;

	concurrency::concurrent_vector<ChunkSideUpdateInfo> chunks_left_side_update_infos;
	concurrency::concurrent_vector<ChunkSideUpdateInfo> chunks_right_side_update_infos;
	concurrency::concurrent_vector<ChunkSideUpdateInfo> chunks_top_side_update_infos;
	concurrency::concurrent_vector<ChunkSideUpdateInfo> chunks_bottom_side_update_infos;

	concurrency::concurrent_vector<Coordinate> top_left_corner_update_infos;
	concurrency::concurrent_vector<Coordinate> top_right_corner_update_infos;
	concurrency::concurrent_vector<Coordinate> bottom_left_corner_update_infos;
	concurrency::concurrent_vector<Coordinate> bottom_right_corner_update_infos;
	
	concurrency::concurrent_vector<Coordinate> coordinates_of_chunks_to_create;

	std::shared_ptr<OpenCLContext> opencl_context;
};


//--------------------------------------------------------------------------------
struct Grid_Execution_State {
	bool use_opencl_kernel = false;
	int number_of_iterations_per_single_frame = 1;
	bool updated_grid_coordinates = false;
	bool updated_border_coordinates = false;
	bool is_running = false;
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