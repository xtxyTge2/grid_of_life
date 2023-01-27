#pragma once

#include <Eigen/Core>
#include "cube.hpp"



/*
//--------------------------------------------------------------------------------
template <typename T>
class Matrix {
public:
	Matrix(size_t r, size_t c);

	Matrix(size_t r, size_t c, T default_value);

	size_t index(size_t r, size_t c);

	T get(size_t r, size_t c);

	void set(size_t, size_t c, T value); 
	
	void increment_if_valid_index(size_t r, size_t c);
	//--------------------------------------------------------------------------------
	// data
	size_t rows;
	size_t columns;

	std::vector<T> data;
};
*/

//--------------------------------------------------------------------------------
struct Grid_Info {
	int rows;
	int columns;

	int origin_row; 
	int origin_column;
	int number_of_alive_cells;

	int iteration;
};

//--------------------------------------------------------------------------------
class Grid_Render_Data {
public:
	Grid_Render_Data();

	Grid_Info grid_info;
	std::vector<Cube_Render_Data> cubes_render_data;
};

//--------------------------------------------------------------------------------
class Grid {
public:
	Grid(int r, int c, int origin_r, int origin_c);

	~Grid();

	std::vector<Cube> create_cubes_for_alive_grid_cells();

	void update_neighbour_count();

	void update();

	Grid_Render_Data* create_render_data();

	void next_iteration();

	void resize_if_needed();
	//--------------------------------------------------------------------------------
	// data

	int rows;
	int columns;

	int origin_row; 
	int origin_column;
	
	//Matrix<bool>* cells;
	//Matrix<unsigned int>* neighbour_count;
	Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> cells;
	Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> neighbour_count;

	int number_of_alive_cells;
	int iteration;
};