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

	grid_info->iteration = grid->iteration;
	grid_info->rows = 0;
	grid_info->columns = 0;
	grid_info->origin_row = 0;
	grid_info->origin_column = 0;
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
		grid->update_coordinates_for_alive_grid_cells();
		grid_execution_state.updated_grid_coordinates = true;
	}

	if (grid->iteration == 0 || (grid_execution_state.show_chunk_borders && grid_changed) || grid_execution_state.have_to_update_chunk_borders) {
		grid->update_coordinates_for_chunk_borders();
		grid_execution_state.updated_border_coordinates = true;
	}
	// @Cleanup, factor this into a grid_info->update() call?
	update_grid_info();
}


void Grid::update_coordinates_for_alive_grid_cells() {
	ZoneScoped;

	grid_coordinates.clear();
	for (auto& [chunk_coord, chunk]: chunk_map) {
		// transform local chunk coordinates of alive grid cells into world coordinates and add them to our vector of world coordinates

		// get chunk coordinates from alive grid cells
		std::array<unsigned char, Chunk::rows*Chunk::columns>& cells_data = chunk.cells_data;
		for (int i = 0; i < Chunk::rows * Chunk::columns; i++) {
			int r = i / Chunk::rows;
			int c = i % Chunk::columns;
			if (cells_data[i]) {
				grid_coordinates.push_back(std::make_pair(r + chunk.chunk_origin_row, c + chunk.chunk_origin_column));
			}
		}
	}
}

void Grid::update_coordinates_for_chunk_borders() {
	ZoneScoped;

	border_coordinates.clear();
	for (auto& [chunk_coord, chunk]: chunk_map) {
		for (int r = 0; r < Chunk::rows; r++) {
			border_coordinates.push_back(std::make_pair(chunk.chunk_origin_row + r, chunk.chunk_origin_column - 1));
			border_coordinates.push_back(std::make_pair(chunk.chunk_origin_row + r, chunk.chunk_origin_column + Chunk::columns));
		}
		for (int c = 0; c < Chunk::columns; c++) {
			border_coordinates.push_back(std::make_pair(chunk.chunk_origin_row -1, chunk.chunk_origin_column + c));
			border_coordinates.push_back(std::make_pair(chunk.chunk_origin_row + Chunk::rows, chunk.chunk_origin_column + c));
		}
	}
}

//--------------------------------------------------------------------------------
Grid::Grid(std::shared_ptr<OpenCLContext> context) :
	iteration(0),
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
	create_new_chunk_and_set_alive_cells(Coordinate(0, 0), initial_coordinates);


	update_coordinates_for_alive_grid_cells();

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

	Chunk chunk = Chunk(coord, origin_coordinate, coordinates);

	chunk_map.insert(std::make_pair(coord, chunk));
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

	iteration++;

	update_neighbour_count_and_set_info_of_all_chunks();

	for (Coordinate coord: coordinates_of_chunks_to_create) {
		create_new_chunk(coord);
	}

	update_neighbours_of_all_chunks();

	update_cells_of_all_chunks();
	
	remove_empty_chunks();
}


void Grid::update_neighbour_count_and_set_info_of_all_chunks() {
	ZoneScoped;

	chunks_left_side_update_infos.clear();
	chunks_right_side_update_infos.clear();
	chunks_bottom_side_update_infos.clear();
	chunks_top_side_update_infos.clear();

	top_left_corner_update_infos.clear();
	top_right_corner_update_infos.clear();
	bottom_left_corner_update_infos.clear();
	bottom_right_corner_update_infos.clear();

	coordinates_of_chunks_to_create.clear();

	std::for_each(
		std::execution::par_unseq,
		chunk_map.begin(),
		chunk_map.end(),
		[](auto&& it) {
		Chunk& chunk = it.second;
		chunk.update_neighbour_count_inside();
	}
	);

	// cant use pragma omp parallel below, since we insert into a vector and that is not thread safe.
	for (auto it = chunk_map.begin(); it != chunk_map.end(); ++it) {
		{
			Chunk& chunk = it->second;
			set_chunk_neighbour_info(chunk);
		}
	}
}

