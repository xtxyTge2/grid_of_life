#pragma once

#include "world.hpp"

class Cube_System {
public:
	Cube_System();

	void update(Grid_Manager* grid_manager);

	void clear();

	Cube* create_new_cube();
	//--------------------------------------------------------------------------------
	// data
	int current_number_of_cubes;
	constexpr static size_t MAX_NUMBER_OF_CUBES = 50000;
	std::array<Cube, MAX_NUMBER_OF_CUBES> cubes_array;
};