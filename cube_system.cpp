#include "cube_system.hpp"

Cube_System::Cube_System() :
	grid_manager(nullptr),
border_cubes({}),
number_of_threads(4)
{
}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
	
	border_cubes.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	cubes_model_data.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES + EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
}

std::size_t Cube_System::add_grid_coordinates_to_queue(std::pair<int, int> start_end_index_pair) {
	ZoneScoped;

	size_t number_elements_enqueued = 0;
	int start_index = start_end_index_pair.first;
	int end_index = start_end_index_pair.second;
	for (int chunk_index = start_index; chunk_index < end_index; chunk_index++) {
		Chunk& chunk = grid_manager->grid->chunks[chunk_index];
		for (int i = 0; i < Chunk::rows * Chunk::columns; i++) {
			int r = i / Chunk::rows;
			int c = i % Chunk::columns;
			if (chunk.cells_data[i]) {
				std::pair<int, int> value = std::make_pair(r + chunk.chunk_origin_row, c + chunk.chunk_origin_column);
					
				grid_coordinates_queue.enqueue(value);
				number_elements_enqueued++;
			}
		}
	}
	return number_elements_enqueued;
}


size_t Cube_System::add_all_coordinates_of_alive_grid_cells_to_queue() {
	auto& chunks = grid_manager->grid->chunks;
	// clear this before we do anything else! dont want old coordinates showing up.
	grid_coordinates_queue = moodycamel::ConcurrentQueue<std::pair<int, int>>();

	size_t chunks_per_thread = grid_manager->grid->number_of_chunks / number_of_threads;
	std::vector<std::pair<int, int>> chunk_block_start_end_indices = get_work_group_start_end_indices_pairs(chunks_per_thread, chunks.size());

	std::vector<std::future < size_t> > working_thread_results;
	for (auto& index_pair: chunk_block_start_end_indices) {
		working_thread_results.emplace_back(std::async(std::launch::async, &Cube_System::add_grid_coordinates_to_queue, this, index_pair));
	}
		
	for (auto& r: working_thread_results) {
		r.wait();
	}

	size_t total_number_of_elements_enqueued = 0;
	for (auto& r: working_thread_results) {
		total_number_of_elements_enqueued += r.get();
	}

	return total_number_of_elements_enqueued;
}


void Cube_System::create_cubes_for_all_coordinates_in_queue(size_t number_of_coords) {
	size_t elements_per_thread = number_of_coords / number_of_threads;
	std::vector<std::future<void>> working_group_threads;
	for (int i = 0; i < number_of_threads; i++) {
		working_group_threads.emplace_back(std::async(std::launch::async,
		                                              &Cube_System::create_cubes_from_coordinates, this, elements_per_thread));
	}
		
	for (auto& r: working_group_threads) {
		r.wait();
	}
}

void Cube_System::create_cubes_for_alive_grid_cells() {
	ZoneScoped;

	size_t number_of_coords = add_all_coordinates_of_alive_grid_cells_to_queue();
	
	create_cubes_for_all_coordinates_in_queue(number_of_coords);
}

void Cube_System::update_model_matrix_queue_partially(size_t work_items) {
	ZoneScoped;

	size_t dequeued_items = 0;

	while (dequeued_items < work_items) {
		Cube cube;
		bool success = cubes_queue.try_dequeue(cube);
		if (success) {
			dequeued_items++;

			const glm::mat4 model_matrix = cube.compute_model_matrix_no_rotation();
			model_matrix_queue.enqueue(model_matrix);
		}
	}
}

void Cube_System::update_model_matrix_queue() {
	ZoneScoped;
	size_t number_of_cubes = cubes_queue.size_approx();

	size_t work_items_per_thread = number_of_cubes / number_of_threads;

	std::vector<std::future<void>> work_threads;
	for (int i = 0; i < number_of_threads; i++) {
		work_threads.emplace_back(std::async(std::launch::async,
		                                     &Cube_System::update_model_matrix_queue_partially, this, work_items_per_thread));
	}
}

void Cube_System::create_cubes_from_coordinates(size_t number_of_needed_cubes) {
	ZoneScoped;

	size_t number_of_created_cubes = 0;
	while (number_of_created_cubes < number_of_needed_cubes) {
		std::pair<int, int> coord_pair;
		bool success = grid_coordinates_queue.try_dequeue(coord_pair);
		if (success) {
			number_of_created_cubes++;

			float x = static_cast<float>(coord_pair.first);
			float y = static_cast<float>(coord_pair.second);
			Cube cube = Cube(glm::vec3(y, -x, -3.0f), 0.0f);
			cubes_queue.enqueue(cube);
		}
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
		border_cubes.emplace_back(glm::vec3((float) y, (float) -x, -3.0f), 50.0f);
	}
}


void Cube_System::update() {
	ZoneScoped;
	
	bool has_to_update_cubes_model_data = false;
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		create_cubes_for_alive_grid_cells();
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
			ZoneScopedN("update cubes_model_data vector");
			update_model_matrix_queue();

			cubes_model_data.clear();
			size_t queue_size = model_matrix_queue.size_approx();
			size_t dequeued_matrices = 0;
			while (dequeued_matrices < queue_size) {
				glm::mat4 matrix;
				bool success = model_matrix_queue.try_dequeue(matrix);
				if (success) {
					dequeued_matrices++;
					cubes_model_data.push_back(matrix);
				}
			}
			
			
			{
				ZoneScopedN("compute mvp data for border cubes");
				for (const Cube& cube: border_cubes) {
					const glm::mat4 model_matrix = cube.compute_model_matrix_with_rotation();
					cubes_model_data.push_back(model_matrix);
				}
			}
		}
	}
}


std::vector<std::pair<int, int>> get_work_group_start_end_indices_pairs(size_t desired_work_group_size, size_t total_number_of_elements) {
	// create work groups the threads will work on

	// for 2385 blocks we expect to create the pairs (0, 499), (500, 999), (1000, 1499), (1500, 1999), (2000, 2384).
	std::vector<std::pair<int, int>> work_group_indices_pairs;
	int block_size = static_cast<int>(desired_work_group_size);
	int number_of_blocks = static_cast<int>(total_number_of_elements / desired_work_group_size);
	int remaining_blocks = static_cast<int>(total_number_of_elements % desired_work_group_size);

	for (int i = 0; i < number_of_blocks; i++) {
		work_group_indices_pairs.emplace_back(i*block_size, (i + 1)*block_size);
	}
	// add last block
	work_group_indices_pairs.emplace_back((number_of_blocks + 1) * block_size, (number_of_blocks + 1) * block_size + remaining_blocks);

	return work_group_indices_pairs;
}