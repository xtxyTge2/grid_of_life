#pragma once

#include "Tracy.hpp"

#include "world.hpp"
#include <future>
#include <concurrent_queue.h>

std::vector<std::pair<int, int>> get_work_group_start_end_indices_pairs(size_t desired_work_group_size, size_t total_number_of_elements);


class Cube_System {
public:
	Cube_System();

	void initialise(std::shared_ptr<Grid_Manager> manager);

	void update();

	void model_queue_producer(std::pair<std::size_t, std::size_t> start_end_index);

	void model_queue_busy_wait_consumer(std::stop_source stop_source);

	void create_border_cubes_for_grid();

	void update_model_matrix_data();
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_GRID_CUBES = 500000;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_BORDER_CUBES = 500000;
	std::vector<Cube> border_cubes;

	moodycamel::ConcurrentQueue<glm::mat4> model_matrix_queue;

	std::vector<glm::mat4> cubes_model_data;
};