void Grid::set_chunk_neighbour_info(Chunk& chunk) {
	ZoneScoped;

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

		ChunkSideUpdateInfo& top_info = chunks_bottom_side_update_infos.emplace_back(top_coord);
		std::copy_n(std::begin(cells_data), Chunk::columns, std::begin(top_info.data));
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

		ChunkSideUpdateInfo& bottom_info = chunks_top_side_update_infos.emplace_back(bottom_coord);
		std::copy_n(std::end(cells_data) - Chunk::columns , Chunk::columns, std::begin(bottom_info.data));
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
		chunks_right_side_update_infos.emplace_back(left_column, left_coord);
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
		chunks_left_side_update_infos.emplace_back(right_column, right_coord);
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
	{
		for (ChunkSideUpdateInfo& info : chunks_left_side_update_infos) {
			Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
			Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
			chunk.update_neighbour_count_left_side(info.data);
		}

		{
			for (ChunkSideUpdateInfo& info : chunks_right_side_update_infos) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_right_side(info.data);
			}
		}

		{
			for (ChunkSideUpdateInfo& info : chunks_top_side_update_infos) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_side(info.data);
			}
		}

		{
			for (ChunkSideUpdateInfo& info : chunks_bottom_side_update_infos) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_side(info.data);
			}
		}

		{
			for (Coordinate& chunk_coordinate : top_left_corner_update_infos) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_left_corner();
			}
		}

		{
			for (Coordinate& chunk_coordinate : top_right_corner_update_infos) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_right_corner();
			}
		}

		{
			for (Coordinate& chunk_coordinate : bottom_left_corner_update_infos) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_left_corner();
			}
		}



		{
			for (Coordinate& chunk_coordinate : bottom_right_corner_update_infos) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_right_corner();
			}
		}
	}
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
		std::for_each(
			std::execution::par_unseq,
			chunk_map.begin(),
			chunk_map.end(),
			[](auto&& it) {
				Chunk& chunk = it.second;
				chunk.update_cells();
			}
		);
	}
}


void Grid::remove_empty_chunks() {
	ZoneScoped;

	// remove all chunks, which dont have alive cells.
	boost::unordered::erase_if(chunk_map, [](const auto& item) {
		return !item.second.has_alive_cells;
		});
}


	/*
		std::for_each(
			std::execution::par_unseq,
			chunks_left_side_update_infos.begin(),
			chunks_left_side_update_infos.end(),
			[this](ChunkSideUpdateInfo& info) {
			// todo should use auto&& it above!
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_left_side(info.data);
			}
		);
		std::for_each(
			std::execution::par_unseq,
			chunks_right_side_update_infos.begin(),
			chunks_right_side_update_infos.end(),
			[this](ChunkSideUpdateInfo& info) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
		Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
		chunk.update_neighbour_count_right_side(info.data);
			}
		);
		std::for_each(
			std::execution::par_unseq,
			chunks_top_side_update_infos.begin(),
			chunks_top_side_update_infos.end(),
			[this](ChunkSideUpdateInfo& info) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_side(info.data);
			}
		);
		std::for_each(
			std::execution::par_unseq,
			chunks_bottom_side_update_infos.begin(),
			chunks_bottom_side_update_infos.end(),
			[this](ChunkSideUpdateInfo& info) {
				Coordinate chunk_coordinate = info.chunk_to_update_coordinate;
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_side(info.data);
			}
		);

		std::for_each(
			std::execution::par_unseq,
			top_left_corner_update_infos.begin(),
			top_left_corner_update_infos.end(),
			[this](Coordinate& chunk_coordinate) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_left_corner();
			}
		);

		std::for_each(
			std::execution::par_unseq,
			top_right_corner_update_infos.begin(),
			top_right_corner_update_infos.end(),
			[this](Coordinate& chunk_coordinate) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_top_right_corner();
			}
		);
		std::for_each(
			std::execution::par_unseq,
			bottom_left_corner_update_infos.begin(),
			bottom_left_corner_update_infos.end(),
			[this](Coordinate& chunk_coordinate) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_left_corner();
			}
		);
		std::for_each(
			std::execution::par_unseq,
			bottom_right_corner_update_infos.begin(),
			bottom_right_corner_update_infos.end(),
			[this](Coordinate& chunk_coordinate) {
				Chunk& chunk = chunk_map.find(chunk_coordinate)->second;
				chunk.update_neighbour_count_bottom_right_corner();
			}
		);
	*/