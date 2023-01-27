#pragma once

#include "grid.hpp"
#include "Tracy.hpp"
#include <iostream>

using Eigen::Array;
using Eigen::Dynamic;

//--------------------------------------------------------------------------------
Grid::Grid() : number_of_alive_cells(0), iteration(0) {
	ZoneScoped;

	int chunk_internal_row = 64;
	int chunk_internal_column = 64;
	create_new_chunk(0, 0);
	Chunk& chunk = chunks.front();
	
	// set data
	chunk.cells(chunk_internal_row, chunk_internal_column) = true;
	chunk.cells(chunk_internal_row + 1, chunk_internal_column) = true;
	chunk.cells(chunk_internal_row + 1, chunk_internal_column - 1) = true;
	chunk.cells(chunk_internal_row + 1, chunk_internal_column - 2) = true;
	chunk.cells(chunk_internal_row + 2, chunk_internal_column - 1) = true;

	chunk.cells(chunk_internal_row, chunk_internal_column - 4) = true;
	chunk.cells(chunk_internal_row, chunk_internal_column - 5) = true;
	chunk.cells(chunk_internal_row, chunk_internal_column - 6) = true;
	chunk.cells(chunk_internal_row + 1, chunk_internal_column - 5) = true;

	chunk.update_neighbour_count_inside();
}

void Grid::create_new_chunk(int i, int j) {
	Chunk* chunk = new Chunk();
	chunk->chunk_origin_row = i;
	chunk->chunk_origin_column = j;
	chunk->number_of_alive_cells = 0;

	chunks.push_back(*chunk);
}

void Chunk::add_cube(std::pair<int, int> coord, bool is_border) {
	ZoneScoped;
	Cube* cube = new Cube();

	int x = coord.first;
	int y = coord.second;
	// note the switch in y and x coordinates here!
	cube->m_position = glm::vec3((float) x, (float) y, -3.0f);

	// do this to distinguish the border from the rest of the grid.
	if (is_border) {
		cube->m_angle = 50.0f;
	} else {
		cube->m_angle = 0.0f;
	}
	
	cubes.push_back(*cube);
}

//--------------------------------------------------------------------------------
void Chunk::create_cubes_for_alive_grid_cells() {
	ZoneScoped;
	cubes.clear();
	for (std::pair<int, int>& coord: chunk_coordinates) {
		std::pair<int, int> world_coordinate = transform_to_world_coordinate(coord);
		add_cube(world_coordinate, false);
	}

	// add border cubes
	for (int r = 0; r < rows; r++) {
		std::pair<int, int> coord_left = std::make_pair(r, - 1);
		std::pair<int, int> coord_right = std::make_pair(r, columns);

		add_cube(transform_to_world_coordinate(coord_left), true);
		add_cube(transform_to_world_coordinate(coord_right), true);
	}
	
	for (int c = 0; c < columns; c++) {
		std::pair<int, int> coord_top = std::make_pair(-1, c);
		std::pair<int, int> coord_bottom = std::make_pair(rows, c);

		add_cube(transform_to_world_coordinate(coord_top), true);
		add_cube(transform_to_world_coordinate(coord_bottom), true);
	}
	
}

//--------------------------------------------------------------------------------
void Grid::update() {
	
}

void Grid::next_iteration() {
	ZoneScoped;

	iteration++;
	for (Chunk& chunk: chunks) {
		chunk.update_neighbour_count_inside();
	}
	// handle border case, ie create new chunks at border if needed etc.

	for (Chunk& chunk: chunks) {
		chunk.update_cells();
	}
}

