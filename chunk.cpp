#include "chunk.hpp"

ChunkUpdateInfo::ChunkUpdateInfo() 
{

}

void ChunkUpdateInfo::add_coordinate(char value) {
	ZoneScoped;
	switch (direction) {
		case ChunkUpdateInfoDirection::LEFT:
			// fallthrough, handle left and right together
		case ChunkUpdateInfoDirection::RIGHT:
			data.push_back(std::make_pair(value + chunk_offset_coordinate.x, chunk_offset_coordinate.y));
			break;
		case ChunkUpdateInfoDirection::TOP:
			// fallthrough, handle bottom and top together
		case ChunkUpdateInfoDirection::BOTTOM:
			data.push_back(std::make_pair(chunk_offset_coordinate.x, value + chunk_offset_coordinate.y));
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:
			// fallthrough, handle all corners together
		case ChunkUpdateInfoDirection::TOP_RIGHT:
			// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:
			// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:
			data.push_back(std::make_pair(chunk_offset_coordinate.x, chunk_offset_coordinate.y));
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}
}

void ChunkUpdateInfo::initialise(ChunkUpdateInfoDirection dir, Coordinate chunk_grid_coordinate) 
{
	ZoneScoped;

	direction = dir;
	int grid_row_offset = 0;
	int grid_column_offset = 0;

	data.clear();
	data_max_value = 0;

	chunk_offset_coordinate = Coordinate(0, 0);
	switch (dir) {
		case ChunkUpdateInfoDirection::LEFT:
			grid_column_offset = -1;
			data_max_value = Chunk::rows - 2;
			chunk_offset_coordinate = Coordinate(0, Chunk::columns - 1);
			break;
		case ChunkUpdateInfoDirection::RIGHT:
			grid_column_offset = 1;
			data_max_value = Chunk::rows - 2;
			chunk_offset_coordinate = Coordinate(0, 0);
			break;
		case ChunkUpdateInfoDirection::TOP:
			grid_row_offset = -1;
			data_max_value = Chunk::columns - 2;
			chunk_offset_coordinate = Coordinate(Chunk::rows - 1, 0);
			break;
		case ChunkUpdateInfoDirection::BOTTOM:
			grid_row_offset = 1;
			data_max_value = Chunk::columns - 2;
			chunk_offset_coordinate = Coordinate(0, 0);
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:
			grid_row_offset = -1;
			grid_column_offset = -1;
			chunk_offset_coordinate = Coordinate(Chunk::rows - 1, Chunk::columns - 1);
			break;
		case ChunkUpdateInfoDirection::TOP_RIGHT:
			grid_row_offset = -1;
			grid_column_offset = 1;
			chunk_offset_coordinate = Coordinate(Chunk::rows - 1, 0);
			break;
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:
			grid_row_offset = 1;
			grid_column_offset = -1;
			chunk_offset_coordinate = Coordinate(0, Chunk::columns - 1);
			break;
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:
			grid_row_offset = 1;
			grid_column_offset = 1;
			chunk_offset_coordinate = Coordinate(0, 0);
			break;
		case ChunkUpdateInfoDirection::DIRECTION_COUNT:
			break;
		default:
			break;
	}

	neighbour_grid_coordinate = Coordinate(chunk_grid_coordinate.x + grid_row_offset, chunk_grid_coordinate.y + grid_column_offset);
}

Chunk::Chunk(const Coordinate& coord, Coordinate origin_coord) :
	grid_coordinate_row(coord.x),
grid_coordinate_column(coord.y),
chunk_origin_row(origin_coord.x),
chunk_origin_column(origin_coord.y),
number_of_alive_cells(0)
{
	ZoneScoped;
	cells.setConstant(false);
	neighbour_count.setConstant(0);

	for (int direction = ChunkUpdateInfoDirection::LEFT; direction < ChunkUpdateInfoDirection::DIRECTION_COUNT; direction++) {
		ChunkUpdateInfo& info = update_info[direction];
		info.initialise((ChunkUpdateInfoDirection) direction, Coordinate(grid_coordinate_row, grid_coordinate_column));
	}
}


