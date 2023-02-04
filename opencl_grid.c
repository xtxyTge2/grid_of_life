kernel void update_cells(global bool *cells_result, global unsigned int *neighbour_count, global bool* cells) {
	bool cell = cells[get_global_id(0)];
	unsigned int count = neighbour_count[get_global_id(0)];
	
	if (cell) {
		if (count == 2 || count == 3) {
			cells_result[get_global_id(0)] = true;
		} else {
			cells_result[get_global_id(0)] = false;
		}
	} else {
		if (count == 3) {
			cells_result[get_global_id(0)] = true;
		} else {
			cells_result[get_global_id(0)] = false;
		}
	}
}