#pragma once

#include <Eigen/Core>
#include "cube.hpp"
#include "ui_state.hpp"
#include <unordered_set>

class Coordinate {
public:
	Coordinate(int a_x, int a_y) : 
		x(a_x), 
		y(a_y) 
	{}

	~Coordinate() 
	{}

	bool operator == (const Coordinate& rhs) const{
		return x == rhs.x && y == rhs.y;
	}

	bool operator != (const Coordinate& rhs) const{
		return !(*this == rhs);
	}

	int x;
	int y;
};

namespace std
{
	template<>
	struct hash<Coordinate> {
		std::size_t operator()(Coordinate const &coord) const noexcept {
			return (51 + std::hash<int>()(coord.x) + 51 * std::hash<int>()(coord.y));
		}
	};
}

class Chunk {
public:
	Chunk(Coordinate coord);

	void print_chunk();

	void update_neighbour_count_inside();

	void update_neighbour_count_top();

	void update_neighbour_count_bottom();

	void update_neighbour_count_left();

	void update_neighbour_count_right();

	void update_neighbour_count_corners();

	void clear_neighbour_update_info();
	
	void update_cells();

	void update_neighbour_count_and_set_info();

	void update_chunk_coordinates();

	bool has_to_update_right();

	bool has_to_update_left();

	bool has_to_update_top();

	bool has_to_update_bottom();

	bool has_to_update_corners();
	
	bool has_to_update_neighbours();

	Coordinate transform_to_world_coordinate(Coordinate chunk_coord);

	constexpr static int rows = 16;
	constexpr static int columns = 16;

	int grid_coordinate_row;
	int grid_coordinate_column;

	int chunk_origin_row;
	int chunk_origin_column;
	int number_of_alive_cells;

	Eigen::Array < bool, rows, columns > cells;
	Eigen::Array < unsigned int, rows, columns > neighbour_count;

	std::unordered_set<Coordinate> chunk_coordinates;
	std::vector<Coordinate> border_coordinates;

	std::vector<int> left_row_indices_to_update;
	std::vector<int> right_row_indices_to_update;
	std::vector<int> top_column_indices_to_update;
	std::vector<int> bottom_column_indices_to_update;

	bool has_to_update_top_left_corner;
	bool has_to_update_top_right_corner;
	bool has_to_update_bottom_right_corner;
	bool has_to_update_bottom_left_corner;
};

//--------------------------------------------------------------------------------
class Grid {
public:
	Grid();

	void create_new_chunk_and_set_alive_cells(Coordinate coord, std::vector<std::pair<int, int>> coordinates);

	void update_cells_of_all_chunks();

	void remove_empty_chunks();

	void print_all_chunks_info();
;
	void create_new_chunk(Coordinate coord);

	void update();

	void update_neighbours_of_chunk(std::shared_ptr<Chunk> chunk);

	void next_iteration();
	
	void update_all_neighbours_of_all_chunks();

	void create_all_needed_neighbour_chunks();
	//--------------------------------------------------------------------------------
	// data
	int number_of_alive_cells;
	int iteration;

	std::unordered_map<Coordinate, std::shared_ptr<Chunk>> chunk_map;
};

//--------------------------------------------------------------------------------
struct Grid_Execution_State {
	bool is_running = true;
	bool run_manual_next_iteration = false;
	float time_since_last_iteration = 0.0f;
	float grid_speed = 1.0f;
	bool show_chunk_borders = false;
	bool have_to_update_chunk_borders = false;
};

//--------------------------------------------------------------------------------
class Grid_Manager {
public:
	Grid_Manager();

	void update(double dt, Grid_UI_Controls_Info grid_ui_controls_info);

	void update_grid_execution_state(Grid_UI_Controls_Info ui_info);

	void create_new_grid();

	void update_coordinates_for_alive_grid_cells();

	void update_coordinates_for_chunk_borders();

	Grid_Info get_grid_info();

	std::unordered_set<Coordinate> world_coordinates;
	std::vector<Coordinate> border_coordinates;
	std::unique_ptr<Grid> grid;
	Grid_Execution_State grid_execution_state;
};