void Chunk::clear_neighbour_update_info() {
	ZoneScoped;

	for (ChunkUpdateInfo& info: update_info) {
		info.data.clear();
	}
}

Coordinate Chunk::transform_to_world_coordinate(Coordinate chunk_coord) {
	ZoneScoped;

	return Coordinate(chunk_coord.x + chunk_origin_row, chunk_coord.y + chunk_origin_column);
}

void Chunk::update_neighbour_count_in_direction(ChunkUpdateInfoDirection direction) {
	ZoneScoped;

	ChunkUpdateInfo& info = update_info[direction];

	int current_row = 0;
	int row_offset = 0; // offset, can either be -1, 0, or 1. It defines if we look one row up, stay in the current row or look one row down, when updating neighbour count.
	// For top it has to be +1, so we only update the first row of our current neighbour count of the current chunk. For bottom it has to be -1, so we update only the second to last row and stay inside the bounds of neighbour count.

	// column case completely analogous to row case.
	int current_column = 0;
	int column_offset = 0;

	ChunkUpdateInfo* top_or_bottom_info = nullptr;
	ChunkUpdateInfo* left_or_right_info = nullptr;
	switch (direction) {
		case ChunkUpdateInfoDirection::LEFT:
			current_column = 0;
			column_offset = 1;
			break;
		case ChunkUpdateInfoDirection::RIGHT:
			current_column = Chunk::columns - 1;
			column_offset = -1;
			break;
		case ChunkUpdateInfoDirection::TOP:
			current_row = 0;
			row_offset = 1;
			break;
		case ChunkUpdateInfoDirection::BOTTOM:
			current_row = Chunk::rows - 1;
			row_offset = -1;
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:
			current_row = 0;
			row_offset = 1;
			current_column = 0;
			column_offset = 1;

			top_or_bottom_info = &update_info[ChunkUpdateInfoDirection::TOP];
			left_or_right_info = &update_info[ChunkUpdateInfoDirection::LEFT];
			break;
		case ChunkUpdateInfoDirection::TOP_RIGHT:
			current_row = 0;
			row_offset = 1;
			current_column = Chunk::columns - 1;
			column_offset = -1;

			top_or_bottom_info = &update_info[ChunkUpdateInfoDirection::TOP];
			left_or_right_info = &update_info[ChunkUpdateInfoDirection::RIGHT];
			break;
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:
			current_row = Chunk::rows - 1;
			row_offset = -1;
			current_column = 0;
			column_offset = 1;

			top_or_bottom_info = &update_info[ChunkUpdateInfoDirection::BOTTOM];
			left_or_right_info = &update_info[ChunkUpdateInfoDirection::LEFT];
			break;
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:
			current_row = Chunk::rows - 1;
			row_offset = -1;
			current_column = Chunk::columns - 1;
			column_offset = -1;

			top_or_bottom_info = &update_info[ChunkUpdateInfoDirection::BOTTOM];
			left_or_right_info = &update_info[ChunkUpdateInfoDirection::RIGHT];
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}

	// having determined what row/column we need to update we do it below. This is intentionally split up into two switch statements, so that we can streamline the bottom code via simple fallthrough, which actually keeps us from writing the same code/logic for left-right and bottom-top case again.
	unsigned char* neighbour_count_data = neighbour_count.data();
	switch (direction) {
		case ChunkUpdateInfoDirection::LEFT: // fallthrough, left and right together
		case ChunkUpdateInfoDirection::RIGHT:
			for (int r = 1; r < rows - 1; r++) {
				if (cells(r, current_column)) {
					info.add_coordinate(r);
					
					neighbour_count_data[(r - 1) * rows + current_column]++;
					neighbour_count_data[(r + 1) * rows + current_column]++;
					neighbour_count_data[(r - 1) * rows + current_column + column_offset]++;
					neighbour_count_data[r * rows + current_column + column_offset]++;
					neighbour_count_data[(r + 1)*rows + current_column + column_offset]++;
				}
				/*
				neighbour_count(r, current_column) += cells(r - 1, current_column);
				neighbour_count(r, current_column) += cells(r + 1, current_column);

				neighbour_count(r, current_column) += cells(r - 1, current_column + column_offset);
				neighbour_count(r, current_column) += cells(r, current_column + column_offset);
				neighbour_count(r, current_column) += cells(r + 1, current_column + column_offset);
				*/
			}
			break;
		case ChunkUpdateInfoDirection::TOP: // fallthrough, bottom and top together.
		case ChunkUpdateInfoDirection::BOTTOM:
			for (int c = 1; c < columns - 1; c++) {
				if (cells(current_row, c)) {
					info.add_coordinate(c);

					neighbour_count_data[current_row*rows + c - 1]++; 
					neighbour_count_data[current_row*rows + c + 1]++; 
					neighbour_count_data[(current_row + row_offset)*rows + c - 1]++; 
					neighbour_count_data[(current_row + row_offset)*rows + c]++; 
					neighbour_count_data[(current_row + row_offset)*rows + c + 1]++; 
				}
				/*
				neighbour_count(current_row, c) += cells(current_row, c - 1);
				neighbour_count(current_row, c) += cells(current_row, c + 1);

				neighbour_count(current_row, c) += cells(current_row + row_offset, c - 1);
				neighbour_count(current_row, c) += cells(current_row + row_offset, c);
				neighbour_count(current_row, c) += cells(current_row + row_offset, c + 1);
				*/
			}
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:// fallthrough
		case ChunkUpdateInfoDirection::TOP_RIGHT:// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:// fallthrough
			if (cells(current_row, current_column)) {
				info.add_coordinate(0);
				left_or_right_info->add_coordinate(current_row);
				top_or_bottom_info->add_coordinate(current_column);
				
				neighbour_count_data[(current_row + row_offset)*rows +  current_column + column_offset]++; 
				neighbour_count_data[(current_row + row_offset)*rows +  current_column]++;
				neighbour_count_data[current_row*rows +  current_column + column_offset]++;
			}
			/*
			neighbour_count(current_row, current_column) += cells(current_row + row_offset, current_column + column_offset);
			neighbour_count(current_row, current_column) += cells(current_row + row_offset, current_column);
			neighbour_count(current_row, current_column) += cells(current_row, current_column + column_offset);
			*/
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}
}

