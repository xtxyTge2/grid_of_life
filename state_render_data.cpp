#pragma once

#include "state_render_data.hpp"
#include "Tracy.hpp"



void Cube_System::update(Grid_Manager* grid_manager) {
	// make sure that grid_manager was updated before this, otherwise coordinates might be outdated.
	std::vector<std::pair<int, int>> world_coordinates = grid_manager->world_coordinates;

	if (world_coordinates.size() > MAX_NUMBER_OF_CUBES) {
		std::cout << "Error. Cant create more than " << MAX_NUMBER_OF_CUBES << " cubes. Tried to create " << world_coordinates.size() << " cubes.\n";
		return;
	}

	clear();
	for (int i = 0; i < world_coordinates.size(); i++) {
		Cube* current_cube = create_new_cube();
		std::pair<int, int> coord = world_coordinates[i];
		int x = coord.first;
		int y = coord.second;
		// note the switch in y and x coordinates here!
		current_cube->m_position = glm::vec3((float) y, (float) -x, -3.0f);
		// TODO reset cube here.

		/*
		// do this to distinguish the border from the rest of the grid.
		if (is_border) {
			cube->m_angle = 50.0f;
		} else {
			cube->m_angle = 0.0f;
		}
		*/
	}

	grid_render_data = new Grid_Render_Data();
	
	
	for (int i = 0; i < current_number_of_cubes; i++) {
		Cube_Render_Data* cube_render_data = cubes_array[i].create_render_data();
		grid_render_data->cubes_render_data.push_back(*cube_render_data);
	}

	// fill out grid info
	Grid* grid = grid_manager->grid;
	grid_render_data->grid_info = {};
	grid_render_data->grid_info.iteration = grid->iteration;
	grid_render_data->grid_info.rows = 0;
	grid_render_data->grid_info.columns = 0;
	grid_render_data->grid_info.origin_row = 0;
	grid_render_data->grid_info.origin_column = 0;
	grid_render_data->grid_info.number_of_alive_cells = grid->number_of_alive_cells;


}

void Cube_System::clear() {
	current_number_of_cubes = 0;
}


Cube_System::Cube_System() : current_number_of_cubes(0) {
	
};

Cube* Cube_System::create_new_cube() {
	if (current_number_of_cubes < MAX_NUMBER_OF_CUBES) {
		current_number_of_cubes++;
		return &cubes_array[current_number_of_cubes];
	} else {
		return nullptr;
	}
}