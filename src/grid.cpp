#include "grid.hpp"

Grid_Manager::Grid_Manager()
: grid_execution_state({})
{
	ZoneScoped;
	grid_info = std::make_shared < Grid_Info > ();
	opencl_context = std::make_shared < OpenCLContext > ();

	std::string open_cl_source_code_path = "opencl_grid.c";
	opencl_context->initialise(open_cl_source_code_path);

	create_new_grid();
};


void Grid_Manager::update_grid_info() {
	ZoneScoped;

	grid_info->iteration = static_cast<int>(grid->iteration);
	grid_info->number_of_chunks = static_cast<int>(grid->number_of_chunks);
}

//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;
	
	grid_execution_state = {};
	grid_execution_state.use_opencl_kernel = opencl_context->is_valid_context;
	
	grid = std::make_unique < Grid > (opencl_context);
	
}

void Grid_Manager::update_grid_execution_state(const Grid_UI_Controls_Info& ui_info) {
	ZoneScoped;

	switch (ui_info.button_type) {
		case GRID_NO_BUTTON_PRESSED:
			break;
		case GRID_RESET_BUTTON_PRESSED:
			grid.reset();
			create_new_grid();
			return;
			// if you remove the return somehow later, dont forget a break statement here :)
			// break;
		case GRID_NEXT_ITERATION_BUTTON_PRESSED:
			// only set run_manual_next_iteration if we are stopped
			if (!grid_execution_state.is_running) {
				grid_execution_state.run_manual_next_iteration = true;
			}
			break;
		case GRID_START_STOP_BUTTON_PRESSED:
			grid_execution_state.is_running = !grid_execution_state.is_running;
			grid_execution_state.time_since_last_iteration = 0.0f;
			break;
		default:
			break;
	}
	// check if the user changed to show/hide the grid borders.
	grid_execution_state.have_to_update_chunk_borders = grid_execution_state.show_chunk_borders != ui_info.show_chunk_borders;
	
	// set it to actual new value always.
	grid_execution_state.show_chunk_borders = ui_info.show_chunk_borders;
	grid_execution_state.grid_speed = ui_info.grid_speed_slider_value;
	grid_execution_state.should_run_at_max_possible_speed = ui_info.run_grid_at_max_possible_speed;
	grid_execution_state.number_of_iterations_per_single_frame = ui_info.number_of_grid_iterations_per_single_frame;
}

void Grid_Manager::update(double dt, const Grid_UI_Controls_Info& ui_info) {
	ZoneScoped;
	
	update_grid_execution_state(ui_info);
	
	grid_execution_state.updated_grid_coordinates = false;
	grid_execution_state.updated_border_coordinates = false;

	bool grid_changed = false;
	if (grid_execution_state.is_running) {
		if (grid_execution_state.should_run_at_max_possible_speed) {
			for (int i = 0; i < grid_execution_state.number_of_iterations_per_single_frame; i++) {
				grid->next_iteration();
			}
			grid_changed = true;
		} else {
			//assert(grid_execution_state.grid_speed > 0.0f);
			grid_execution_state.time_since_last_iteration += (float) dt;
			float threshold = 1.0f / grid_execution_state.grid_speed;
			if (grid_execution_state.time_since_last_iteration >= threshold) {
				grid->next_iteration();
				grid_changed = true;
				grid_execution_state.time_since_last_iteration = 0.0f;
			}
		}
	} else {
		if (grid_execution_state.run_manual_next_iteration) {
			grid_execution_state.run_manual_next_iteration = false;
			grid->next_iteration();
			grid_changed = true;
		}
	}

	// only update coordinates of alive grid cells if we are in the first iteration or if the grid changed.
	if (grid->iteration == 0 || grid_changed) {
		grid_execution_state.updated_grid_coordinates = true;
	}

	if (grid->iteration == 0 || (grid_execution_state.show_chunk_borders && grid_changed) || grid_execution_state.have_to_update_chunk_borders) {
		grid_execution_state.updated_border_coordinates = true;
	}
	// @Cleanup, factor this into a grid_info->update() call?
	update_grid_info();
}

//--------------------------------------------------------------------------------
Grid::Grid(std::shared_ptr<OpenCLContext> context) :
	iteration(0),
