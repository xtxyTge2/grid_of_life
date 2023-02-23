#include "chunk.hpp"


Chunk::Chunk() :
	grid_coordinate_row(0),
grid_coordinate_column(0),
chunk_origin_row(0),
chunk_origin_column(0),
has_alive_cells(false),
cells_data({}),
neighbour_count_data({}),
coordinates_of_alive_cells({}),
number_of_alive_cells(0)
{
	ZoneScoped;
}

Chunk::Chunk(const Coordinate& coord, Coordinate origin_coord, const std::vector<std::pair<int, int>>& alive_cells_coordinates) :
	grid_coordinate_row(coord.x),
grid_coordinate_column(coord.y),
chunk_origin_row(origin_coord.x),
chunk_origin_column(origin_coord.y),
has_alive_cells(false),
cells_data({}),
neighbour_count_data({}),
coordinates_of_alive_cells({}),
number_of_alive_cells(0)
{
	ZoneScoped;

	has_alive_cells = alive_cells_coordinates.size() > 0;
	for (auto [r, c]: alive_cells_coordinates) {
		cells_data[r*Chunk::rows + c] = 0xFF;
	}
}


Coordinate Chunk::transform_to_world_coordinate(Coordinate chunk_coord) {
	ZoneScoped;

	return Coordinate(chunk_coord.x + chunk_origin_row, chunk_coord.y + chunk_origin_column);
}


void Chunk::update_neighbour_count_left_side(const std::array<unsigned char, Chunk::rows>& data) {
	ZoneScoped;

	if (data[0]) {
		neighbour_count_data[0]++;
		neighbour_count_data[Chunk::rows]++;
	}


	for (int r = 1; r < Chunk::rows - 1; r++) {
		if (data[r]) {
			neighbour_count_data[(r-1)*Chunk::rows]++;
			neighbour_count_data[r*Chunk::rows]++;
			neighbour_count_data[(r + 1)*Chunk::rows]++;
		}
	}

	if (data[Chunk::rows - 1]) {
		neighbour_count_data[(Chunk::rows - 2)*Chunk::rows]++;
		neighbour_count_data[(Chunk::rows - 1)*Chunk::rows]++;
	}
}

void Chunk::update_neighbour_count_right_side(const std::array<unsigned char, Chunk::rows>& data) {
	ZoneScoped;

	if (data[0]) {
		neighbour_count_data[0 + Chunk::columns - 1]++;
		neighbour_count_data[Chunk::rows + Chunk::columns - 1]++;
	}

	for (int r = 1; r < Chunk::rows - 1; r++) {
		if (data[r]) {
			neighbour_count_data[(r-1)*Chunk::rows + Chunk::columns - 1]++;
			neighbour_count_data[r*Chunk::rows + Chunk::columns - 1]++;
			neighbour_count_data[(r + 1)*Chunk::rows + Chunk::columns - 1]++;
		}
	}

	if (data[Chunk::rows - 1]) {
		neighbour_count_data[(Chunk::rows - 2)*Chunk::rows + Chunk::columns - 1]++;
		neighbour_count_data[(Chunk::rows - 1)*Chunk::rows + Chunk::columns - 1]++;
	}
}

void Chunk::update_neighbour_count_top_side(const std::array<unsigned char, Chunk::columns>& data) {
	ZoneScoped;

	const long long value_1 = 0x0101010101010101;
	__m256i _mm256_epi8_value_1 = _mm256_set_epi64x(value_1, value_1, value_1, value_1);
	
	// here we assume Chunk::columns == 32, so that the array contains exactly 32 * 8 = 256 bits and fit into the __m256i registers.
	__m256i const* cells_data_ptr = (__m256i const *) data.data();
	__m256i cells_data_row = _mm256_loadu_si256(cells_data_ptr);

	__m256i const* neighbour_count_data_ptr = (__m256i const *) neighbour_count_data.data();
	__m256i neighbour_count_data_row = _mm256_loadu_si256(neighbour_count_data_ptr);

	__m256i values_middle = _mm256_blendv_epi8(_mm256_setzero_si256(), _mm256_epi8_value_1, cells_data_row);

	__m256i values_left_shifted = _mm256_custom_shift_left_epi256(values_middle, 1);
	__m256i values_right_shifted = _mm256_custom_shift_right_epi256(values_middle, 1);

	__m256i values_left_right_added = _mm256_add_epi8(values_left_shifted, values_right_shifted);
	__m256i values_left_middle_right_added = _mm256_add_epi8(values_left_right_added, values_middle);

	neighbour_count_data_row = _mm256_add_epi8(neighbour_count_data_row, values_left_middle_right_added);
	_mm256_store_si256((__m256i *)neighbour_count_data_ptr, neighbour_count_data_row);
}