void Chunk::update_neighbour_count_and_set_info() {
	ZoneScoped;

	clear_neighbour_update_info();

	update_neighbour_count_inside();

	for (int direction = ChunkUpdateInfoDirection::LEFT; direction < ChunkUpdateInfoDirection::DIRECTION_COUNT; direction++) {
		update_neighbour_count_in_direction(static_cast<ChunkUpdateInfoDirection> (direction));
	}
}


bool Chunk::has_to_update_in_direction(ChunkUpdateInfoDirection direction) {
	return update_info[direction].data.size() > 0;
}


void Chunk::update_neighbour_count_inside() {
	ZoneScoped;
	neighbour_count.setConstant(0);

	
	unsigned char* neighbour_count_data = neighbour_count.data();
	unsigned char* cells_data = cells.data();
	for (int r = 1; r < rows - 1; r++) {
#pragma omp simd
		for (int c = 1; c < columns - 1; c++) {
			int i = r * rows + c;
			
			/*
			neighbour_count_data[i] += cells_data[rows * (r - 1) + c - 1];
			neighbour_count_data[i] += cells_data[rows * (r - 1) + c];
			neighbour_count_data[i] += cells_data[rows * (r - 1) + c + 1];
			neighbour_count_data[i] += cells_data[rows * r + c - 1];
			neighbour_count_data[i] += cells_data[rows * r + c + 1];
			neighbour_count_data[i] += cells_data[rows * (r + 1) + c - 1];
			neighbour_count_data[i] += cells_data[rows * (r + 1) + c];
			neighbour_count_data[i] += cells_data[rows * (r + 1) + c + 1];
			*/
			
			if (cells_data[i]) {
				//neighbour_count(r - 1, c - 1)++;
				int index1 = rows * (r - 1) + c - 1;
				neighbour_count_data[index1]++;
				//neighbour_count(r - 1, c)++;
				int index2 = rows * (r - 1) + c;
				neighbour_count_data[index2]++;

				//neighbour_count(r - 1, c + 1)++;
				int index3 = rows * (r - 1) + c + 1;
				neighbour_count_data[index3]++;

				//neighbour_count(r, c - 1)++;
				int index4 = rows * r + c - 1;
				neighbour_count_data[index4]++;

				//neighbour_count(r, c + 1)++;
				int index5 = rows * r + c + 1;
				neighbour_count_data[index5]++;

				//neighbour_count(r + 1, c - 1)++;
				int index6 = rows * (r + 1) + c - 1;
				neighbour_count_data[index6]++;

				//neighbour_count(r + 1, c)++;
				int index7 = rows * (r + 1) + c;
				neighbour_count_data[index7]++;

				//neighbour_count(r + 1, c + 1)++;
				int index8 = rows * (r + 1) + c + 1;
				neighbour_count_data[index8]++;
			}	
		}
	}
	/*
	for (int r = 1; r < rows - 1; r++) {
		for (int c = 1; c < columns - 1; c++) {
			if (cells(r, c)) {
				neighbour_count(r - 1, c - 1)++;
				neighbour_count(r - 1, c)++;
				neighbour_count(r - 1, c + 1)++;
				neighbour_count(r, c - 1)++;
				
				neighbour_count(r, c + 1)++;
				neighbour_count(r + 1, c - 1)++;
				neighbour_count(r + 1, c)++;
				neighbour_count(r + 1, c + 1)++;
			}
		}
	}
	*/
}

