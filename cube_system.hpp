#pragma once

#include "Tracy.hpp"

#include "world.hpp"
#include <concurrent_queue.h>

class Cube_System {
public:
	Cube_System();

	void initialise(std::shared_ptr<Grid_Manager> manager);

	void update();

	void create_border_cubes_for_grid();

	void create_grid_cubes_for_grid();

	void compute_coordinates_of_alive_grid_cells();

	void create_cubes_from_coordinates();
	//--------------------------------------------------------------------------------
	// data
	std::shared_ptr<Grid_Manager> grid_manager;

	
	constexpr static size_t EXPECTED_MAX_NUMBER_OF_GRID_CUBES = 500000;
	std::vector<Cube> grid_cubes;

	constexpr static size_t EXPECTED_MAX_NUMBER_OF_BORDER_CUBES = 500000;
	std::vector<Cube> border_cubes;

	std::vector<glm::mat4> cubes_model_data;

	moodycamel::ConcurrentQueue<std::pair<int, int>> grid_coordinates;

	std::atomic<bool> t1_done;
	std::atomic<size_t> total_number_of_elements_enqueued;
};