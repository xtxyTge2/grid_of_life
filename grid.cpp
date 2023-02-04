#pragma once

#include "grid.hpp"
#include "Tracy.hpp"
#include <iostream>

using Eigen::Array;


Grid_Manager::Grid_Manager()
: grid_execution_state({})
{
	ZoneScoped;
	grid_info = std::make_shared < Grid_Info > ();
	opencl_context = std::make_shared < OpenCLContext > ();

	std::string open_cl_source_code_path = "opencl_grid.c";
	opencl_context->initialise(open_cl_source_code_path);

	create_new_grid();
};


void Grid_Manager::update_grid_info() {
	ZoneScoped;

	grid_info->iteration = grid->iteration;
	grid_info->rows = 0;
	grid_info->columns = 0;
	grid_info->origin_row = 0;
	grid_info->origin_column = 0;
	grid_info->number_of_alive_cells = grid->number_of_alive_cells;
}

//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;
	
	grid_execution_state = {};
	grid_execution_state.use_opencl_kernel = opencl_context->is_valid_context;
	
	grid = std::make_unique < Grid > (opencl_context);
	
}

void Grid_Manager::update_grid_execution_state(const Grid_UI_Controls_Info& ui_info) {
	ZoneScoped;

	switch (ui_info.button_type) {
		case GRID_NO_BUTTON_PRESSED:
			break;
		case GRID_RESET_BUTTON_PRESSED:
			grid.reset();
			create_new_grid();
			return;
			// if you remove the return somehow later, dont forget a break statement here :)
			// break;
		case GRID_NEXT_ITERATION_BUTTON_PRESSED:
			// only set run_manual_next_iteration if we are stopped
			if (!grid_execution_state.is_running) {
				grid_execution_state.run_manual_next_iteration = true;
			}
			break;
		case GRID_START_STOP_BUTTON_PRESSED:
			grid_execution_state.is_running = !grid_execution_state.is_running;
			grid_execution_state.time_since_last_iteration = 0.0f;
			break;
		default:
			break;
	}
	// check if the user changed to show/hide the grid borders.
	grid_execution_state.have_to_update_chunk_borders = grid_execution_state.show_chunk_borders != ui_info.show_chunk_borders;
	
	// set it to actual new value always.
	grid_execution_state.show_chunk_borders = ui_info.show_chunk_borders;
	grid_execution_state.grid_speed = ui_info.grid_speed_slider_value;
	grid_execution_state.should_run_at_max_possible_speed = ui_info.run_grid_at_max_possible_speed;
	grid_execution_state.number_of_iterations_per_single_frame = ui_info.number_of_grid_iterations_per_single_frame;
}

void Grid_Manager::update(double dt, const Grid_UI_Controls_Info& ui_info) {
	ZoneScoped;
	
	update_grid_execution_state(ui_info);
	
	grid_execution_state.updated_grid_coordinates = false;
	grid_execution_state.updated_border_coordinates = false;

	bool grid_changed = false;
	if (grid_execution_state.is_running) {
		if (grid_execution_state.should_run_at_max_possible_speed) {
			for (int i = 0; i < grid_execution_state.number_of_iterations_per_single_frame; i++) {
				grid->next_iteration();
			}
			grid_changed = true;
		} else {
			//assert(grid_execution_state.grid_speed > 0.0f);
			grid_execution_state.time_since_last_iteration += (float) dt;
			float threshold = 1.0f / grid_execution_state.grid_speed;
			if (grid_execution_state.time_since_last_iteration >= threshold) {
				grid->next_iteration();
				grid_changed = true;
				grid_execution_state.time_since_last_iteration = 0.0f;
			}
		}
	} else {
		if (grid_execution_state.run_manual_next_iteration) {
			grid_execution_state.run_manual_next_iteration = false;
			grid->next_iteration();
			grid_changed = true;
		}
	}

	// only update coordinates of alive grid cells if we are in the first iteration or if the grid changed.
	if (grid->iteration == 0 || grid_changed) {
		grid->update_coordinates_for_alive_grid_cells();
		grid_execution_state.updated_grid_coordinates = true;
	}

	if (grid->iteration == 0 || (grid_execution_state.show_chunk_borders && grid_changed) || grid_execution_state.have_to_update_chunk_borders) {
		grid->update_coordinates_for_chunk_borders();
		grid_execution_state.updated_border_coordinates = true;
	}
	// @Cleanup, factor this into a grid_info->update() call?
	update_grid_info();
}