number_of_chunks(0),
chunk_map({}),
chunks({}),
opencl_context(context)
{
	ZoneScoped;

	constexpr static int EXPECTED_MAX_NUMBER_OF_CHUNKS = 1000;
	chunks_left_side_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	chunks_right_side_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	chunks_top_side_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	chunks_bottom_side_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	top_left_corner_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	top_right_corner_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	bottom_left_corner_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);
	bottom_right_corner_update_infos.reserve(EXPECTED_MAX_NUMBER_OF_CHUNKS);

	updated_chunks_id_queue = moodycamel::ConcurrentQueue < std::size_t > (EXPECTED_MAX_NUMBER_OF_CHUNKS);

	int base_row = (int) (Chunk::rows / 2);
	int base_column = (int) (Chunk::columns / 2);
	/*
	std::vector<std::pair<int, int>> initial_coordinates = { { 0, 1 }, { 0, 2 }, {0, 3} };
	create_new_chunk_and_set_alive_cells(0, 0, initial_coordinates);
	create_new_chunk_and_set_alive_cells(-1, 0, initial_coordinates);
	*/

	base_row = 1;
	base_column = 7;
	std::vector<std::pair<int, int>> initial_coordinates = {
		{ base_row, base_column },
		{ base_row + 1, base_column },
		{ base_row + 1, base_column - 1 },
		{ base_row + 1, base_column - 2 },
		{ base_row + 2, base_column - 1 },
		{ base_row, base_column - 4 },
		{ base_row, base_column - 5 },
		{ base_row, base_column - 6 },
		{ base_row + 1, base_column - 5 },
		{ base_row, base_column }
	};
	
	std::vector<Coordinate> chunk_coordinates;
	for (int r = -20; r < 20; r++) {
		for (int c = -20; c < 20; c++) {
			create_new_chunk_and_set_alive_cells(Coordinate(r, c), initial_coordinates);
		}
	}
	
	//create_new_chunk_and_set_alive_cells(Coordinate(0, 0), initial_coordinates);



	/*
	{
		int chunk_origin_test_row = -2;
		int chunk_origin_test_column = 3;
		//bottom left
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row, chunk_origin_test_column, { { 0, Chunk::columns - 2 }, { 0, Chunk::columns - 1 } });
		// top left
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row - 1, chunk_origin_test_column, { { Chunk::rows - 1, Chunk::columns - 1 } });
		// top right
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row-1, chunk_origin_test_column+1, { { Chunk::rows -1, 0 } });
		// bottom right
		create_new_chunk_and_set_alive_cells(chunk_origin_test_row, chunk_origin_test_column +1, { { 0, 0 }});
	}
	*/
}


void Grid::create_new_chunk_and_set_alive_cells(const Coordinate& coord, const std::vector<std::pair<int, int>>& coordinates) {
	ZoneScoped;

	const Coordinate& origin_coordinate = Coordinate(coord.x * Chunk::rows, coord.y * Chunk::columns);

	std::size_t chunk_index = chunks.size();
	chunks.emplace_back(coord, origin_coordinate, coordinates);

	chunk_map.insert(std::make_pair(coord, chunk_index));

	number_of_chunks++;
}

void Grid::create_new_chunk(const Coordinate& coord) {
	ZoneScoped;

	create_new_chunk_and_set_alive_cells(coord, {});
}

//--------------------------------------------------------------------------------
void Grid::update() {
	ZoneScoped;
}


void Grid::next_iteration() {
	ZoneScoped;

	if (chunks.size() == 0) {
		return;
	}
	{
		// sanity checks
		number_of_chunks = chunk_map.size();
		assert(number_of_chunks == chunks.size());

		assert(updated_chunks_id_queue.size_approx() == 0);
		std::size_t x;
		assert(updated_chunks_id_queue.try_dequeue(x) == false);
	}


	update_neighbour_count_and_set_info_of_all_chunks();

	for (Coordinate coord: coordinates_of_chunks_to_create) {
		create_new_chunk(coord);
	}

	update_neighbours_of_all_chunks();

	update_cells_of_all_chunks();
	
	remove_empty_chunks();

	iteration++;
	number_of_chunks = static_cast<int>(chunk_map.size());
	assert(chunk_map.size() == chunks.size());
}

