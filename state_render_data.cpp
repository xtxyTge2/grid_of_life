#pragma once

#include "state_render_data.hpp"
#include "Tracy.hpp"


void Cube_System::update(Grid_Manager* grid_manager) {
	ZoneScoped;
	// make sure that grid_manager was updated before this, otherwise coordinates might be outdated.
	std::vector<std::pair<int, int>> world_coordinates = grid_manager->world_coordinates;
	std::vector<std::pair<int, int>> border_coordinates = grid_manager->border_coordinates;

	size_t wanted_number_of_cubes = world_coordinates.size();
	if (grid_manager->grid_execution_state.show_chunk_borders) {
		wanted_number_of_cubes += border_coordinates.size();
	}

	if (wanted_number_of_cubes > MAX_NUMBER_OF_CUBES) {
		std::cout << "Error. Cant create more than " << MAX_NUMBER_OF_CUBES << " cubes. Tried to create " << wanted_number_of_cubes << " cubes.\n";
		return;
	}

	clear();
	for(auto& [x, y]: world_coordinates) {
		Cube* current_cube = create_new_cube();
		// note the switch in y and x coordinates here!
		current_cube->m_position = glm::vec3((float) y, (float) -x, -3.0f);
		current_cube->m_angle = 0.0f;
		// TODO reset cube here.
	}

	if (grid_manager->grid_execution_state.show_chunk_borders) {
		for (auto& [x, y]: border_coordinates) {
			Cube* current_cube = create_new_cube();
			// note the switch in y and x coordinates here!
			current_cube->m_position = glm::vec3((float) y, (float) -x, -3.0f);
			current_cube->m_angle = 50.0f;	
		}
	}

}

void Cube_System::clear() {
	ZoneScoped;
	current_number_of_cubes = 0;
}

Cube_System::Cube_System() : current_number_of_cubes(0) {
	ZoneScoped;
};

Cube* Cube_System::create_new_cube() {
	ZoneScoped;
	if (current_number_of_cubes < MAX_NUMBER_OF_CUBES - 1) {
		current_number_of_cubes++;
		return &cubes_array[current_number_of_cubes];
	} else {
		return nullptr;
	}
}