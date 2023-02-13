#include "chunk.hpp"

ChunkUpdateInDirectionInfo::ChunkUpdateInDirectionInfo() :
	data_max_value(0),
current_number_of_values(0),
data({})
{

}

bool ChunkUpdateInDirectionInfo::is_not_trivial() {
	return current_number_of_values > 0;
}

void ChunkUpdateInDirectionInfo::add_coordinate(char value) {
	ZoneScoped;
	switch (direction) {
		case ChunkUpdateInfoDirection::LEFT:
			// fallthrough, handle left and right together
		case ChunkUpdateInfoDirection::RIGHT:
			data[current_number_of_values++] = std::make_pair(value + chunk_offset_coordinate.x, chunk_offset_coordinate.y);
			break;
		case ChunkUpdateInfoDirection::TOP:
			// fallthrough, handle bottom and top together
		case ChunkUpdateInfoDirection::BOTTOM:
			data[current_number_of_values++] = std::make_pair(chunk_offset_coordinate.x, value + chunk_offset_coordinate.y);
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:
			// fallthrough, handle all corners together
		case ChunkUpdateInfoDirection::TOP_RIGHT:
			// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:
			// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:
			data[current_number_of_values++] = std::make_pair(chunk_offset_coordinate.x, chunk_offset_coordinate.y);
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}
}

void ChunkUpdateInDirectionInfo::initialise(ChunkUpdateInfoDirection dir, Coordinate chunk_grid_coordinate)
{
	ZoneScoped;

	direction = dir;
	int grid_row_offset = 0;
	int grid_column_offset = 0;

	data = {};
	current_number_of_values = 0;

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

Chunk::Chunk(const Coordinate& coord, Coordinate origin_coord, const std::vector<std::pair<int, int>>& alive_cells_coordinates) :
	grid_coordinate_row(coord.x),
grid_coordinate_column(coord.y),
chunk_origin_row(origin_coord.x),
chunk_origin_column(origin_coord.y),
has_alive_cells(false),
cells_data({}),
neighbour_count_data({})
{
	ZoneScoped;

	has_alive_cells = alive_cells_coordinates.size() > 0;
	for (auto [r, c]: alive_cells_coordinates) {
		cells_data[r*Chunk::rows + c] = 0xFF;
	}
	

}

ChunkUpdateInfo::ChunkUpdateInfo(Coordinate chunk_grid_coord) :
	data({})
{
	ZoneScoped;

	for (int direction = ChunkUpdateInfoDirection::LEFT; direction < ChunkUpdateInfoDirection::DIRECTION_COUNT; direction++) {
		ChunkUpdateInDirectionInfo& info = data[direction];
		info.initialise((ChunkUpdateInfoDirection) direction, chunk_grid_coord);
	}
}

Coordinate Chunk::transform_to_world_coordinate(Coordinate chunk_coord) {
	ZoneScoped;

	return Coordinate(chunk_coord.x + chunk_origin_row, chunk_coord.y + chunk_origin_column);
}


void Chunk::update_neighbour_count_in_direction(ChunkUpdateInfoDirection direction, ChunkUpdateInfo& update_info) {
	ZoneScoped;

	ChunkUpdateInDirectionInfo& info = update_info.data[direction];

	int current_row = 0;
	int row_offset = 0; // offset, can either be -1, 0, or 1. It defines if we look one row up, stay in the current row or look one row down, when updating neighbour count.
	// For top it has to be +1, so we only update the first row of our current neighbour count of the current chunk. For bottom it has to be -1, so we update only the second to last row and stay inside the bounds of neighbour count.

	// column case completely analogous to row case.
	int current_column = 0;
	int column_offset = 0;

	ChunkUpdateInDirectionInfo* top_or_bottom_info = nullptr;
	ChunkUpdateInDirectionInfo* left_or_right_info = nullptr;
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

			top_or_bottom_info = &update_info.data[ChunkUpdateInfoDirection::TOP];
			left_or_right_info = &update_info.data[ChunkUpdateInfoDirection::LEFT];
			break;
		case ChunkUpdateInfoDirection::TOP_RIGHT:
			current_row = 0;
			row_offset = 1;
			current_column = Chunk::columns - 1;
			column_offset = -1;

			top_or_bottom_info = &update_info.data[ChunkUpdateInfoDirection::TOP];
			left_or_right_info = &update_info.data[ChunkUpdateInfoDirection::RIGHT];
			break;
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:
			current_row = Chunk::rows - 1;
			row_offset = -1;
			current_column = 0;
			column_offset = 1;

			top_or_bottom_info = &update_info.data[ChunkUpdateInfoDirection::BOTTOM];
			left_or_right_info = &update_info.data[ChunkUpdateInfoDirection::LEFT];
			break;
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:
			current_row = Chunk::rows - 1;
			row_offset = -1;
			current_column = Chunk::columns - 1;
			column_offset = -1;

			top_or_bottom_info = &update_info.data[ChunkUpdateInfoDirection::BOTTOM];
			left_or_right_info = &update_info.data[ChunkUpdateInfoDirection::RIGHT];
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}

	// having determined what row/column we need to update we do it below. This is intentionally split up into two switch statements, so that we can streamline the bottom code via simple fallthrough, which actually keeps us from writing the same code/logic for left-right and bottom-top case again.
	//unsigned char* neighbour_count_data = neighbour_count.data();
	switch (direction) {
		case ChunkUpdateInfoDirection::LEFT: // fallthrough, left and right together
		case ChunkUpdateInfoDirection::RIGHT:
			for (int r = 1; r < rows - 1; r++) {
				if (cells_data[r*rows + current_column]) {
					info.add_coordinate(r);
				}
			}
			break;
		case ChunkUpdateInfoDirection::TOP: // fallthrough, bottom and top together.
		case ChunkUpdateInfoDirection::BOTTOM:
			for (int c = 1; c < columns - 1; c++) {
				if (cells_data[current_row*rows + c]) {
					info.add_coordinate(c);
				}
				
			}
			break;
		case ChunkUpdateInfoDirection::TOP_LEFT:// fallthrough
		case ChunkUpdateInfoDirection::TOP_RIGHT:// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_LEFT:// fallthrough
		case ChunkUpdateInfoDirection::BOTTOM_RIGHT:// fallthrough
			if (cells_data[current_row*rows + current_column]) {
				info.add_coordinate(0);

				left_or_right_info->add_coordinate(current_row);
				top_or_bottom_info->add_coordinate(current_column);
			}
			break;
		case DIRECTION_COUNT:
			break;
		default:
			break;
	}
}

