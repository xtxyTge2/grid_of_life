#pragma once

#include "Tracy.hpp"

#include "world.hpp"


class Cube_System {
public:
	Cube_System();

	void initialise(std::shared_ptr<Grid_Manager> manager);

	void update();

	void create_border_cubes_for_grid();

	void create_grid_cubes_for_grid();
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	int current_number_of_grid_cubes;
	constexpr static size_t MAX_NUMBER_OF_GRID_CUBES = 500000;
	std::array<Cube, MAX_NUMBER_OF_GRID_CUBES> grid_cubes;

	int current_number_of_border_cubes;
	constexpr static size_t MAX_NUMBER_OF_BORDER_CUBES = 500000;
	std::array<Cube, MAX_NUMBER_OF_BORDER_CUBES> border_cubes;

	std::vector<glm::mat4> cubes_model_data;
};