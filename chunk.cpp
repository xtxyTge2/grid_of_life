#pragma once

#include "chunk.hpp"



Chunk::Chunk(const Coordinate& coord) :
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

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			unsigned int count = neighbour_count(r, c);
			if (cells(r, c)) {
				// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
				if (count != 2 && count != 3) {
					cells(r, c) = false;
					chunk_coordinates.erase(Coordinate(r, c));
				}
			} else {
				// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
				if (count == 3) {
					cells(r, c) = true;
					chunk_coordinates.insert(Coordinate(r, c));
				}
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
			chunk_coordinates.insert(Coordinate(r, c));
		}
	}

	for (int i = 0; i < number_current_dead_grid_cells; i++) {
		Coordinate& coord = dead_grid_cells_coordinates.at(i);
		int r = coord.x;
		int c = coord.y;
		unsigned int count = neighbour_count(r, c);
		if (count == 3) {
			chunk_coordinates.insert(Coordinate(r, c));
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
				chunk_coordinates.insert(Coordinate(r, c));
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
				chunk_coordinates.insert(Coordinate(r, c));
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