void Grid::update_coordinates_for_alive_grid_cells() {
	ZoneScoped;

	grid_coordinates.clear();
	for (auto& [chunk_coord, chunk]: chunk_map) {
		// transform local chunk coordinates of alive grid cells into world coordinates and add them to our vector of world coordinates
		for (Coordinate coord: chunk->chunk_coordinates) {
			grid_coordinates.insert(chunk->transform_to_world_coordinate(coord));
		}
	}
	
}

void Grid::update_coordinates_for_chunk_borders() {
	ZoneScoped;

	border_coordinates.clear();
	for (auto& [chunk_coord, chunk]: chunk_map) {
		// transform local chunk coordinates of alive grid cells into world coordinates and add them to our vector of world coordinates
		for (Coordinate coord: chunk->border_coordinates) {
			border_coordinates.insert(chunk->transform_to_world_coordinate(coord));
		}
	}
}

//--------------------------------------------------------------------------------
Grid::Grid(std::shared_ptr<OpenCLContext> context) :
	opencl_context(context),
number_of_alive_cells(0),
iteration(0) {
	ZoneScoped;

	int base_row = (int) (Chunk::rows / 2);
	int base_column = (int) (Chunk::columns / 2);
	/*
	std::vector<std::pair<int, int>> initial_coordinates = { { 0, 1 }, { 0, 2 }, {0, 3} };
	create_new_chunk_and_set_alive_cells(0, 0, initial_coordinates);
	create_new_chunk_and_set_alive_cells(-1, 0, initial_coordinates);
	*/

	base_row = 1;
	base_column = 7;
	std::vector<std::pair<int, int>> initial_coordinates = {
		{ base_row, base_column },
		{ base_row + 1, base_column },
		{ base_row + 1, base_column - 1 },
		{ base_row + 1, base_column - 2 },
		{ base_row + 2, base_column - 1 },
		{ base_row, base_column - 4 },
		{ base_row, base_column - 5 },
		{ base_row, base_column - 6 },
		{ base_row + 1, base_column - 5 },
		{ base_row, base_column }
	};
	create_new_chunk_and_set_alive_cells(Coordinate(0, 0), initial_coordinates);

	for (auto& [chunk_coord, chunk]: chunk_map) {
		number_of_alive_cells += chunk->number_of_alive_cells;
	}

	update_coordinates_for_alive_grid_cells();

	/*
	{
		int chunk_origin_test_row = -2;
		int chunk_origin_test_column = 3;
		//bottom left
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row, chunk_origin_test_column, { { 0, Chunk::columns - 2 }, { 0, Chunk::columns - 1 } });
		// top left
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row - 1, chunk_origin_test_column, { { Chunk::rows - 1, Chunk::columns - 1 } });
		// top right
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row-1, chunk_origin_test_column+1, { { Chunk::rows -1, 0 } });
		// bottom right
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row, chunk_origin_test_column +1, { { 0, 0 }});
	}
	*/
}


void Grid::create_new_chunk_and_set_alive_cells(const Coordinate& coord, const std::vector<std::pair<int, int>>& coordinates) {
	ZoneScoped;

	std::shared_ptr<Chunk> chunk = std::make_shared < Chunk > (coord);
	chunk->chunk_origin_row = coord.x * Chunk::rows;
	chunk->chunk_origin_column = coord.y * Chunk::columns;
	chunk->number_of_alive_cells = 0;

	for (auto [r, c]: coordinates) {
		chunk->cells(r, c) = true;
	}
	chunk->update_chunk_coordinates();

	chunk_map.insert(std::make_pair(coord, chunk));
}

void Grid::create_new_chunk(const Coordinate& coord) {
	ZoneScoped;

	create_new_chunk_and_set_alive_cells(coord, {});
}

void Chunk::update_neighbour_count_and_set_info() {
	ZoneScoped;

	clear_neighbour_update_info();

	update_neighbour_count_inside();

	update_neighbour_count_top();
	update_neighbour_count_left();
	update_neighbour_count_right();
	update_neighbour_count_bottom();

	update_neighbour_count_corners();
}

//--------------------------------------------------------------------------------
void Grid::update() {
	ZoneScoped;
}


