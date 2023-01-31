#pragma once

#include "grid.hpp"
#include "Tracy.hpp"
#include <iostream>

using Eigen::Array;
using Eigen::Dynamic;


Grid_Manager::Grid_Manager()
: grid_execution_state({})
{
	ZoneScoped;

	create_new_grid();
};


//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;
	
	grid_execution_state = {};
	grid = new Grid();
}

void Grid_Manager::update(double dt, Grid_UI_Controls_Info ui_info) {
	switch (ui_info.button_type) {
		case GRID_NO_BUTTON_PRESSED:
			break;
		case GRID_RESET_BUTTON_PRESSED:
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
	grid_execution_state.show_chunk_borders = ui_info.show_chunk_borders;
	grid_execution_state.grid_speed = ui_info.grid_speed_slider_value;
	
	bool grid_changed = false;

	if (grid_execution_state.is_running) {
		//assert(grid_execution_state.grid_speed > 0.0f);
		grid_execution_state.time_since_last_iteration += (float) dt;
		float threshold = 1.0f / grid_execution_state.grid_speed;
		if (grid_execution_state.time_since_last_iteration >= threshold) {
			grid->next_iteration();
			grid_changed = true;
			grid_execution_state.time_since_last_iteration = 0.0f;
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
		world_coordinates.clear();
		for (Chunk& chunk: grid->chunks) {
			for (std::pair<int, int>& coord: chunk.chunk_coordinates) {
				world_coordinates.push_back(chunk.transform_to_world_coordinate(coord));
			}
		}
	}
}

//--------------------------------------------------------------------------------
Grid::Grid() : number_of_alive_cells(0), iteration(0) {
	ZoneScoped;

	int chunk_middle_row = (int) (Chunk::rows / 2);
	int chunk_middle_column = (int) (Chunk::columns / 2);
	/*
	std::vector<std::pair<int, int>> initial_coordinates = { { 0, 1 }, { 0, 2 }, {0, 3} };
	create_new_chunk_and_set_alive_cells(0, 0, initial_coordinates);
	create_new_chunk_and_set_alive_cells(-1, 0, initial_coordinates);
	*/
	
	std::vector<std::pair<int, int>> initial_coordinates = {
		{ chunk_middle_row, chunk_middle_column },
		{ chunk_middle_row + 1, chunk_middle_column },
		{ chunk_middle_row + 1, chunk_middle_column - 1 },
		{ chunk_middle_row + 1, chunk_middle_column - 2 },
		{ chunk_middle_row + 2, chunk_middle_column - 1 },
		{ chunk_middle_row, chunk_middle_column - 4 },
		{ chunk_middle_row, chunk_middle_column - 5 },
		{ chunk_middle_row, chunk_middle_column - 6 },
		{ chunk_middle_row + 1, chunk_middle_column - 5 },
		{ chunk_middle_row, chunk_middle_column }
	};
	create_new_chunk_and_set_alive_cells(0, 0, initial_coordinates);
	for (Chunk& chunk: chunks) {
		number_of_alive_cells += chunk.number_of_alive_cells;
	}
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

void Grid::create_new_chunk_and_set_alive_cells(int i, int j, std::vector<std::pair<int, int>> coordinates) {
	ZoneScoped;

	Chunk* chunk = new Chunk(i, j);
	chunk->chunk_origin_row = i * Chunk::rows;
	chunk->chunk_origin_column = j * Chunk::columns;
	chunk->number_of_alive_cells = 0;

	for (auto [r, c]: coordinates) {
		chunk->cells(r, c) = true;
	}
	chunk->update_chunk_coordinates();

	chunks.push_back(*chunk);
}

void Grid::create_new_chunk(int i, int j) {
	ZoneScoped;

	Chunk* chunk = new Chunk(i, j);
	chunk->chunk_origin_row = i * Chunk::rows;
	chunk->chunk_origin_column = j * Chunk::columns;
	chunk->number_of_alive_cells = 0;

	chunks.push_back(*chunk);
}


void Grid_Manager::create_cube(std::pair<int, int> coord, bool is_border) {
	/*
	ZoneScoped;

	Cube* cube = new Cube();

	int x = coord.first;
	int y = coord.second;
	// note the switch in y and x coordinates here!
	cube->m_position = glm::vec3((float) y, (float) -x, -3.0f);

	// do this to distinguish the border from the rest of the grid.
	if (is_border) {
		cube->m_angle = 50.0f;
	} else {
		cube->m_angle = 0.0f;
	}
	
	cubes.push_back(*cube);
	*/
}

//--------------------------------------------------------------------------------
void Grid_Manager::create_cubes_for_alive_grid_cells() {
	ZoneScoped;
	/*
	cubes.clear();
	for (Chunk& chunk: grid->chunks) {
		for (std::pair<int, int>& coord: chunk.chunk_coordinates) {
			std::pair<int, int> world_coordinate = chunk.transform_to_world_coordinate(coord);
			create_cube(world_coordinate, false);
		}
		
		if (grid_execution_state.show_chunk_borders) {
			// add border cubes
			for (int r = 0; r < Chunk::rows; r++) {
				std::pair<int, int> coord_left = std::make_pair(r, - 1);
				std::pair<int, int> coord_right = std::make_pair(r, Chunk::columns);
		
				create_cube(chunk.transform_to_world_coordinate(coord_left), true);
				create_cube(chunk.transform_to_world_coordinate(coord_right), true);
			}
			
			for (int c = 0; c < Chunk::columns; c++) {
				std::pair<int, int> coord_top = std::make_pair(-1, c);
				std::pair<int, int> coord_bottom = std::make_pair(Chunk::rows, c);
		
				create_cube(chunk.transform_to_world_coordinate(coord_top), true);
				create_cube(chunk.transform_to_world_coordinate(coord_bottom), true);
			}
		}
	}
	*/
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

void Grid::next_iteration() {
	ZoneScoped;

	iteration++;
	for (Chunk& chunk: chunks) {
		chunk.update_neighbour_count_and_set_info();
	}

	// have updated internally each chunk and set the border information, so
	// that neighbouring chunks can now update each other if needed.

	// we iterate over every chunk once and check if it has to update its neighbours,
	// in that case we either already have the corresponding neighbour inside chunks
	// or it doesnt exist yet. If it doesnt exist we add it to chunks at the end and hence iterate over it later. This invalidates any iterators of std::vector, hence we manually loop over it here.
	
	for (int chunk_index = 0; chunk_index < chunks.size(); chunk_index++) {
		Chunk current_chunk = chunks[chunk_index];
		update_neighbours_of_chunk(current_chunk);
		chunks[chunk_index].clear_neighbour_update_info();
	}

	number_of_alive_cells = 0;
	for (Chunk& chunk: chunks) {
		chunk.update_cells();
		number_of_alive_cells += chunk.number_of_alive_cells;
	}
	std::vector<Chunk> used_chunks;
	used_chunks.reserve(chunks.size());
	for (Chunk& chunk: chunks) {
		if (chunk.number_of_alive_cells > 0) {
			used_chunks.push_back(chunk);
		}
	}
	chunks = used_chunks;
}


void Chunk::print_chunk() {
	if (false) {
		std::cout << "\n\n";
		std::cout << "i: " << grid_coordinate_row << ", j: " << grid_coordinate_column << "\n";
		std::cout << "neighbour_count of chunk:\n";
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				std::cout << neighbour_count(r, c) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "cells of chunk:\n";
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				std::cout << cells(r, c) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "left_update: ";
		for (int v : left_row_indices_to_update) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		std::cout << "top_update: ";
		for (int v : top_column_indices_to_update) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		std::cout << "right_update: ";
		for (int v : right_row_indices_to_update) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		std::cout << "bottom_update: ";
		for (int v : bottom_column_indices_to_update) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		std::cout << "update_top_left_corner: " << has_to_update_top_left_corner << "\n";
		std::cout << "update_top_right_corner: " << has_to_update_top_right_corner << "\n";
		std::cout << "update_bottom_right_corner: " << has_to_update_bottom_right_corner << "\n";
		std::cout << "update_bottom_left_corner: " << has_to_update_bottom_left_corner << "\n";
	}
}

void Grid::print_all_chunks_info() {
	for (Chunk& chunk: chunks) {
		chunk.print_chunk();
	}
	std::cout << "##################################################\n";
}

// @TODO @Cleanup deduplicate this code, all cases are very similar. We could create for each chunk a command queue which contains a vector of indices together with an enum value, which indicates the direction.
void Grid::update_neighbours_of_chunk(Chunk& chunk) {
	if (!chunk.has_to_update_neighbours()) return;

	int grid_row = chunk.grid_coordinate_row;
	int grid_column = chunk.grid_coordinate_column;

	if (chunk.has_to_update_top()) {
		int top_row = grid_row - 1;
		int top_column = grid_column;
		Chunk* top_chunk = get_chunk_if_it_exists(top_row, top_column);
		if (top_chunk == nullptr) {
			create_new_chunk(top_row, top_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		top_chunk = get_chunk_if_it_exists(top_row, top_column);

		for (int c: chunk.top_column_indices_to_update) {
			top_chunk->neighbour_count(Chunk::rows - 1, c)++;
			if (c > 0) {
				top_chunk->neighbour_count(Chunk::rows - 1, c - 1)++;
			}
			if (c < Chunk::columns - 1) {
				top_chunk->neighbour_count(Chunk::rows - 1, c + 1)++;
			}
		}
	}

	if (chunk.has_to_update_bottom()) {
		int bottom_row = grid_row + 1;
		int bottom_column = grid_column;
		Chunk* bottom_chunk = get_chunk_if_it_exists(bottom_row, bottom_column);
		if (bottom_chunk == nullptr) {
			create_new_chunk(bottom_row, bottom_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		bottom_chunk = get_chunk_if_it_exists(bottom_row, bottom_column);

		for (int c: chunk.bottom_column_indices_to_update) {
			bottom_chunk->neighbour_count(0, c)++;
			if (c > 0) {
				bottom_chunk->neighbour_count(0, c - 1)++;
			}
			if (c < Chunk::columns - 1) {
				bottom_chunk->neighbour_count(0, c + 1)++;
			}
		}
	}

	if (chunk.has_to_update_left()) {
		int left_row = grid_row;
		int left_column = grid_column - 1;
		Chunk* left_chunk = get_chunk_if_it_exists(left_row, left_column);
		if (left_chunk == nullptr) {
			create_new_chunk(left_row, left_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		left_chunk = get_chunk_if_it_exists(left_row, left_column);

		for (int r: chunk.left_row_indices_to_update) {
			left_chunk->neighbour_count(r, Chunk::columns - 1)++;
			if (r > 0) {
				left_chunk->neighbour_count(r - 1, Chunk::columns - 1)++;
			}
			if (r < Chunk::rows - 1) {
				left_chunk->neighbour_count(r + 1, Chunk::columns - 1)++;
			}
		}
	}

	if (chunk.has_to_update_right()) {
		int top_row = grid_row;
		int right_column = grid_column + 1;
		Chunk* right_chunk = get_chunk_if_it_exists(top_row, right_column);
		if (right_chunk == nullptr) {
			create_new_chunk(top_row, right_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		right_chunk = get_chunk_if_it_exists(top_row, right_column);

		for (int r: chunk.right_row_indices_to_update) {
			right_chunk->neighbour_count(r, 0)++;
			if (r > 0) {
				right_chunk->neighbour_count(r - 1, 0)++;
			}
			if (r < Chunk::rows - 1) {
				right_chunk->neighbour_count(r + 1, 0)++;
			}
		}
	}

	if (chunk.has_to_update_top_left_corner) {
		int top_left_row = grid_row - 1;
		int top_left_column = grid_column - 1;
		Chunk* top_left_chunk = get_chunk_if_it_exists(top_left_row, top_left_column);
		if (top_left_chunk == nullptr) {
			create_new_chunk(top_left_row, top_left_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		top_left_chunk = get_chunk_if_it_exists(top_left_row, top_left_column);

		top_left_chunk->neighbour_count(Chunk::rows -1 , Chunk::columns - 1)++;
	}
	if (chunk.has_to_update_top_right_corner) {
		int top_right_row = grid_row - 1;
		int top_right_column = grid_column + 1;
		Chunk* top_right_chunk = get_chunk_if_it_exists(top_right_row, top_right_column);
		if (top_right_chunk == nullptr) {
			create_new_chunk(top_right_row, top_right_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		top_right_chunk = get_chunk_if_it_exists(top_right_row, top_right_column);

		top_right_chunk->neighbour_count(Chunk::rows - 1, 0)++;
	}
	if (chunk.has_to_update_bottom_left_corner) {
		int bottom_left_row = grid_row + 1;
		int bottom_left_corner = grid_column - 1;
		Chunk* bottom_left_chunk = get_chunk_if_it_exists(bottom_left_row, bottom_left_corner);
		if (bottom_left_chunk == nullptr) {
			create_new_chunk(bottom_left_row, bottom_left_corner);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		bottom_left_chunk = get_chunk_if_it_exists(bottom_left_row, bottom_left_corner);

		bottom_left_chunk->neighbour_count(0, Chunk::columns - 1)++;
	}

	if (chunk.has_to_update_bottom_right_corner) {
		int bottom_right_row = grid_row + 1;
		int bottom_right_column = grid_column + 1;
		Chunk* bottom_right_chunk = get_chunk_if_it_exists(bottom_right_row, bottom_right_column);
		if (bottom_right_chunk == nullptr) {
			create_new_chunk(bottom_right_row, bottom_right_column);
		}
		// @Speed @Hack change our functions so we actually get the top_chunk and dont have to find it again in chunks.
		bottom_right_chunk = get_chunk_if_it_exists(bottom_right_row, bottom_right_column);

		bottom_right_chunk->neighbour_count(0, 0)++;
	}

	// dont need this but we just make sure we dont get stupid bugs if we somehow manage to insert the chunk again, even though this doesnt happen and we look out 
	// so we dont. just being super careful with respect to future potential bugs.
	chunk.clear_neighbour_update_info();
}

Chunk* Grid::get_chunk_if_it_exists(int grid_row, int grid_column) {
	for (Chunk& chunk: chunks) {
		if (chunk.grid_coordinate_row == grid_row && chunk.grid_coordinate_column == grid_column) {
			return &chunk;
		}
	}
	return nullptr;
}

Chunk::Chunk(int coordinate_row, int coordinate_column) :
	grid_coordinate_row(coordinate_row),
grid_coordinate_column(coordinate_column),
number_of_alive_cells(0),
has_to_update_top_left_corner(false),
has_to_update_top_right_corner(false),
has_to_update_bottom_right_corner(false),
has_to_update_bottom_left_corner(false)
{
	ZoneScoped;
	cells.setConstant(false);
	neighbour_count.setConstant(0);

	left_row_indices_to_update.reserve(rows);
	right_row_indices_to_update.reserve(rows);
	top_column_indices_to_update.reserve(columns);
	bottom_column_indices_to_update.reserve(columns);
}


void Chunk::clear_neighbour_update_info() {
	ZoneScoped;

	left_row_indices_to_update.clear();
	right_row_indices_to_update.clear();
	top_column_indices_to_update.clear();
	bottom_column_indices_to_update.clear();

	has_to_update_top_left_corner = false;
	has_to_update_top_right_corner = false;
	has_to_update_bottom_right_corner = false;
	has_to_update_bottom_left_corner = false;
}

std::pair<int, int> Chunk::transform_to_world_coordinate(std::pair<int, int> chunk_coord) {
	return std::make_pair(chunk_coord.first + chunk_origin_row, chunk_coord.second + chunk_origin_column);
}

void Chunk::update_neighbour_count_top() {
	ZoneScoped;

	int r = 0;
	for (int c = 1; c < columns - 1; c++) {
		if (cells(r, c)) {
			number_of_alive_cells++;
			// remember the column to update for the top neighbour
			top_column_indices_to_update.push_back(c);
			
			// update all neighbours except the one above
			neighbour_count(r, c - 1)++;
			neighbour_count(r, c + 1)++;
			
			neighbour_count(r + 1, c - 1)++;
			neighbour_count(r + 1, c)++;
			neighbour_count(r + 1, c + 1)++;
		}
	}
}

void Chunk::update_neighbour_count_bottom() {
	ZoneScoped;

	int r = rows - 1;
	for (int c = 1; c < columns - 1; c++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			bottom_column_indices_to_update.push_back(c);

			neighbour_count(r - 1, c - 1)++;
			neighbour_count(r - 1, c)++;
			neighbour_count(r - 1, c + 1)++;
			
			neighbour_count(r, c - 1)++;
			neighbour_count(r, c + 1)++;
		}
	}
}

void Chunk::update_neighbour_count_left() {
	ZoneScoped;

	int c = 0;
	for (int r = 1; r < rows - 1; r++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			left_row_indices_to_update.push_back(r);

			neighbour_count(r - 1, c)++;
			neighbour_count(r - 1, c + 1)++;
			neighbour_count(r, c + 1)++;
			neighbour_count(r + 1, c)++;
			neighbour_count(r + 1, c + 1)++;
		}
	}
}

void Chunk::update_neighbour_count_right() {
	ZoneScoped;

	int c = columns - 1;
	for (int r = 1; r < rows - 1; r++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			right_row_indices_to_update.push_back(r);

			neighbour_count(r - 1, c - 1)++;
			neighbour_count(r - 1, c)++;
			neighbour_count(r, c - 1)++;
			neighbour_count(r + 1, c - 1)++;
			neighbour_count(r + 1, c)++;
		}
	}
}

void Chunk::update_neighbour_count_corners() {
	ZoneScoped;

	has_to_update_top_left_corner = false;
	if (cells(0, 0)) {
		number_of_alive_cells++;

		has_to_update_top_left_corner = true;
		left_row_indices_to_update.push_back(0);
		top_column_indices_to_update.push_back(0);

		neighbour_count(1, 0)++;
		neighbour_count(0, 1)++;
		neighbour_count(1, 1)++;
	}

	has_to_update_top_right_corner = false;
	if (cells(0, columns - 1)) {
		number_of_alive_cells++;

		has_to_update_top_right_corner = true;
		top_column_indices_to_update.push_back(columns - 1);
		right_row_indices_to_update.push_back(0);

		neighbour_count(0, columns - 2)++;
		neighbour_count(1, columns - 1)++;
		neighbour_count(1, columns - 2)++;
	}

	has_to_update_bottom_left_corner = false;
	if (cells(rows - 1, 0)) {
		number_of_alive_cells++;

		has_to_update_bottom_left_corner = true;
		left_row_indices_to_update.push_back(rows - 1);
		bottom_column_indices_to_update.push_back(0);

		neighbour_count(rows - 1, 1)++;
		neighbour_count(rows - 2, 0)++;
		neighbour_count(rows - 2, 1)++;
	}

	has_to_update_bottom_right_corner = false;
	if (cells(rows - 1 , columns - 1)) {
		number_of_alive_cells++;

		has_to_update_bottom_right_corner = true;
		bottom_column_indices_to_update.push_back(columns - 1);
		right_row_indices_to_update.push_back(columns - 1);
		
		neighbour_count(rows - 1, columns - 2)++;
		neighbour_count(rows - 2, columns - 1)++;
		neighbour_count(rows - 2, columns - 2)++;
	}
}

bool Chunk::has_to_update_left() {
	return left_row_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_right() {
	return right_row_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_bottom() {
	return bottom_column_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_top() {
	return top_column_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_neighbours() {
	return has_to_update_top() || has_to_update_left() || has_to_update_right() || has_to_update_bottom() || has_to_update_corners();
}

bool Chunk::has_to_update_corners() {
	return has_to_update_top_left_corner || has_to_update_top_right_corner || has_to_update_bottom_left_corner || has_to_update_bottom_right_corner;
}

void Chunk::update_neighbour_count_inside() {
	ZoneScoped;
	// @Speed: just memset to zero ?
	neighbour_count.setConstant(0);


	number_of_alive_cells = 0;
	
	for (int r = 1; r < rows - 1; r++) {
		for (int c = 1; c < columns - 1; c++) {
			if (cells(r, c)) {
				number_of_alive_cells++;

				neighbour_count(r - 1, c - 1)++;
				neighbour_count(r - 1, c)++;
				neighbour_count(r - 1, c + 1)++;
				neighbour_count(r, c - 1)++;
				
				neighbour_count(r, c + 1)++;
				neighbour_count(r + 1, c - 1)++;
				neighbour_count(r + 1, c)++;
				neighbour_count(r + 1, c + 1)++;
			}
		}
	}
}


void Chunk::update_cells() {
	ZoneScoped;
	number_of_alive_cells = 0;
	// TODO: split this up and handle interior and border of the grid individually. If we do that we can incorporate the resize_if_needed() call into the border case computation
	chunk_coordinates.clear();
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			unsigned int count = neighbour_count(r, c);
			if (cells(r, c)) {
				// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
				if (count != 2 && count != 3) {
					cells(r, c) = false;
				}
			} else {
				// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
				if (count == 3) {
					cells(r, c) = true;
				}
			}
			
			if (cells(r, c)) {
				chunk_coordinates.push_back(std::make_pair(r, c));
				number_of_alive_cells += 1;
			}
		}
	}
}


void Chunk::update_chunk_coordinates() {
	chunk_coordinates.clear();

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				chunk_coordinates.push_back(std::make_pair(r, c));
				number_of_alive_cells += 1;
			}
		}
	}
}

//--------------------------------------------------------------------------------
Grid_Render_Data::Grid_Render_Data() {
	
}

//--------------------------------------------------------------------------------
void Grid::resize_if_needed() {
	/*
	ZoneScoped;
	bool has_to_resize = false;
	for (int c = 0; c < columns; c++) {
	has_to_resize |= cells(0, c) || cells(rows - 1, c);
	if (has_to_resize) break;
	}
	for (int r = 0; r < rows; r++) {
	has_to_resize |= cells(r, 0) || cells(r, columns - 1);
	if (has_to_resize) break;
	}
	if (!has_to_resize) return;

	int new_rows = 3 * rows;
	int new_columns = 3 * columns;

	int row_offset = rows;
	int column_offset = columns;

	int new_origin_row = origin_row + row_offset;
	int new_origin_column = origin_column + column_offset;

	Array < bool, Dynamic, Dynamic > new_cells(new_rows, new_columns);
	new_cells.setConstant(false);

	Array < unsigned int, Dynamic, Dynamic > new_neighbour_count(new_rows, new_columns);
	new_neighbour_count.setConstant(0);

	// TODO faster
	for (int r = 0; r < rows; r++) {
	for (int c = 0; c < columns; c++) {
	bool old_value = cells(r, c);
	int new_r = r + row_offset;
	int new_c = c + column_offset;
	new_cells(new_r, new_c) = old_value;
	}
	}

	rows = new_rows;
	columns = new_columns;

	origin_row = new_origin_row;
	origin_column = new_origin_column;

	cells = new_cells;
	neighbour_count = new_neighbour_count;

	cubes.reserve(rows * columns);
	update_neighbour_count();
	*/
}




/*
	//--------------------------------------------------------------------------------
	void Grid::next_iteration_old() {
	ZoneScoped;

	iteration++;
	resize_if_needed();

	update_neighbour_count();

	// TODO: split this up and handle interior and border of the grid individually. If we do that we can incorporate the resize_if_needed() call into the border case computation
	coordinates.clear();
	for (int r = 0; r < rows; r++) {
	for (int c = 0; c < columns; c++) {
	unsigned int count = neighbour_count(r, c);
	if (cells(r, c)) {
	// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
	if (count != 2 && count != 3) {
	cells(r, c) = false;
	} else {
	coordinates.push_back(std::make_pair(r, c));
	}
	} else {
	// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
	if (count == 3) {
	cells(r, c) = true;
	coordinates.push_back(std::make_pair(r, c));
	}
	}
	}
	}
	}
	*/