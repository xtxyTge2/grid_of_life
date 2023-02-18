#include "cube_system.hpp"
#include <boost/unordered/unordered_flat_map.hpp>

Cube_System::Cube_System() :
	t1_done(false),
	total_number_of_elements_enqueued(0),
	grid_manager(nullptr),
	grid_cubes({}),
border_cubes({})
{
}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
	/*
	grid_cubes.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	border_cubes.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	cubes_model_data.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES + EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
		*/
}


void Cube_System::compute_coordinates_of_alive_grid_cells() {
	ZoneScoped;
	size_t internal_number_elements_enqueued = 0;
	
	concurrency::concurrent_vector<size_t> number_of_alive_cells_vector;
	concurrency::parallel_for_each(
		std::begin(grid_manager->grid->chunk_map),
		std::end(grid_manager->grid->chunk_map),
		[&number_of_alive_cells_vector, this](auto&& it) {
			Chunk& chunk = it.second;
			size_t number_of_alive_cells_current_chunk = 0;

			for (int i = 0; i < Chunk::rows * Chunk::columns; i++) {
				int r = i / Chunk::rows;
				int c = i % Chunk::columns;
				if (chunk.cells_data[i]) {
					std::pair<int, int> value = std::make_pair(r + chunk.chunk_origin_row, c + chunk.chunk_origin_column);
					
					grid_coordinates.enqueue(value);
					number_of_alive_cells_current_chunk++;
				}
			}
			number_of_alive_cells_vector.push_back(number_of_alive_cells_current_chunk);
		}
	);
	

	// calculate how many elements in total we have enqueued, store this in an internal variable first. Saving us atomic updates atm.
	for (size_t value: number_of_alive_cells_vector) {
		internal_number_elements_enqueued += value;
	}


	// ORDER MATTERS HERE!
	// save some atomic writes by only updating it once at the point where we actually need it.
	total_number_of_elements_enqueued = internal_number_elements_enqueued;
	// first update the number of elements enqueued, then set t1_done to true. This ensures that the second thread gets the correct number of elements enqueued, since he only reads it after t1_done is true.
	// never forget to set thread to done here!
	t1_done = true;

}

void Cube_System::create_cubes_from_coordinates() {
	ZoneScoped;
	size_t number_elements_dequeued = 0;

	// run until thread t1 is done
	while (!t1_done) {
		std::pair<int, int> coord_pair;
		bool success = grid_coordinates.try_dequeue(coord_pair);
			
		if (success) {
			number_elements_dequeued++;
			float x = (float) coord_pair.first;
			float y = (float) coord_pair.second;
			grid_cubes.emplace_back(glm::vec3(y, -x, -3.0f), 0.0f);
		}
	}
	// only get here once thread t1 is done. By this point the atomic variable total_number_of_elements_enqueued is set correctly by thread 1. (it gets updated before t1_done is set to true, so no possible race condition between atomic variables updating.)
	while (number_elements_dequeued < total_number_of_elements_enqueued) {
		std::pair<int, int> coord_pair; 
		bool success = grid_coordinates.try_dequeue(coord_pair);
			
		if (success) {
			number_elements_dequeued++;
			float x = (float) coord_pair.first;
			float y = (float) coord_pair.second;
			grid_cubes.emplace_back(glm::vec3(y, -x, -3.0f), 0.0f);
		}
	}
}

void Cube_System::create_grid_cubes_for_grid() {
	ZoneScoped;

	// clear this before we do anything else! dont want old coordinates showing up.
	grid_coordinates = moodycamel::ConcurrentQueue<std::pair<int, int>>();
	total_number_of_elements_enqueued = 0;
	t1_done = false;
	std::thread t1(&Cube_System::compute_coordinates_of_alive_grid_cells, this);
	std::thread t2(&Cube_System::create_cubes_from_coordinates, this);
	
	t1.join();
	t2.join();
}

void Cube_System::create_border_cubes_for_grid() {
	ZoneScoped;

	std::vector<std::pair<int, int>> coordinates;
	for (auto& [chunk_coord, chunk]: grid_manager->grid->chunk_map) {
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
		border_cubes.emplace_back(glm::vec3((float) y, (float) -x, -3.0f), 50.0f);
	}
}


void Cube_System::update() {
	ZoneScoped;
	
	bool has_to_update_cubes_model_data = false;
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		grid_cubes.clear();
		create_grid_cubes_for_grid();
		has_to_update_cubes_model_data = true;
	}
	if (grid_manager->grid_execution_state.updated_border_coordinates) {
		border_cubes.clear();
		has_to_update_cubes_model_data = true;
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
		}
	}

	if (has_to_update_cubes_model_data) {
		/*
		concurrency::concurrent_vector<glm::mat4> cubes_model_data_concurrent_vector;
		cubes_model_data_concurrent_vector.reserve(current_number_of_grid_cubes + current_number_of_border_cubes);
		concurrency::parallel_for(
			std::begin(
		);
		*/
		{
			ZoneScopedN("compute mvp data for all cubes");

			cubes_model_data.clear();
			for (const Cube& cube: grid_cubes) {
				const glm::mat4 model_matrix = cube.compute_model_matrix_no_rotation();
				cubes_model_data.push_back(model_matrix);
			}
			for (const Cube& cube: border_cubes) {
				const glm::mat4 model_matrix = cube.compute_model_matrix_with_rotation();
				cubes_model_data.push_back(model_matrix);
			}
		}
	}
}