void Grid::create_all_needed_neighbour_chunks() {
	ZoneScoped;
	// set of coordinates of the neighbour chunks, note that this is a set and hence we do not create neighbours multiple times
	std::unordered_set<Coordinate> coordinates_of_chunks_to_create;
	for (auto& [chunk_coord, chunk]: chunk_map) {
		if (!chunk->has_to_update_neighbours()) {
			continue;
		}
		int grid_row = chunk->grid_coordinate_row;
		int grid_column = chunk->grid_coordinate_column;

		if (chunk->has_to_update_top()) {
			Coordinate top_coord = Coordinate(grid_row - 1, grid_column);
			if (!chunk_map.contains(top_coord)) {
				coordinates_of_chunks_to_create.insert(top_coord);
			}
		}

		if (chunk->has_to_update_left()) {
			Coordinate left_coord = Coordinate(grid_row, grid_column - 1);
			if (!chunk_map.contains(left_coord)) {
				coordinates_of_chunks_to_create.insert(left_coord);
			}
		}

		if (chunk->has_to_update_bottom()) {
			Coordinate bottom_coord = Coordinate(grid_row + 1, grid_column);
			if (!chunk_map.contains(bottom_coord)) {
				coordinates_of_chunks_to_create.insert(bottom_coord);
			}
		}
		if (chunk->has_to_update_right()) {
			Coordinate right_coord = Coordinate(grid_row, grid_column + 1);
			if (!chunk_map.contains(right_coord)) {
				coordinates_of_chunks_to_create.insert(right_coord);
			}
		}

		if (chunk->has_to_update_top_left_corner) {
			Coordinate top_left_coord = Coordinate(grid_row - 1, grid_column - 1);
			if (!chunk_map.contains(top_left_coord)) {
				coordinates_of_chunks_to_create.insert(top_left_coord);
			}
		}
		if (chunk->has_to_update_bottom_left_corner) {
			Coordinate bottom_left_coord = Coordinate(grid_row + 1, grid_column - 1);
			if (!chunk_map.contains(bottom_left_coord)) {
				coordinates_of_chunks_to_create.insert(bottom_left_coord);
			}
		}
		if (chunk->has_to_update_bottom_right_corner) {
			Coordinate bottom_right_coord = Coordinate(grid_row + 1, grid_column + 1);
			if (!chunk_map.contains(bottom_right_coord)) {
				coordinates_of_chunks_to_create.insert(bottom_right_coord);
			}
		}
		if (chunk->has_to_update_top_right_corner) {
			Coordinate top_right_coord = Coordinate(grid_row - 1, grid_column + 1);
			if (!chunk_map.contains(top_right_coord)) {
				coordinates_of_chunks_to_create.insert(top_right_coord);
			}
		}
	}

	// create the chunks now
	for (Coordinate coord: coordinates_of_chunks_to_create) {
		create_new_chunk(coord);
	}
}

void Grid::next_iteration() {
	ZoneScoped;

	iteration++;
	for (auto& [chunk_coord, chunk]: chunk_map) {
		chunk->update_neighbour_count_and_set_info();
	}

	create_all_needed_neighbour_chunks();

	update_all_neighbours_of_all_chunks();

	update_cells_of_all_chunks();
	
	remove_empty_chunks();
}


void Grid::update_all_neighbours_of_all_chunks() {
	ZoneScoped;

	for (auto& [chunk_coord, chunk]: chunk_map) {
		update_neighbours_of_chunk(chunk);
		chunk->clear_neighbour_update_info();
	}
}


void Grid::update_cells_of_all_chunks() {
	ZoneScoped;
	
	if (opencl_context->is_valid_context) {
		for (auto& [chunk_coord, chunk]: chunk_map) {
			chunk->cells = opencl_context->get_updated_cells(chunk->neighbour_count, chunk->cells);
		}
	}
	
	for (auto& [chunk_coord, chunk]: chunk_map) {
		chunk->update_cells();
	}
}


void Grid::remove_empty_chunks() {
	number_of_alive_cells = 0;
	for (auto it = chunk_map.begin(); it != chunk_map.end();) {
		if (it->second->number_of_alive_cells <= 0) {
			it = chunk_map.erase(it); // erase does not invalidate the iterator of std::unordered_map, (insert does!)
		} else {
			// DONT FORGET TO ADVANCE THE ITERATOR!
			number_of_alive_cells += it->second->number_of_alive_cells;
			it++;
		}
	}
}