void Chunk::update_neighbour_count_and_set_info(std::vector<ChunkUpdateInfo>& update_info_data) {
	ZoneScoped;

	update_neighbour_count_inside();

	ChunkUpdateInfo update_info = ChunkUpdateInfo(Coordinate(grid_coordinate_row, grid_coordinate_column));

	bool is_not_trivial_info = false;
	for (int direction = ChunkUpdateInfoDirection::LEFT; direction < ChunkUpdateInfoDirection::DIRECTION_COUNT; direction++) {
		update_neighbour_count_in_direction(static_cast<ChunkUpdateInfoDirection>(direction), update_info);
		is_not_trivial_info |= update_info.data[direction].is_not_trivial();
	}

	if (is_not_trivial_info) {
		update_info_data.push_back(update_info);
	}
}


void Chunk::update_neighbour_count_inside() {
	ZoneScoped;

	__m256i _mm256_epi8_value_1 = _mm256_set_epi8(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	__m256i* cells_data_ptr = (__m256i*) &cells_data[0];
	__m256i* neighbour_count_data_ptr = (__m256i*) &neighbour_count_data[0];

	__m256i current_row_cells_data = _mm256_load_si256(&cells_data_ptr[0]);
	__m256i values_middle = _mm256_blendv_epi8(_mm256_setzero_si256(), _mm256_epi8_value_1, current_row_cells_data);
	//__m256i test = _mm256_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
	//__m256i test = _mm256_epi8_value_1;

	__m256i values_left_shifted = _mm256_custom_shift_left_epi256(values_middle, 1);
	__m256i values_right_shifted = _mm256_custom_shift_right_epi256(values_middle, 1);

	__m256i values_left_right = _mm256_add_epi8(values_left_shifted, values_right_shifted);
	__m256i values_left_middle_right = _mm256_add_epi8(values_left_right, values_middle);

	__m256i prev_row_neighbour_count = _mm256_setzero_si256();
	__m256i current_row_neighbour_count = values_left_right;
	__m256i next_row_neighbour_count = values_left_middle_right;

	prev_row_neighbour_count = current_row_neighbour_count;
	current_row_neighbour_count = next_row_neighbour_count;
	next_row_neighbour_count = _mm256_setzero_si256();

	for (int r = 1; r < Chunk::rows; r++) {
		current_row_cells_data = _mm256_load_si256(&cells_data_ptr[r]);
		values_middle = _mm256_blendv_epi8(_mm256_setzero_si256(), _mm256_epi8_value_1, current_row_cells_data);

		values_left_shifted = _mm256_custom_shift_left_epi256(values_middle, 1);
		values_right_shifted = _mm256_custom_shift_right_epi256(values_middle, 1);

		values_left_right = _mm256_add_epi8(values_left_shifted, values_right_shifted);
		values_left_middle_right = _mm256_add_epi8(values_left_right, values_middle);

		prev_row_neighbour_count = _mm256_add_epi8(prev_row_neighbour_count, values_left_middle_right);
		current_row_neighbour_count = _mm256_add_epi8(current_row_neighbour_count, values_left_right);
		next_row_neighbour_count = _mm256_add_epi8(next_row_neighbour_count, values_left_middle_right);

		_mm256_store_si256(&neighbour_count_data_ptr[r - 1], prev_row_neighbour_count);

		prev_row_neighbour_count = current_row_neighbour_count;
		current_row_neighbour_count = next_row_neighbour_count;
		next_row_neighbour_count = _mm256_setzero_si256();
	}
	// store last row
	_mm256_store_si256(&neighbour_count_data_ptr[rows - 1], prev_row_neighbour_count);
}

__m256i _mm256_custom_shift_left_epi256(__m256i a, const int imm8) {
	int left_correction_value = _mm256_extract_epi8(a, 15);
	__m256i left_shift_correction = _mm256_set_epi32(0, 0, 0, left_correction_value, 0, 0, 0, 0);
	__m256i left_shift = _mm256_bslli_epi128(a, 1);
	__m256i values_left_shifted = _mm256_or_si256(left_shift, left_shift_correction);
	return values_left_shifted;
}

__m256i _mm256_custom_shift_right_epi256(__m256i a, const int imm8) {
	int right_correction_value = _mm256_extract_epi8(a, 16) << 24;
	__m256i right_shift_correction = _mm256_set_epi32(0, 0, 0, 0, right_correction_value, 0, 0, 0);
	__m256i right_shift = _mm256_bsrli_epi128(a, 1);
	__m256i values_right_shifted = _mm256_or_si256(right_shift, right_shift_correction);
	return values_right_shifted;
}

void Chunk::update_cells() {
	ZoneScoped;

	// assume that Chunk::columns = 32, so that a single row is exactly 256 bits big.
	__m256i* cells_data_ptr = (__m256i*) &cells_data[0];
	__m256i* neighbour_count_data_ptr = (__m256i*) &neighbour_count_data[0];

	__m256i _mm256_epi8_equal_to_0x02_mask = _mm256_set_epi8(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
	__m256i _mm256_epi8_equal_to_0x03_mask = _mm256_set_epi8(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3);
	has_alive_cells = false;
	for (int r = 0; r < Chunk::rows; r++) {
		__m256i neighbour_count_row = _mm256_load_si256(&neighbour_count_data_ptr[r]);
		__m256i cells_data_row = _mm256_load_si256(&cells_data_ptr[r]);

		__m256i neighbour_count_equal_to_2 = _mm256_cmpeq_epi8(neighbour_count_row, _mm256_epi8_equal_to_0x02_mask);
		__m256i neighbour_count_equal_to_3 = _mm256_cmpeq_epi8(neighbour_count_row, _mm256_epi8_equal_to_0x03_mask);
		__m256i neighbour_count_equal_to_2_or_3 = _mm256_or_si256(neighbour_count_equal_to_2, neighbour_count_equal_to_3);

		__m256i mask_cells_alive_and_neighbour_count_is_2_or_3 = _mm256_blendv_epi8(_mm256_setzero_si256(), neighbour_count_equal_to_2_or_3, cells_data_row);
		__m256i mask_cells_dead_and_neighbour_count_is_3 = _mm256_blendv_epi8(neighbour_count_equal_to_3, _mm256_setzero_si256(), cells_data_row);

		__m256i new_row = _mm256_or_si256(mask_cells_alive_and_neighbour_count_is_2_or_3, mask_cells_dead_and_neighbour_count_is_3);

		_mm256_store_si256(&cells_data_ptr[r], new_row);

		bool is_zero_row = (bool) _mm256_testz_si256(new_row, new_row);
		has_alive_cells |= !is_zero_row;
	}
}
