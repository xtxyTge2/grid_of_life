#include "cube_system.hpp"

Cube_System::Cube_System() :
	grid_manager(nullptr)
{
}

void Cube_System::initialise(std::shared_ptr<Grid_Manager> manager) {
	grid_manager = manager;
	
	cubes_model_data.reserve(EXPECTED_MAX_NUMBER_OF_BORDER_CUBES + EXPECTED_MAX_NUMBER_OF_BORDER_CUBES);
	model_translations_queue = moodycamel::ConcurrentQueue < glm::vec3 > (cubes_model_data.capacity());
}


void Cube_System::model_translation_producer(std::pair<std::size_t, std::size_t> start_end_index) {
	ZoneScoped;
	
	std::array<std::pair<int, int>, Chunk::rows * Chunk::columns> local_data;
	std::size_t start_index = start_end_index.first;
	std::size_t end_index = start_end_index.second;
	
	for (std::size_t idx = start_index; idx < end_index; ++idx) {
		unsigned int count = 0;
		
		Chunk& chunk = grid_manager->grid->chunks[idx];
		for (int r = 0; r < Chunk::rows; r++) {
			for (int c = 0; c < Chunk::columns; c++) {
				if (chunk.cells_data[r * Chunk::rows + c]) {
					int x = c + chunk.chunk_origin_column;
					int y = -(r + chunk.chunk_origin_row);
					local_data[count++] = std::make_pair(x, y);
				}
			}
		}
		
		std::array<glm::vec3, Chunk::rows * Chunk::columns> translation_data;
		for (std::size_t i = 0; i < count; i++) {
			std::pair<int, int> xy_position = local_data[i];
			float x = static_cast<float>(xy_position.first);
			float y = static_cast<float>(xy_position.second);
			translation_data[i] = glm::vec3(x, y, -3.0f);
		}
		model_translations_queue.enqueue_bulk(translation_data.begin(), count);
	}
}

void Cube_System::model_queue_busy_wait_consumer(std::stop_source stop_source) {
	ZoneScoped;

	std::stop_token stoken = stop_source.get_token();

	constexpr int local_array_size = 64 * Chunk::rows * Chunk::columns;
	std::array<glm::vec3, local_array_size> local_matrix_data;
	while (!stoken.stop_requested()) {
		std::size_t number_dequeued = model_translations_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
		if (number_dequeued > 0) {
			cubes_model_data.insert(std::end(cubes_model_data), std::begin(local_matrix_data), std::begin(local_matrix_data) + number_dequeued);
		}
	}
	
	std::size_t number_dequeued = model_translations_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
	while (number_dequeued > 0) {
		cubes_model_data.insert(std::end(cubes_model_data), std::begin(local_matrix_data), std::begin(local_matrix_data) + number_dequeued);
		number_dequeued = model_translations_queue.try_dequeue_bulk(local_matrix_data.begin(), local_matrix_data.size());
	}
}


void Cube_System::update_model_translations_data() {
	ZoneScoped;

	assert(model_translations_queue.size_approx() == 0);

	std::vector<std::pair<std::size_t, std::size_t>> chunks_partition = grid_manager->grid->get_partition_data_for_chunks(1, false);
		
	std::stop_source consumer_thread_stop_source;

	// schedule the single consumer thread before all the producers, so it hopefully wont get scheduled at the very last
	std::jthread consumer_thread = std::jthread(&Cube_System::model_queue_busy_wait_consumer, this, consumer_thread_stop_source);

	std::vector<std::jthread> producers;
	for (int i = 0; i < chunks_partition.size(); i++) {
		producers.emplace_back(&Cube_System::model_translation_producer, this, chunks_partition[i]);
	}

	for (auto& t: producers) {
		t.join();
	}
	// once all producer threads finish we tell wait consumer thread to break out from its busy wait loop. At that point we know that nothing is being added to the queue and the consumer can safely process the remaining elements.
	consumer_thread_stop_source.request_stop();
	consumer_thread.join();

	assert(model_translations_queue.size_approx() == 0);
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
		cubes_model_data.push_back(glm::vec3((float) y, (float) -x, -3.0f));
	}
}


void Cube_System::update() {
	ZoneScoped;
	
	if (grid_manager->grid_execution_state.updated_grid_coordinates) {
		cubes_model_data.clear();
		update_model_translations_data();
	}
	if (grid_manager->grid_execution_state.updated_border_coordinates) {
		if (grid_manager->grid_execution_state.show_chunk_borders) {
			create_border_cubes_for_grid();
		}
	}
}