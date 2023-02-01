#pragma once

#include "state_render_data.hpp"
#include "Tracy.hpp"

void Cube_System::create_grid_cubes_from_coordinates(std::unordered_set<Coordinate> coordinates) {
	ZoneScoped;

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

void Cube_System::create_border_cubes_from_coordinates(std::vector<Coordinate> coordinates) {
	ZoneScoped;


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


void Cube_System::update(Grid_Manager* grid_manager) {
	ZoneScoped;

	clear_border_and_grid_cubes_array();
	create_grid_cubes_from_coordinates(grid_manager->world_coordinates);
	create_border_cubes_from_coordinates(grid_manager->border_coordinates);
}

void Cube_System::clear_border_and_grid_cubes_array() {
	ZoneScoped;
	current_number_of_grid_cubes = 0;
	current_number_of_border_cubes = 0;
}

Cube_System::Cube_System() : current_number_of_grid_cubes(0), current_number_of_border_cubes(0) {
	ZoneScoped;
};

Cube* Cube_System::create_new_cube() {
	ZoneScoped;
	if (current_number_of_grid_cubes < MAX_NUMBER_OF_GRID_CUBES - 1) {
		current_number_of_grid_cubes++;
		return &grid_cubes[current_number_of_grid_cubes];
	} else {
		return nullptr;
	}
}