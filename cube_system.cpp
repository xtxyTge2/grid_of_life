#include "cube_system.hpp"

Cube_System::Cube_System() : 
	grid_manager(nullptr), 
	current_number_of_grid_cubes(0), 
	current_number_of_border_cubes(0) 
{

}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
	cubes_model_data.reserve(MAX_NUMBER_OF_BORDER_CUBES + MAX_NUMBER_OF_BORDER_CUBES);
}


void Cube_System::create_grid_cubes_for_grid() {
	ZoneScoped;

	if (false) {
		std::vector<std::pair<int, int>> coordinates = grid_manager->grid->grid_coordinates;
	
		if (coordinates.size() > MAX_NUMBER_OF_GRID_CUBES) {
			std::cout << "Error. Cant create more than " << MAX_NUMBER_OF_GRID_CUBES << " grid cubes. Tried to create " << coordinates.size() << " grid cubes.\n";
			return;
		}

		current_number_of_grid_cubes = 0;
		for (auto& [x, y]: coordinates) {
			Cube& current_cube = grid_cubes[current_number_of_grid_cubes];
			current_number_of_grid_cubes++;
			// note the switch in y and x coordinates here!
			current_cube.m_position = glm::vec3((float) y, (float) -x, -3.0f);
			current_cube.m_angle = 0.0f;
			// TODO reset cube here.
		}
	} else {
		moodycamel::ConcurrentQueue < std::pair<int, int>>&  grid_coordinates_concurrent = grid_manager->grid->grid_coordinates_concurrent;
		size_t number_elements_dequeued = 0;
		while (number_elements_dequeued < grid_manager->grid->number_of_elements_enqueued) {
			std::pair<int, int> coord_pair; 
			bool success_dequeue = grid_coordinates_concurrent.try_dequeue(coord_pair);
			if (success_dequeue) {
				int x = coord_pair.first;
				int y = coord_pair.second;
	
				Cube& current_cube = grid_cubes[current_number_of_grid_cubes];
				current_number_of_grid_cubes++;
				// note the switch in y and x coordinates here!
				current_cube.m_position = glm::vec3((float) y, (float) -x, -3.0f);
				current_cube.m_angle = 0.0f;
				number_elements_dequeued++;
			}
		}
	}
}

void Cube_System::create_border_cubes_for_grid() {
	ZoneScoped;
	std::vector<std::pair<int, int>> coordinates = grid_manager->grid->border_coordinates;
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
	
	bool has_to_update_cubes_model_data = false;
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		current_number_of_grid_cubes = 0;
		create_grid_cubes_for_grid();
		has_to_update_cubes_model_data = true;
	} 
	if(grid_manager->grid_execution_state.updated_border_coordinates) {
		current_number_of_border_cubes = 0;
		has_to_update_cubes_model_data = true;
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
		}
	}

	if (has_to_update_cubes_model_data) {
		cubes_model_data.clear();
		for (int i = 0; i < current_number_of_grid_cubes; i++) {
			const Cube& current_cube = grid_cubes[i];
			const glm::mat4 model_matrix = current_cube.compute_model_matrix_no_rotation();
			cubes_model_data.push_back(model_matrix);
		}
		for (int i = 0; i < current_number_of_border_cubes; i++) {
			const Cube& current_cube = border_cubes[i];
			const glm::mat4 model_matrix = current_cube.compute_model_matrix_with_rotation();
			cubes_model_data.push_back(model_matrix);
		}
	}
}