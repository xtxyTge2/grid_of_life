#pragma once

#include "cube_system.hpp"
#include "Tracy.hpp"

Cube_System::Cube_System() : 
	grid_manager(nullptr), 
	current_number_of_grid_cubes(0), 
	current_number_of_border_cubes(0) 
{
	ZoneScoped;
}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
}

void Cube_System::create_grid_cubes_for_grid() {
	ZoneScoped;

	std::unordered_set<Coordinate> coordinates = grid_manager->grid->grid_coordinates;

	if (coordinates.size() > MAX_NUMBER_OF_GRID_CUBES) {
		std::cout << "Error. Cant create more than " << MAX_NUMBER_OF_GRID_CUBES << " grid cubes. Tried to create " << coordinates.size() << " grid cubes.\n";
		return;
	}

	current_number_of_grid_cubes = 0;
	for(auto& [x, y]: coordinates) {
		Cube& current_cube = grid_cubes[current_number_of_grid_cubes];
		current_number_of_grid_cubes++;
		// note the switch in y and x coordinates here!
		current_cube.m_position = glm::vec3((float) y, (float) -x, -3.0f);
		current_cube.m_angle = 0.0f;
		// TODO reset cube here.
	}
}

void Cube_System::create_border_cubes_for_grid() {
	ZoneScoped;
	std::unordered_set<Coordinate> coordinates = grid_manager->grid->border_coordinates;
	if (coordinates.size() > MAX_NUMBER_OF_BORDER_CUBES) {
		std::cout << "Error. Cant create more than " << MAX_NUMBER_OF_BORDER_CUBES << " border cubes. Tried to create " << coordinates.size() << " border cubes.\n";
		return;
	}

	current_number_of_border_cubes = 0;
	for (auto& [x, y]: coordinates) {
		Cube& current_cube = border_cubes[current_number_of_border_cubes];
		current_number_of_border_cubes++;
		// note the switch in y and x coordinates here!
		current_cube.m_position = glm::vec3((float) y, (float) -x, -3.0f);
		current_cube.m_angle = 50.0f;
	}
}


void Cube_System::update() {
	ZoneScoped;
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		current_number_of_grid_cubes = 0;
		create_grid_cubes_for_grid();
	} 
	if(grid_manager->grid_execution_state.updated_border_coordinates) {
		current_number_of_border_cubes = 0;
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
		}
	}
}