std::pair<int, int> Chunk::transform_to_world_coordinate(std::pair<int, int> chunk_coord) {
	return std::make_pair(chunk_coord.first + chunk_origin_row, chunk_coord.second + chunk_origin_column);
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

void Grid::update_neighbour_count_top() {
	/*
	ZoneScoped;
	int r = 0;
	for (int c = 1; c < columns - 1; c++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			neighbour_count(r, c - 1)++;
			neighbour_count(r, c + 1)++;
			
			neighbour_count(r + 1, c - 1)++;
			neighbour_count(r + 1, c)++;
			neighbour_count(r + 1, c + 1)++;
		}
	}
	*/
}

void Grid::update_neighbour_count_bottom() {
	/*
	ZoneScoped;
	int r = rows - 1;
	for (int c = 1; c < columns - 1; c++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			neighbour_count(r - 1, c - 1)++;
			neighbour_count(r - 1, c)++;
			neighbour_count(r - 1, c + 1)++;
			
			neighbour_count(r, c - 1)++;
			neighbour_count(r, c + 1)++;
		}
	}
	*/
}

void Grid::update_neighbour_count_left() {
	/*
	ZoneScoped;
	int c = 0;
	for (int r = 1; r < rows - 1; r++) {
		if (cells(r, c)) {
			number_of_alive_cells++;
			neighbour_count(r - 1, c)++;
			neighbour_count(r - 1, c + 1)++;
			neighbour_count(r, c + 1)++;
			neighbour_count(r + 1, c)++;
		}
	}
	*/
}

void Grid::update_neighbour_count_right() {
	/*
	ZoneScoped;
	int c = columns - 1;
	for (int r = 1; r < rows - 1; r++) {
		if (cells(r, c)) {
			number_of_alive_cells++;

			neighbour_count(r - 1, c - 1)++;
			neighbour_count(r - 1, c)++;
			neighbour_count(r, c - 1)++;
			neighbour_count(r + 1, c - 1)++;
			neighbour_count(r + 1, c)++;
		}
	}

	*/
}

void Grid::update_neighbour_count_corners() {
	/*
	ZoneScoped;

	if (cells(0, 0)) {
		number_of_alive_cells++;
		neighbour_count(1, 0)++;
		neighbour_count(0, 1)++;
		neighbour_count(1, 1)++;
	}
	if (cells(0, columns - 1)) {
		number_of_alive_cells++;
		neighbour_count(0, columns - 2)++;
		neighbour_count(1, columns - 1)++;
		neighbour_count(1, columns - 2)++;
	}
	if (cells(rows - 1, 0)) {
		number_of_alive_cells++;
		neighbour_count(rows - 1, 1)++;
		neighbour_count(rows - 2, 0)++;
		neighbour_count(rows - 2, 1)++;
	}
	if (cells(rows - 1 , columns - 1)) {
		number_of_alive_cells++;
		neighbour_count(rows - 1, columns - 2)++;
		neighbour_count(rows - 2, columns - 1)++;
		neighbour_count(rows - 2, columns - 2)++;
	}
	*/
}

//--------------------------------------------------------------------------------
void Grid::update_neighbour_count() {
	/*
	ZoneScoped;
	neighbour_count.setConstant(0);


	number_of_alive_cells = 0;
	update_neighbour_count_top();
	update_neighbour_count_bottom();
	update_neighbour_count_left();
	update_neighbour_count_right();
	update_neighbour_count_corners();

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
	*/
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

Chunk::Chunk() {
	ZoneScoped;
	cells.setConstant(false);
	neighbour_count.setConstant(0);
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
	// TODO: split this up and handle interior and border of the grid individually. If we do that we can incorporate the resize_if_needed() call into the border case computation
	chunk_coordinates.clear();
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			unsigned int count = neighbour_count(r, c);
			if (cells(r, c)) {
				// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
				if (count != 2 && count != 3) {
					cells(r, c) = false;
				} else {
					chunk_coordinates.push_back(std::make_pair(r, c));
				}
			} else {
				// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
				if (count == 3) {
					cells(r, c) = true;
					chunk_coordinates.push_back(std::make_pair(r, c));
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
Grid_Render_Data* Grid::create_render_data() {
	ZoneScoped;
	Grid_Render_Data* grid_render_data = new Grid_Render_Data();
	

	// cubes render data for each alive cell of the grid.
	for (Chunk& chunk: chunks) {
		number_of_alive_cells += chunk.number_of_alive_cells;
		chunk.create_cubes_for_alive_grid_cells();
		for (Cube& cube: chunk.cubes) {
			Cube_Render_Data* cube_render_data = cube.create_render_data();
			grid_render_data->cubes_render_data.push_back(*cube_render_data);
		}
	}

	// fill out grid info
	grid_render_data->grid_info = {};
	grid_render_data->grid_info.iteration = iteration;
	grid_render_data->grid_info.rows = 0;
	grid_render_data->grid_info.columns = 0;
	grid_render_data->grid_info.origin_row = 0;
	grid_render_data->grid_info.origin_column = 0;
	grid_render_data->grid_info.number_of_alive_cells = number_of_alive_cells;
	return grid_render_data;
};

//--------------------------------------------------------------------------------
Grid_Render_Data::Grid_Render_Data() {
	
}