// @TODO @Cleanup deduplicate this code, all cases are very similar. We could create for each chunk a command queue which contains a vector of indices together with an enum value, which indicates the direction.
void Grid::update_neighbours_of_chunk(std::shared_ptr<Chunk> chunk) {
	ZoneScoped;

	if (!chunk->has_to_update_neighbours()) return;

	int grid_row = chunk->grid_coordinate_row;
	int grid_column = chunk->grid_coordinate_column;

	if (chunk->has_to_update_top()) {
		Coordinate top_coord = Coordinate(grid_row - 1, grid_column);
		std::shared_ptr<Chunk> top_chunk = chunk_map.find(top_coord)->second;

		for (int c: chunk->top_column_indices_to_update) {
			top_chunk->neighbour_count(Chunk::rows - 1, c)++;
			if (c > 0) {
				top_chunk->neighbour_count(Chunk::rows - 1, c - 1)++;
			}
			if (c < Chunk::columns - 1) {
				top_chunk->neighbour_count(Chunk::rows - 1, c + 1)++;
			}
		}
	}

	if (chunk->has_to_update_bottom()) {
		Coordinate bottom_coord = Coordinate(grid_row + 1, grid_column);
		std::shared_ptr<Chunk> bottom_chunk = chunk_map.find(bottom_coord)->second;

		for (int c: chunk->bottom_column_indices_to_update) {
			bottom_chunk->neighbour_count(0, c)++;
			if (c > 0) {
				bottom_chunk->neighbour_count(0, c - 1)++;
			}
			if (c < Chunk::columns - 1) {
				bottom_chunk->neighbour_count(0, c + 1)++;
			}
		}
	}

	if (chunk->has_to_update_left()) {
		Coordinate left_coord = Coordinate(grid_row, grid_column - 1);
		std::shared_ptr<Chunk> left_chunk = chunk_map.find(left_coord)->second;

		for (int r: chunk->left_row_indices_to_update) {
			left_chunk->neighbour_count(r, Chunk::columns - 1)++;
			if (r > 0) {
				left_chunk->neighbour_count(r - 1, Chunk::columns - 1)++;
			}
			if (r < Chunk::rows - 1) {
				left_chunk->neighbour_count(r + 1, Chunk::columns - 1)++;
			}
		}
	}

	if (chunk->has_to_update_right()) {
		Coordinate right_coord = Coordinate(grid_row, grid_column + 1);
		std::shared_ptr<Chunk> right_chunk = chunk_map.find(right_coord)->second;

		for (int r: chunk->right_row_indices_to_update) {
			right_chunk->neighbour_count(r, 0)++;
			if (r > 0) {
				right_chunk->neighbour_count(r - 1, 0)++;
			}
			if (r < Chunk::rows - 1) {
				right_chunk->neighbour_count(r + 1, 0)++;
			}
		}
	}

	if (chunk->has_to_update_top_left_corner) {
		Coordinate top_left_coord = Coordinate(grid_row - 1, grid_column - 1);
		std::shared_ptr<Chunk> top_left_chunk = chunk_map.find(top_left_coord)->second;

		top_left_chunk->neighbour_count(Chunk::rows - 1 , Chunk::columns - 1)++;
	}
	if (chunk->has_to_update_top_right_corner) {
		Coordinate top_right_coord = Coordinate(grid_row - 1, grid_column + 1);
		std::shared_ptr<Chunk> top_right_chunk = chunk_map.find(top_right_coord)->second;

		top_right_chunk->neighbour_count(Chunk::rows - 1, 0)++;
	}
	if (chunk->has_to_update_bottom_left_corner) {
		Coordinate bottom_left_coord = Coordinate(grid_row + 1, grid_column - 1);
		std::shared_ptr<Chunk> bottom_left_chunk = chunk_map.find(bottom_left_coord)->second;

		bottom_left_chunk->neighbour_count(0, Chunk::columns - 1)++;
	}
	if (chunk->has_to_update_bottom_right_corner) {
		Coordinate bottom_right_coord = Coordinate(grid_row + 1, grid_column + 1);
		std::shared_ptr<Chunk> bottom_right_chunk = chunk_map.find(bottom_right_coord)->second;

		bottom_right_chunk->neighbour_count(0, 0)++;
	}
}

void Grid::print_all_chunks_info() {
	ZoneScoped;

	for (auto& [chunk_coord, chunk]: chunk_map) {
		chunk->print_chunk();
	}
	std::cout << "##################################################\n";
}
