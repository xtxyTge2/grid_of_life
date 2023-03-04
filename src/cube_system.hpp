#pragma once

#include <tracy/Tracy.hpp>

#include <glm/glm.hpp>
#include "grid.hpp"


std::vector<std::pair<int, int>> get_work_group_start_end_indices_pairs(size_t desired_work_group_size, size_t total_number_of_elements);


class Cube_System {
public:
	Cube_System();

	void initialise(std::shared_ptr<Grid_Manager> manager);

	void update();

	void create_border_cubes_for_grid();

	void update_model_translations_data();
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	constexpr static size_t MAX_NUMBER_OF_CUBES = 1000000;

	std::array<glm::vec3, MAX_NUMBER_OF_CUBES> cubes_translation_data;

	std::size_t number_of_translation_data;
};