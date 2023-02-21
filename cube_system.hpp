#pragma once

#include "Tracy.hpp"

#include <glm/glm.hpp>
#include "grid.hpp"


std::vector<std::pair<int, int>> get_work_group_start_end_indices_pairs(size_t desired_work_group_size, size_t total_number_of_elements);


class Cube_System {
public:
	Cube_System();

	void initialise(std::shared_ptr<Grid_Manager> manager);

	void update();

	void model_translation_producer(std::pair<std::size_t, std::size_t> start_end_index);

	void model_queue_busy_wait_consumer(std::stop_source stop_source);

	void create_border_cubes_for_grid();

	void update_model_translations_data();
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_GRID_CUBES = 500000;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_BORDER_CUBES = 500000;

	moodycamel::ConcurrentQueue<glm::vec3> model_translations_queue;

	std::vector<glm::vec3> cubes_model_data;
};