void Chunk::update_neighbour_count_bottom_side(const std::array<unsigned char, Chunk::columns>& data) {
	ZoneScoped;

	const long long value_1 = 0x0101010101010101;
	__m256i _mm256_epi8_value_1 = _mm256_set_epi64x(value_1, value_1, value_1, value_1);
	
	// here we assume Chunk::columns == 32, so that the array contains exactly 32 * 8 = 256 bits and fit into the __m256i registers.
	__m256i const* cells_data_ptr = (__m256i const *) data.data();
	__m256i cells_data_row = _mm256_loadu_si256(cells_data_ptr);

	__m256i const* neighbour_count_data_ptr = (__m256i const *) neighbour_count_data.data();

	__m256i neighbour_count_data_row = _mm256_loadu_si256((__m256i const*) &neighbour_count_data_ptr[Chunk::rows - 1]);

	__m256i values_middle = _mm256_blendv_epi8(_mm256_setzero_si256(), _mm256_epi8_value_1, cells_data_row);

	__m256i values_left_shifted = _mm256_custom_shift_left_epi256(values_middle, 1);
	__m256i values_right_shifted = _mm256_custom_shift_right_epi256(values_middle, 1);

	__m256i values_left_right_added = _mm256_add_epi8(values_left_shifted, values_right_shifted);
	__m256i values_left_middle_right_added = _mm256_add_epi8(values_left_right_added, values_middle);

	neighbour_count_data_row = _mm256_add_epi8(neighbour_count_data_row, values_left_middle_right_added);
	_mm256_store_si256((__m256i *)&neighbour_count_data_ptr[Chunk::rows - 1], neighbour_count_data_row);
}

void Chunk::update_neighbour_count_top_left_corner() {
	ZoneScoped;
	neighbour_count_data[0]++;
}

void Chunk::update_neighbour_count_top_right_corner() {
	ZoneScoped;
	neighbour_count_data[Chunk::columns - 1]++;
}


void Chunk::update_neighbour_count_bottom_left_corner() {
	ZoneScoped;
	neighbour_count_data[(Chunk::rows - 1)* Chunk::rows]++;
}


void Chunk::update_neighbour_count_bottom_right_corner() {
	ZoneScoped;
	neighbour_count_data[(Chunk::rows - 1)* Chunk::rows + Chunk::columns - 1]++;
}

void Chunk::update_neighbour_count_inside() {
	ZoneScoped;

	const long long value_1 = 0x0101010101010101;
	__m256i _mm256_epi8_value_1 = _mm256_set_epi64x(value_1, value_1, value_1, value_1);
	__m256i* cells_data_ptr = (__m256i*) &cells_data[0];
	__m256i* neighbour_count_data_ptr = (__m256i*) &neighbour_count_data[0];

	__m256i prev_row_neighbour_count = _mm256_setzero_si256();
	__m256i current_row_neighbour_count = _mm256_setzero_si256();
	__m256i next_row_neighbour_count = _mm256_setzero_si256();
	int r = 0;
	while (r < Chunk::rows) {
		__m256i current_row_cells_data = _mm256_load_si256(&cells_data_ptr[r]);
		__m256i values_middle = _mm256_blendv_epi8(_mm256_setzero_si256(), _mm256_epi8_value_1, current_row_cells_data);

		__m256i values_left_shifted = _mm256_custom_shift_left_epi256(values_middle, 1);
		__m256i values_right_shifted = _mm256_custom_shift_right_epi256(values_middle, 1);

		__m256i values_left_right_added = _mm256_add_epi8(values_left_shifted, values_right_shifted);
		__m256i values_left_middle_right_added = _mm256_add_epi8(values_left_right_added, values_middle);

		prev_row_neighbour_count = _mm256_add_epi8(prev_row_neighbour_count, values_left_middle_right_added);
		current_row_neighbour_count = _mm256_add_epi8(current_row_neighbour_count, values_left_right_added);
		next_row_neighbour_count = _mm256_add_epi8(next_row_neighbour_count, values_left_middle_right_added);

		if (r > 0) {
			_mm256_store_si256(&neighbour_count_data_ptr[r - 1], prev_row_neighbour_count);
		}

		prev_row_neighbour_count = current_row_neighbour_count;
		current_row_neighbour_count = next_row_neighbour_count;
		next_row_neighbour_count = _mm256_setzero_si256();
		
		r++;
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

bool _mm256_is_zero(__m256i a) {
	return (bool) _mm256_testz_si256(a, a);
}


void Chunk::update_cells() {
	ZoneScoped;

	// assume that Chunk::columns = 32, so that a single row is exactly 256 bits big.
	__m256i* cells_data_ptr = (__m256i*) &cells_data[0];
	__m256i* neighbour_count_data_ptr = (__m256i*) &neighbour_count_data[0];

	const long long value_2 = 0x0202020202020202;
	__m256i _mm256_epi8_equal_to_0x02_mask = _mm256_set_epi64x(value_2, value_2, value_2, value_2);
	const long long value_3 = 0x0303030303030303;
	__m256i _mm256_epi8_equal_to_0x03_mask = _mm256_set_epi64x(value_3, value_3, value_3, value_3);
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

		has_alive_cells |= !_mm256_is_zero(new_row);
	}
}


void Chunk::update_coordinates_of_alive_cells() {
	ZoneScoped; 

	number_of_alive_cells = 0;
	for (int r = 0; r < Chunk::rows; ++r) {
		for (int c = 0; c < Chunk::columns; ++c) {
			if (cells_data[r * Chunk::rows + c]) {
				int x = c + chunk_origin_column;
				int y = -(r + chunk_origin_row);
				coordinates_of_alive_cells[number_of_alive_cells++] = std::make_pair(x, y);
			}
		}
	}
}

