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


Grid_Info Grid_Manager::get_grid_info() {
	ZoneScoped;

	Grid_Info grid_info = {};
	grid_info.iteration = grid->iteration;
	grid_info.rows = 0;
	grid_info.columns = 0;
	grid_info.origin_row = 0;
	grid_info.origin_column = 0;
	grid_info.number_of_alive_cells = grid->number_of_alive_cells;
	return grid_info;
}

//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;
	
	grid_execution_state = {};
	grid = std::make_unique < Grid > ();
}

void Grid_Manager::update_grid_execution_state(Grid_UI_Controls_Info ui_info) {
	ZoneScoped;

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
	// check if the user changed to show/hide the grid borders.
	grid_execution_state.have_to_update_chunk_borders = grid_execution_state.show_chunk_borders != ui_info.show_chunk_borders;
	
	// set it to actual new value always.
	grid_execution_state.show_chunk_borders = ui_info.show_chunk_borders;
	grid_execution_state.grid_speed = ui_info.grid_speed_slider_value;
	grid_execution_state.should_run_at_max_possible_speed = ui_info.run_grid_at_max_possible_speed;
}

void Grid_Manager::update(double dt, Grid_UI_Controls_Info ui_info) {
	ZoneScoped;
	
	update_grid_execution_state(ui_info);
	
	bool grid_changed = false;

	if (grid_execution_state.is_running) {
		//assert(grid_execution_state.grid_speed > 0.0f);
		
		grid_execution_state.time_since_last_iteration += (float) dt;
		float threshold = 1.0f / grid_execution_state.grid_speed;
		if (grid_execution_state.should_run_at_max_possible_speed || grid_execution_state.time_since_last_iteration >= threshold) {
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
		update_coordinates_for_alive_grid_cells();
	}

	if (grid->iteration == 0 || grid_changed || grid_execution_state.have_to_update_chunk_borders) {
		update_coordinates_for_chunk_borders();
	}
}

void Grid_Manager::update_coordinates_for_alive_grid_cells() {
	ZoneScoped;

	world_coordinates.clear();
	for (auto& [chunk_coord, chunk]: grid->chunk_map) {
		// transform local chunk coordinates of alive grid cells into world coordinates and add them to our vector of world coordinates
		for (Coordinate coord: chunk->chunk_coordinates) {
			world_coordinates.insert(chunk->transform_to_world_coordinate(coord));
		}
	}
}

void Grid_Manager::update_coordinates_for_chunk_borders() {
	ZoneScoped;

	border_coordinates.clear();
	if (grid_execution_state.show_chunk_borders) {
		for (auto& [chunk_coord, chunk]: grid->chunk_map) {
			// transform local border coordinates (which are in chunk coordinates) into world coordinates and add them to our vector of border coordinates
			for (Coordinate coord: chunk->border_coordinates) {
				border_coordinates.push_back(chunk->transform_to_world_coordinate(coord));
			}
		}
	}
}

//--------------------------------------------------------------------------------
Grid::Grid() : number_of_alive_cells(0), iteration(0) {
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


void Grid::create_new_chunk_and_set_alive_cells(Coordinate coord, std::vector<std::pair<int, int>> coordinates) {
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

void Grid::create_new_chunk(Coordinate coord) {
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
	std::unordered_set<Coordinate> coordinates_of_chunks_to_add;
	for (auto& [chunk_coord, chunk]: chunk_map) {
		if (!chunk->has_to_update_neighbours()) {
			continue;
		}
		int grid_row = chunk->grid_coordinate_row;
		int grid_column = chunk->grid_coordinate_column;

		if (chunk->has_to_update_top()) {
			Coordinate top_coord = Coordinate(grid_row - 1, grid_column);
			if (!chunk_map.contains(top_coord)) {
				coordinates_of_chunks_to_add.insert(top_coord);
			}
		}
		if (chunk->has_to_update_left()) {
			Coordinate left_coord = Coordinate(grid_row, grid_column - 1);
			if (!chunk_map.contains(left_coord)) {
				coordinates_of_chunks_to_add.insert(left_coord);
			}
		}
		if (chunk->has_to_update_bottom()) {
			Coordinate bottom_coord = Coordinate(grid_row + 1, grid_column);
			if (!chunk_map.contains(bottom_coord)) {
				coordinates_of_chunks_to_add.insert(bottom_coord);
			}
		}
		if (chunk->has_to_update_right()) {
			Coordinate right_coord = Coordinate(grid_row, grid_column + 1);
			if (!chunk_map.contains(right_coord)) {
				coordinates_of_chunks_to_add.insert(right_coord);
			}
		}

		if (chunk->has_to_update_top_left_corner) {
			Coordinate top_left_coord = Coordinate(grid_row - 1, grid_column - 1);
			if (!chunk_map.contains(top_left_coord)) {
				coordinates_of_chunks_to_add.insert(top_left_coord);
			}
		}
		if (chunk->has_to_update_bottom_left_corner) {
			Coordinate bottom_left_coord = Coordinate(grid_row + 1, grid_column - 1);
			if (!chunk_map.contains(bottom_left_coord)) {
				coordinates_of_chunks_to_add.insert(bottom_left_coord);
			}
		}
		if (chunk->has_to_update_bottom_right_corner) {
			Coordinate bottom_right_coord = Coordinate(grid_row + 1, grid_column + 1);
			if (!chunk_map.contains(bottom_right_coord)) {
				coordinates_of_chunks_to_add.insert(bottom_right_coord);
			}
		}
		if (chunk->has_to_update_top_right_corner) {
			Coordinate top_right_coord = Coordinate(grid_row - 1, grid_column + 1);
			if (!chunk_map.contains(top_right_coord)) {
				coordinates_of_chunks_to_add.insert(top_right_coord);
			}
		}
	}

	// create the chunks now
	for (Coordinate coord: coordinates_of_chunks_to_add) {
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


Chunk::Chunk(Coordinate coord) :
	grid_coordinate_row(coord.x),
	grid_coordinate_column(coord.y),
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


	border_coordinates.clear();
	// calculate border coordinates.
	border_coordinates.reserve(2 * rows + 2 * columns + 4);
	for (int r = 0; r < rows; r++) {
		border_coordinates.push_back(Coordinate(r, - 1));
		border_coordinates.push_back(Coordinate(r, columns));
	}
	for (int c = 0; c < Chunk::columns; c++) {
		border_coordinates.push_back(Coordinate(-1, c));
		border_coordinates.push_back(Coordinate(rows, c));
	}
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

Coordinate Chunk::transform_to_world_coordinate(Coordinate chunk_coord) {
	ZoneScoped;

	return Coordinate(chunk_coord.x + chunk_origin_row, chunk_coord.y + chunk_origin_column);
}

void Chunk::update_neighbour_count_top() {
	ZoneScoped;

	int r = 0;
	for (int c = 1; c < columns - 1; c++) {
		if (cells(r, c)) {
			top_column_indices_to_update.push_back(c);
			
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
		has_to_update_top_left_corner = true;
		left_row_indices_to_update.push_back(0);
		top_column_indices_to_update.push_back(0);

		neighbour_count(1, 0)++;
		neighbour_count(0, 1)++;
		neighbour_count(1, 1)++;
	}

	has_to_update_top_right_corner = false;
	if (cells(0, columns - 1)) {
		has_to_update_top_right_corner = true;
		top_column_indices_to_update.push_back(columns - 1);
		right_row_indices_to_update.push_back(0);

		neighbour_count(0, columns - 2)++;
		neighbour_count(1, columns - 1)++;
		neighbour_count(1, columns - 2)++;
	}

	has_to_update_bottom_left_corner = false;
	if (cells(rows - 1, 0)) {
		has_to_update_bottom_left_corner = true;
		left_row_indices_to_update.push_back(rows - 1);
		bottom_column_indices_to_update.push_back(0);

		neighbour_count(rows - 1, 1)++;
		neighbour_count(rows - 2, 0)++;
		neighbour_count(rows - 2, 1)++;
	}

	has_to_update_bottom_right_corner = false;
	if (cells(rows - 1 , columns - 1)) {

		has_to_update_bottom_right_corner = true;
		bottom_column_indices_to_update.push_back(columns - 1);
		right_row_indices_to_update.push_back(columns - 1);
		
		neighbour_count(rows - 1, columns - 2)++;
		neighbour_count(rows - 2, columns - 1)++;
		neighbour_count(rows - 2, columns - 2)++;
	}
}

bool Chunk::has_to_update_left() {
	ZoneScoped;
	return left_row_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_right() {
	ZoneScoped;
	return right_row_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_bottom() {
	ZoneScoped;
	return bottom_column_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_top() {
	ZoneScoped;
	return top_column_indices_to_update.size() > 0;
}

bool Chunk::has_to_update_neighbours() {
	ZoneScoped;
	return has_to_update_top() || has_to_update_left() || has_to_update_right() || has_to_update_bottom() || has_to_update_corners();
}

bool Chunk::has_to_update_corners() {
	ZoneScoped;
	return has_to_update_top_left_corner || has_to_update_top_right_corner || has_to_update_bottom_left_corner || has_to_update_bottom_right_corner;
}

void Chunk::update_neighbour_count_inside() {
	ZoneScoped;
	// @Speed: just memset to zero ?
	neighbour_count.setConstant(0);


	for (int r = 1; r < rows - 1; r++) {
		for (int c = 1; c < columns - 1; c++) {
			if (cells(r, c)) {
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

	constexpr static int version_number = 1;
	switch (version_number) {
		case 1:
			update_cells_first_version();
			break;
		case 2:
			update_cells_second_version();
			break;
		case 3:
			update_cells_third_version();
			break;
		default:
			break;
	}
}


void Chunk::update_cells_first_version() {
	ZoneScoped;

	chunk_coordinates.clear();
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			unsigned int count = neighbour_count(r, c);
			if (cells(r, c)) {
				// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
				if (count != 2 && count != 3) {
					cells(r, c) = false;
					//chunk_coordinates.erase(Coordinate(r, c));
				}
			} else {
				// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
				if (count == 3) {
					cells(r, c) = true;
					//chunk_coordinates.insert(Coordinate(r, c));
				}
			}
			if (cells(r, c)) {
				chunk_coordinates.push_back(Coordinate(r, c));
			}
		}
	}
	number_of_alive_cells = (int) chunk_coordinates.size();
}

void Chunk::update_cells_second_version() {
	ZoneScoped;
	
	std::array<Coordinate, rows*columns> alive_grid_cells_coordinates;
	size_t number_current_alive_grid_cells = 0;
	std::array<Coordinate, rows*columns> dead_grid_cells_coordinates;
	size_t number_current_dead_grid_cells = 0;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				Coordinate& coord = alive_grid_cells_coordinates.at(number_current_alive_grid_cells);
				coord.x = r;
				coord.y = c;
				number_current_alive_grid_cells++;
			} else {
				Coordinate& coord = dead_grid_cells_coordinates.at(number_current_dead_grid_cells);
				coord.x = r;
				coord.y = c;
				number_current_dead_grid_cells++;
			}
		}
	}

	chunk_coordinates.clear();
	for (int i = 0; i < number_of_alive_cells; i++) {
		Coordinate& coord = alive_grid_cells_coordinates.at(i);
		int r = coord.x;
		int c = coord.y;
		unsigned int count = neighbour_count(r, c);
		if (count == 2 || count == 3) {
			chunk_coordinates.push_back(Coordinate(r, c));
		}
	}

	for (int i = 0; i < number_current_dead_grid_cells; i++) {
		Coordinate& coord = dead_grid_cells_coordinates.at(i);
		int r = coord.x;
		int c = coord.y;
		unsigned int count = neighbour_count(r, c);
		if (count == 3) {
			chunk_coordinates.push_back(Coordinate(r, c));
		}
	}

	cells.setConstant(0);
	for (auto& [r, c]: chunk_coordinates) {
		cells(r, c) = true;
	}
	number_of_alive_cells = (int) chunk_coordinates.size();
		
}


void Chunk::update_cells_third_version() {
	ZoneScoped;
	int x = 10;
	Eigen::Array < unsigned int, rows, columns, Eigen::RowMajor > constant_equal_to_3_matrix;
	constant_equal_to_3_matrix.setConstant(3);

	Eigen::Array < unsigned int, rows, columns, Eigen::RowMajor > constant_equal_to_2_matrix;
	constant_equal_to_2_matrix.setConstant(2);

	auto neighbour_count_equal_to_3_matrix = neighbour_count == constant_equal_to_3_matrix;

	auto neighbour_count_equal_to_2_or_3_matrix = (neighbour_count == constant_equal_to_2_matrix) || neighbour_count_equal_to_3_matrix;

	auto alive_and_2_or_3_neighbours = cells && neighbour_count_equal_to_2_or_3_matrix;
	
	auto dead_and_3_neighbours = !cells && neighbour_count_equal_to_3_matrix;

	auto alive_and_2_or_3_neighbours_evaluated = alive_and_2_or_3_neighbours.eval();
	
	auto dead_and_3_neighbours_evaluated = dead_and_3_neighbours.eval();

	auto next_cells = alive_and_2_or_3_neighbours || dead_and_3_neighbours;

	cells = next_cells.eval();

	chunk_coordinates.clear();
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				chunk_coordinates.push_back(Coordinate(r, c));
			}
		}
	}

	number_of_alive_cells = (int) chunk_coordinates.size();	
}



void Chunk::update_chunk_coordinates() {
	ZoneScoped;

	chunk_coordinates.clear();
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				chunk_coordinates.push_back(Coordinate(r, c));
			}
		}
	}
	number_of_alive_cells = (int) chunk_coordinates.size();
}


void Chunk::print_chunk() {
	ZoneScoped;

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
	ZoneScoped;

	for (auto& [chunk_coord, chunk]: chunk_map) {
		chunk->print_chunk();
	}
	std::cout << "##################################################\n";
}