std::vector<std::pair<std::size_t, std::size_t>> Grid::get_partition_data_for_chunks(unsigned int number_of_workers, bool allow_small_task_sizes) {
	ZoneScoped;

	constexpr static std::size_t MINIMUM_NUMBER_OF_CHUNKS_PER_THREAD = 500;
	
	std::size_t number_of_chunks = chunks.size();
	assert(chunks.size() == chunk_map.size());

	std::vector<std::pair<std::size_t, std::size_t>> partition;
	if (number_of_chunks <= MINIMUM_NUMBER_OF_CHUNKS_PER_THREAD || number_of_workers == 1) {
		partition = { { 0, number_of_chunks - 1 } };
	} else {
		std::size_t chunks_per_thread = number_of_chunks / number_of_workers;
		if (!allow_small_task_sizes && chunks_per_thread < MINIMUM_NUMBER_OF_CHUNKS_PER_THREAD) {
			// dont schedule any task with less than these amount of chunks per task if you dont allow small task sizes
			chunks_per_thread = MINIMUM_NUMBER_OF_CHUNKS_PER_THREAD;
		}

		std::size_t number_of_tasks = number_of_chunks / chunks_per_thread;
		
		std::size_t start_index = 0;
		std::size_t end_index = chunks_per_thread - 1;
		for (int i = 0; i < number_of_tasks; i++) {
			if (i == number_of_tasks - 1) {
				// the last worker gets the remaining chunks as well.
				end_index = chunks.size() - 1;
			}
			partition.push_back(std::make_pair(start_index, end_index));

			start_index += chunks_per_thread;
			end_index += chunks_per_thread;
		}
	}
	return partition;
}


void Grid::update_neighbour_count_and_set_info_of_all_chunks() {
	ZoneScoped;

	chunks_left_side_update_infos.clear();
	chunks_left_side_update_infos.shrink_to_fit();
	chunks_right_side_update_infos.clear();
	chunks_right_side_update_infos.shrink_to_fit();
	chunks_bottom_side_update_infos.clear();
	chunks_bottom_side_update_infos.shrink_to_fit();
	chunks_top_side_update_infos.clear();
	chunks_top_side_update_infos.shrink_to_fit();

	top_left_corner_update_infos.clear();
	top_left_corner_update_infos.shrink_to_fit();
	top_right_corner_update_infos.clear();
	top_right_corner_update_infos.shrink_to_fit();
	bottom_left_corner_update_infos.clear();
	bottom_left_corner_update_infos.shrink_to_fit();
	bottom_right_corner_update_infos.clear();
	bottom_right_corner_update_infos.shrink_to_fit();

	coordinates_of_chunks_to_create.clear();
	{	

		auto chunks_partition = get_partition_data_for_chunks(4, false);
		for (auto& it: chunks_partition) {
			std::jthread t(&Grid::update_neighbour_count_inside_for_chunk_index_range, this, it);
		}
	}
	for (std::size_t chunk_id = 0; chunk_id < chunks.size(); chunk_id++) {
		set_chunk_neighbour_info(chunk_id);
	}
}

void Grid::update_neighbour_count_inside_for_chunk_index_range(std::pair<std::size_t, std::size_t> start_end_index_pair) {
	std::size_t start_index = start_end_index_pair.first;
	std::size_t end_index = start_end_index_pair.second;

	for (std::size_t chunk_id = start_index; chunk_id < end_index; ++chunk_id) {
		chunks[chunk_id].update_neighbour_count_inside();
	}
}

