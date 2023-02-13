#include "grid.hpp"

using Eigen::Array;



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
		std::array<char, Chunk::rows*Chunk::columns>& cells_data = chunk->cells_data;
		for (int i = 0; i < Chunk::rows * Chunk::columns; i++) {
			int r = i / Chunk::rows;
			int c = i % Chunk::columns;
			if (cells_data[i]) {
				grid_coordinates.push_back(std::make_pair(r + chunk->chunk_origin_row, c + chunk->chunk_origin_column));
			}
		}
	}
	
}

void Grid::update_coordinates_for_chunk_borders() {
	ZoneScoped;

	border_coordinates.clear();
	for (auto& [chunk_coord, chunk]: chunk_map) {
		for (int r = 0; r < Chunk::rows; r++) {
			border_coordinates.push_back(std::make_pair(chunk->chunk_origin_row + r,chunk->chunk_origin_column - 1));
			border_coordinates.push_back(std::make_pair(chunk->chunk_origin_row + r, chunk->chunk_origin_column + Chunk::columns));
		}
		for (int c = 0; c < Chunk::columns; c++) {
			border_coordinates.push_back(std::make_pair(chunk->chunk_origin_row -1, chunk->chunk_origin_column + c));
			border_coordinates.push_back(std::make_pair(chunk->chunk_origin_row + Chunk::rows, chunk->chunk_origin_column+ c));
		}
	}
}

//--------------------------------------------------------------------------------
Grid::Grid(std::shared_ptr<OpenCLContext> context) :
iteration(0),
opencl_context(context)
{
	ZoneScoped;

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

	Coordinate origin_coordinate = Coordinate(coord.x * Chunk::rows, coord.y * Chunk::columns);

	std::shared_ptr<Chunk> chunk = std::make_shared < Chunk > (coord, origin_coordinate, coordinates); 


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


void Grid::create_needed_neighbours_of_all_chunks() {
	ZoneScoped;
	// set of coordinates of the neighbour chunks, note that this is a set and hence we do not create neighbours multiple times
	std::unordered_set<Coordinate> coordinates_of_chunks_to_create;
	for (ChunkUpdateInfo& chunk_update_infos: update_info) {
		for (ChunkUpdateInDirectionInfo& info: chunk_update_infos.data) {
			if (info.is_not_trivial()) {
				Coordinate neighbour_grid_coordinate = info.neighbour_grid_coordinate;
				if (!chunk_map.contains(neighbour_grid_coordinate)) {
					coordinates_of_chunks_to_create.insert(neighbour_grid_coordinate);
				}
			}
		}
	}

	// create the chunks now
	for (Coordinate coord: coordinates_of_chunks_to_create) {
		create_new_chunk(coord);
	}
}

void Grid::next_iteration() {
	ZoneScoped;

	iteration++;

	update_neighbour_count_and_set_info_of_all_chunks();

	create_needed_neighbours_of_all_chunks();

	update_neighbours_of_all_chunks();

	update_cells_of_all_chunks();
	
	remove_empty_chunks();
}

void Grid::update_neighbour_count_and_set_info_of_all_chunks() {
	ZoneScoped;

	update_info.clear();

	for (auto& [chunk_coord, chunk]: chunk_map) {
		chunk->update_neighbour_count_and_set_info(update_info);
	}
}

void Grid::update_neighbours_of_all_chunks() {
	ZoneScoped;

	for (ChunkUpdateInfo& chunk_update_info: update_info) {
		update_neighbours_of_chunk(chunk_update_info);
	}

	update_info.clear();
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
		for (auto& [chunk_coord, chunk]: chunk_map) {
			chunk->update_cells();
		}
	}
}


void Grid::remove_empty_chunks() {
	for (auto it = chunk_map.begin(); it != chunk_map.end();) {
		if (!it->second->has_alive_cells) {
			it = chunk_map.erase(it); // erase does not invalidate the iterator of std::unordered_map, (insert does!)
		} else {
			// DONT FORGET TO ADVANCE THE ITERATOR!
			it++;
		}
	}
}


void Grid::update_neighbours_of_chunk(ChunkUpdateInfo& chunk_update_info) {
	ZoneScoped;

	for (ChunkUpdateInDirectionInfo& info: chunk_update_info.data) {
		ChunkUpdateInfoDirection direction = info.direction;
		// if its trivial continue
		if (!info.is_not_trivial()) {
			continue;
		}

		Coordinate neighbour_grid_coordinate = info.neighbour_grid_coordinate;
		std::shared_ptr<Chunk> neighbour_chunk = chunk_map.find(neighbour_grid_coordinate)->second;

		std::array<char, Chunk::rows*Chunk::columns>& neighbour_count_data = neighbour_chunk->neighbour_count_data;
		if (direction == LEFT || direction == RIGHT) {
			for (int i = 0; i < info.current_number_of_values; i++) {
				std::pair<char, char>& coord = info.data[i];
				int r = coord.first;
				int c = coord.second;

				neighbour_count_data[r*Chunk::rows +c]++;
				if (r > 0) {
					neighbour_count_data[(r-1)*Chunk::rows +c]++;

				}
				if (r <= info.data_max_value) {
					neighbour_count_data[(r+ 1)*Chunk::rows +c]++;
				}
			}
		} else if (direction == TOP || direction == BOTTOM) {
			for (int i = 0; i < info.current_number_of_values; i++) {
				std::pair<char, char>& coord = info.data[i];
				int r = coord.first;
				int c = coord.second;

				neighbour_count_data[r*Chunk::rows +c]++;
				if (c > 0) {
					neighbour_count_data[r*Chunk::rows +c -1]++;
				}
				if (c <= info.data_max_value) {
					neighbour_count_data[r*Chunk::rows +c + 1]++;
				}
			}
		} else if (direction == TOP_LEFT || direction == TOP_RIGHT || direction == BOTTOM_LEFT || direction == BOTTOM_RIGHT) {
			std::pair<char, char>& coord = info.data[0];
			
			int r = coord.first;
			int c = coord.second;
			neighbour_count_data[r*Chunk::rows +c]++;
		}
	}
}