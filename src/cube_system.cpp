#include "cube_system.hpp"

Cube_System::Cube_System() :
	grid_manager(nullptr),
	number_of_translation_data(0)
{

}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
}


void Cube_System::update_model_translations_data() {
	ZoneScoped;

	number_of_translation_data = 0;
	for (Chunk& chunk: grid_manager->grid->chunks) {
		for (std::size_t i = 0; i < chunk.number_of_alive_cells; ++i) {
			std::pair<int, int> xy_position = chunk.coordinates_of_alive_cells[i];
			float x = static_cast<float>(xy_position.first);
			float y = static_cast<float>(xy_position.second);
			cubes_translation_data[number_of_translation_data + i] = glm::vec3(x, y, -3.0f);
		}
		number_of_translation_data += chunk.number_of_alive_cells;
	}
}

void Cube_System::create_border_cubes_for_grid() {
	ZoneScoped;

	std::vector<std::pair<int, int>> coordinates;
	for (Chunk& chunk: grid_manager->grid->chunks) {
		for (int r = 0; r < Chunk::rows; r++) {
			coordinates.push_back(std::make_pair(chunk.chunk_origin_row + r, chunk.chunk_origin_column - 1));
			coordinates.push_back(std::make_pair(chunk.chunk_origin_row + r, chunk.chunk_origin_column + Chunk::columns));
		}
		for (int c = 0; c < Chunk::columns; c++) {
			coordinates.push_back(std::make_pair(chunk.chunk_origin_row -1, chunk.chunk_origin_column + c));
			coordinates.push_back(std::make_pair(chunk.chunk_origin_row + Chunk::rows, chunk.chunk_origin_column + c));
		}
	}
	for (auto& [x, y]: coordinates) {
		cubes_translation_data[number_of_translation_data++] = glm::vec3(static_cast<float>(y), static_cast<float>(-x), -3.0f);
	}
	
}


void Cube_System::update() {
	ZoneScoped;
	
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		update_model_translations_data();
	}
	if (grid_manager->grid_execution_state.updated_border_coordinates) {
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
		}
	}
}