void Grid::set_chunk_neighbour_info(std::size_t chunk_id) {
	ZoneScoped;
	Chunk& chunk = chunks[chunk_id];
	std::array<unsigned char, Chunk::rows*Chunk::columns>& cells_data = chunk.cells_data;

	// top side of chunk, so bottom side of neighbour chunk
	bool has_to_update_top = false;

	for (int c = 0; c < Chunk::columns; c++) {
		unsigned char value = cells_data[c];
		if (value) {
			has_to_update_top = true;
			break;
		}
	}
	if (has_to_update_top) {
		const Coordinate& top_coord = Coordinate(chunk.grid_coordinate_row - 1, chunk.grid_coordinate_column);
		if (!chunk_map.contains(top_coord)) {
			coordinates_of_chunks_to_create.insert(top_coord);
		}
		ChunkSideUpdateInfo top_info;
		top_info.chunk_to_update_coordinate = top_coord;
		std::copy_n(std::begin(cells_data), Chunk::columns, std::begin(top_info.data));

		chunks_bottom_side_update_infos.push_back(top_info);
	}

	// bottom side of chunk, so top side of neighbour chunk
	bool has_to_update_bottom = false;
	constexpr static int bottom_row_start_index = (Chunk::rows - 1)*Chunk::rows;
	for (int c = 0; c < Chunk::columns; c++) {
		unsigned char value = cells_data[bottom_row_start_index + c];
		if (value) {
			has_to_update_bottom = true;
			break;
		}
	}
	if (has_to_update_bottom) {
		const Coordinate& bottom_coord = Coordinate(chunk.grid_coordinate_row + 1, chunk.grid_coordinate_column);
		if (!chunk_map.contains(bottom_coord)) {
			coordinates_of_chunks_to_create.insert(bottom_coord);
		}
		ChunkSideUpdateInfo bottom_info;
		bottom_info.chunk_to_update_coordinate = bottom_coord;
		std::copy_n(std::end(cells_data) - Chunk::columns , Chunk::columns, std::begin(bottom_info.data));

		chunks_top_side_update_infos.push_back(bottom_info);
	}

	// left side of chunk, so right side of neighbour chunk
	bool has_to_update_left = false;
	std::array<unsigned char, Chunk::columns> left_column;
	for (int r = 0; r < Chunk::rows; r++) {
		unsigned char value = cells_data[r * Chunk::rows];
		if (value) {
			has_to_update_left = true;
		}
		left_column[r] = value;
	}
	if (has_to_update_left) {
		const Coordinate& left_coord = Coordinate(chunk.grid_coordinate_row, chunk.grid_coordinate_column - 1);
		if (!chunk_map.contains(left_coord)) {
			coordinates_of_chunks_to_create.insert(left_coord);
		}
		ChunkSideUpdateInfo left_info;
		left_info.chunk_to_update_coordinate = left_coord;
		left_info.data = left_column;
		chunks_right_side_update_infos.push_back(left_info);
	}

	// right side of chunk, so left side of neighbour chunk
	bool has_to_update_right = false;
	std::array<unsigned char, Chunk::columns> right_column;
	for (int r = 0; r < Chunk::rows; r++) {
		unsigned char value = cells_data[r*Chunk::rows + Chunk::columns - 1];
		if (value) {
			has_to_update_right = true;
		}
		right_column[r] = value;
	}
	if (has_to_update_right) {
		const Coordinate& right_coord = Coordinate(chunk.grid_coordinate_row, chunk.grid_coordinate_column + 1);
		if (!chunk_map.contains(right_coord)) {
			coordinates_of_chunks_to_create.insert(right_coord);
		}
		ChunkSideUpdateInfo right_info;
		right_info.chunk_to_update_coordinate = right_coord;
		right_info.data = right_column;
		chunks_left_side_update_infos.push_back(right_info);
	}

	//top left corner
	if (cells_data[0]) {
		const Coordinate& top_left_coord = Coordinate(chunk.grid_coordinate_row - 1, chunk.grid_coordinate_column - 1);
		if (!chunk_map.contains(top_left_coord)) {
			coordinates_of_chunks_to_create.insert(top_left_coord);
		}
		
		bottom_right_corner_update_infos.push_back(top_left_coord);
	}
	//top right corner
	if (cells_data[Chunk::columns - 1]) {
		const Coordinate& top_right_coord = Coordinate(chunk.grid_coordinate_row - 1, chunk.grid_coordinate_column + 1);
		if (!chunk_map.contains(top_right_coord)) {
			coordinates_of_chunks_to_create.insert(top_right_coord);
		}
		
		bottom_left_corner_update_infos.push_back(top_right_coord);
	}
	//bottom right corner
	if (cells_data[(Chunk::rows - 1) * Chunk::rows + Chunk::columns - 1]) {
		const Coordinate& bottom_right_coord = Coordinate(chunk.grid_coordinate_row + 1, chunk.grid_coordinate_column + 1);
		if (!chunk_map.contains(bottom_right_coord)) {
			coordinates_of_chunks_to_create.insert(bottom_right_coord);
		}
		
		top_left_corner_update_infos.push_back(bottom_right_coord);
	}
	//bottom left corner
	if (cells_data[(Chunk::rows - 1) * Chunk::rows]) {
		const Coordinate& bottom_left_coord = Coordinate(chunk.grid_coordinate_row + 1, chunk.grid_coordinate_column - 1);
		if (!chunk_map.contains(bottom_left_coord)) {
			coordinates_of_chunks_to_create.insert(bottom_left_coord);
		}
		
		top_right_corner_update_infos.push_back(bottom_left_coord);
	}
}

