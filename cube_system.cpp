#include "cube_system.hpp"

Cube_System::Cube_System() :
	grid_manager(nullptr),
border_cubes({})
{
}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
	
	border_cubes.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	cubes_model_data.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES + EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	model_matrix_queue = moodycamel::ConcurrentQueue < glm::mat4 > (cubes_model_data.capacity());
}


void Cube_System::model_queue_producer(std::pair<std::size_t, std::size_t> start_end_index) {
	ZoneScoped;

	std::array<glm::mat4, Chunk::rows * Chunk::columns> local_data;

	std::size_t start_index = start_end_index.first;
	std::size_t end_index = start_end_index.second;
	
	for (std::size_t idx = start_index; idx < end_index; ++idx) {
		Chunk& chunk = grid_manager->grid->chunks[idx];
		std::size_t count = 0;
		for (int i = 0; i < Chunk::rows * Chunk::columns; i++) {
			if (chunk.cells_data[i]) {
				int r = i / Chunk::rows;
				int c = i % Chunk::columns;
				
				r += chunk.chunk_origin_row;
				c += chunk.chunk_origin_column;

				float x = static_cast<float>(r);
				float y = static_cast<float>(c);
				glm::vec3 cube_position = glm::vec3(y, -x, -3.0f);

				glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), cube_position);
				local_data[count++] = model_matrix;
			}
		}
		// std::make_move_iterator?
		model_matrix_queue.enqueue_bulk(local_data.begin(), count);
	}
}

void Cube_System::model_queue_busy_wait_consumer(std::stop_source stop_source) {
	ZoneScoped;

	std::stop_token stoken = stop_source.get_token();

	constexpr int local_array_size = 4 * Chunk::rows * Chunk::columns;
	std::array<glm::mat4, local_array_size> local_matrix_data;
	while (!stoken.stop_requested()) {
		std::size_t number_dequeued = model_matrix_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
		if (number_dequeued > 0) {
			cubes_model_data.insert(std::end(cubes_model_data), std::begin(local_matrix_data), std::begin(local_matrix_data) + number_dequeued);
		}
	}
	
	std::size_t number_dequeued = model_matrix_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
	while (number_dequeued > 0) {
		cubes_model_data.insert(std::end(cubes_model_data), std::begin(local_matrix_data), std::begin(local_matrix_data) + number_dequeued);
		number_dequeued = model_matrix_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
	}
}


void Cube_System::update_model_matrix_data() {
	ZoneScoped;

	
	std::vector<std::pair<std::size_t, std::size_t>> chunks_partition = grid_manager->grid->get_partition_data_for_chunks(2, false);
		
	std::stop_source consumer_thread_stop_source;

	// schedule the single consumer thread before all the producers, so it hopefully wont get scheduled at the very last
	std::jthread consumer_thread = std::jthread(&Cube_System::model_queue_busy_wait_consumer, this, consumer_thread_stop_source);

	std::vector<std::jthread> producers;
	for (int i = 0; i < chunks_partition.size(); i++) {
		producers.emplace_back(&Cube_System::model_queue_producer, this, chunks_partition[i]);
	}

	for (auto& t: producers) {
		t.join();
	}
	// once all producer threads finish we tell wait consumer thread to break out from its busy wait loop. At that point we know that nothing is being added to the queue and the consumer can safely process the remaining elements.
	consumer_thread_stop_source.request_stop();
	
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
	
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		cubes_model_data.clear();
		update_model_matrix_data();
	}
	if (grid_manager->grid_execution_state.updated_border_coordinates) {
		border_cubes.clear();
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
			for (const Cube& cube: border_cubes) {
				const glm::mat4 model_matrix = cube.compute_model_matrix_with_rotation();
				cubes_model_data.push_back(model_matrix);
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