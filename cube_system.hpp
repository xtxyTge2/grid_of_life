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

	void create_border_cubes_for_grid();

	void create_cubes_for_alive_grid_cells();

	void create_cubes_for_all_coordinates_in_queue(size_t number_of_coords);

	size_t add_all_coordinates_of_alive_grid_cells_to_queue();

	void create_cubes_for_all_coordinates_in_queue();

	void create_cubes_from_coordinates(size_t number_of_needed_cubes);

	void update_model_matrix_queue();

	void update_model_matrix_queue_partially(size_t work_items);

	std::size_t add_grid_coordinates_to_queue(std::pair<int, int> start_end_index_pair);
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	
	constexpr static size_t EXPECTED_MAX_NUMBER_OF_GRID_CUBES = 500000;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_BORDER_CUBES = 500000;
	std::vector<Cube> border_cubes;


	moodycamel::ConcurrentQueue<std::pair<int, int>> grid_coordinates_queue;
	
	moodycamel::ConcurrentQueue<Cube> cubes_queue;

	moodycamel::ConcurrentQueue<glm::mat4> model_matrix_queue;

	std::vector<glm::mat4> cubes_model_data;


	size_t number_of_threads;
};