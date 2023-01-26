#pragma once

#include "Tracy.hpp"
//--------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <unordered_map>
#include <array>

#include <Eigen/Core>
#include "opengl.hpp"
#include "camera.hpp"
#include "shader_program.hpp"
#include "read.hpp"
#include "texture.hpp"
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
//--------------------------------------------------------------------------------
class State_Render_Data {
public:
	State_Render_Data();
	//--------------------------------------------------------------------------------
	// data
	Camera_Render_Data* camera_render_data;

	Grid_Render_Data* grid_render_data;
};

//--------------------------------------------------------------------------------
class Timer {
public:
	Timer();

	void update();
	//--------------------------------------------------------------------------------
	// data
	float m_delta_time;
	float m_last_frame_time;
};

//--------------------------------------------------------------------------------
class State {
public:
	State();

	void initialise(GLFWwindow*);

	void update();

	void process_input();
	
	void framebuffer_size_callback(int, int);

	State_Render_Data create_render_data();

	void reset_grid();
	//--------------------------------------------------------------------------------
	// data
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Mouse> m_mouse;
	GLFWwindow* m_window;

	std::unique_ptr<Timer> m_timer;
	
	Grid* grid;
};