void Grid::update_neighbours_of_all_chunks() {
	ZoneScoped;
	
	std::for_each(
		std::begin(chunks_left_side_update_infos),
		std::end(chunks_left_side_update_infos),
		[this](auto&& it) {
		ChunkSideUpdateInfo& info = it;
		Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_left_side(info.data);
	}
	);

	std::for_each(
		std::begin(chunks_right_side_update_infos),
		std::end(chunks_right_side_update_infos),
		[this](auto&& it) {
		ChunkSideUpdateInfo& info = it;
		Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_right_side(info.data);
	}
	);

	std::for_each(
		std::begin(chunks_top_side_update_infos),
		std::end(chunks_top_side_update_infos),
		[this](auto&& it) {
		ChunkSideUpdateInfo& info = it;
		Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_top_side(info.data);
	}
	);

	std::for_each(
		std::begin(chunks_bottom_side_update_infos),
		std::end(chunks_bottom_side_update_infos),
		[this](auto&& it) {
		ChunkSideUpdateInfo& info = it;
		Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_bottom_side(info.data);
	}
	);
	

	std::for_each(
		std::begin(top_left_corner_update_infos),
		std::end(top_left_corner_update_infos),
		[this](auto&& it) {
		Coordinate& chunk_coordinate = it;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_top_left_corner();
	}
	);
		

	std::for_each(
		std::begin(top_right_corner_update_infos),
		std::end(top_right_corner_update_infos),
		[this](auto&& it) {
		Coordinate& chunk_coordinate = it;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_top_right_corner();
	}
	);

	std::for_each(
		std::begin(bottom_left_corner_update_infos),
		std::end(bottom_left_corner_update_infos),
		[this](auto&& it) {
		Coordinate& chunk_coordinate = it;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_bottom_left_corner();
	}
	);

	std::for_each(
		std::begin(bottom_right_corner_update_infos),
		std::end(bottom_right_corner_update_infos),
		[this](auto&& it) {
		Coordinate& chunk_coordinate = it;
		std::size_t chunk_index = chunk_map.find(chunk_coordinate)->second;
		Chunk& chunk = chunks[chunk_index];
		chunk.update_neighbour_count_bottom_right_corner();
	}
	);
}


void Grid::update_cells_of_all_chunks() {
	ZoneScoped;
	constexpr static bool use_opencl_context = false;
	if (use_opencl_context && opencl_context->is_valid_context) {
		/*
		for (auto& [chunk_coord, chunk]: chunk_map) {
			opencl_context->update_cells(chunk->neighbour_count, chunk->cells);
			chunk->update_chunk_coordinates();
		}
		*/
	} else {
		concurrency::parallel_for_each(
			std::begin(chunks),
			std::end(chunks),
			[](auto&& it) {
			it.update_cells();
		}
		);
		
	}
}


void Grid::remove_empty_chunks() {
	ZoneScoped;

	std::vector<int> indices_of_chunks_to_remove;
	for (int idx = 0; idx < chunks.size(); ++idx) {
		const Chunk& chunk = chunks[idx];
		if (!chunk.has_alive_cells) {
			indices_of_chunks_to_remove.push_back(idx);
		}
	}
	std::size_t number_of_indices_to_remove = indices_of_chunks_to_remove.size();
	if (number_of_indices_to_remove > 0) {
		if (number_of_indices_to_remove == chunks.size()) {
			chunks.clear();
			chunk_map = {};
		} else {

			for (int i = static_cast < int > (indices_of_chunks_to_remove.size()) - 1; i >= 0; i--) {
				// get the biggest index to remove
				int idx = indices_of_chunks_to_remove[i];
				int idx_of_last_element = static_cast<int>(chunks.size()) - 1;
				// check if its at the last position
				if (idx == idx_of_last_element) {
					assert(chunks.size() > 0);
					Chunk& chunk = chunks.back();
					chunks.pop_back();
					// update chunk map as well.
					chunk_map.erase(Coordinate(chunk.grid_coordinate_row, chunk.grid_coordinate_column));
				} else {
					// if its not at the last position, move the last elem to that position and update the chunk map.
					const Chunk& chunk_to_remove = chunks[idx];
					chunk_map.erase(Coordinate(chunk_to_remove.grid_coordinate_row, chunk_to_remove.grid_coordinate_column));

					assert(chunks.size() > 0);
					const Chunk& last_chunk = chunks.back();
					chunks.pop_back();
					Coordinate last_chunk_coordinate = Coordinate(last_chunk.grid_coordinate_row, last_chunk.grid_coordinate_column);
					chunk_map.erase(last_chunk_coordinate);

					chunks[idx] = last_chunk;
					chunk_map.insert(std::make_pair(last_chunk_coordinate, idx));
				}
			}
		}
	}
}