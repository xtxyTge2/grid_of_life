#pragma once

#include "world.hpp"


class Cube_System {
public:
	Cube_System();

	void update(Grid_Manager* grid_manager);

	void create_border_cubes_from_coordinates(std::unordered_set<Coordinate> coordinates);

	void create_grid_cubes_from_coordinates(std::unordered_set<Coordinate> coordinates);

	void clear_border_and_grid_cubes_array();

	Cube* create_new_cube();
	//--------------------------------------------------------------------------------
	// data
	int current_number_of_grid_cubes;
	constexpr static size_t MAX_NUMBER_OF_GRID_CUBES = 50000;
	std::array<Cube, MAX_NUMBER_OF_GRID_CUBES> grid_cubes;

	int current_number_of_border_cubes;
	constexpr static size_t MAX_NUMBER_OF_BORDER_CUBES = 50000;
	std::array<Cube, MAX_NUMBER_OF_BORDER_CUBES> border_cubes;
};