void Chunk::update_cells() {
	ZoneScoped;
	/*
	unsigned char* neighbour_count_data = neighbour_count.data();
	unsigned char* cells_data = cells.data();

	unsigned char is_count_2[rows * columns];
#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		unsigned char last_byte_of_count = neighbour_count_data[i];// &0xF;
		is_count_2[i] = static_cast<unsigned char>(!(last_byte_of_count ^ 0x2));
	}

	unsigned char is_count_3[rows * columns];
#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		unsigned char last_byte_of_count = neighbour_count_data[i] & 0xF;
		is_count_3[i] = static_cast<unsigned char>(!(last_byte_of_count ^ 0x3));
	}
	
	unsigned char is_count_2_or_3[rows * columns];
#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		is_count_2_or_3[i] = is_count_2[i] | is_count_3[i];
	}
	
	unsigned char alive_cells_data[rows * columns];
#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		alive_cells_data[i] = cells_data[i] & is_count_2_or_3[i];
	}

	unsigned char dead_cells_data[rows * columns];
#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		dead_cells_data[i] = static_cast<unsigned char>(!cells_data[i]) & is_count_3[i];
	}

#pragma omp simd 
	for (int i = 0; i < rows * columns; i++) {
		cells_data[i] = alive_cells_data[i] | dead_cells_data[i];
	}
	*/
	number_of_alive_cells = 0;
	unsigned char* neighbour_count_data = neighbour_count.data();
	unsigned char* cells_data = cells.data();
	for (int i = 0; i < rows * columns; i++) {
		unsigned char count = neighbour_count_data[i];
		if (cells_data[i]) {
			// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
			if (count == 2 || count == 3) {
				cells_data[i] = true;
				number_of_alive_cells++;
			} else {
				cells_data[i] = false;
			}
		} else {
			// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
			if (count == 3) {
				cells_data[i] = true;
				number_of_alive_cells++;
			} else {
				cells_data[i] = false;
			}
		}
	}
}