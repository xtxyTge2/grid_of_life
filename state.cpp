#pragma once

//--------------------------------------------------------------------------------
#include "state.hpp"
#include "Tracy.hpp"


inline static void increment_if_valid_index(int r, int c, int rows, int columns, Eigen::Matrix < unsigned int, Eigen::Dynamic, Eigen::Dynamic > & m);

//--------------------------------------------------------------------------------
State::State() :
	grid(nullptr),
m_camera(nullptr),
m_mouse(nullptr),
m_window(nullptr),
m_timer(nullptr)
{

}

//--------------------------------------------------------------------------------
void State::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;

	m_timer = std::make_unique < Timer > ();

	m_mouse = std::make_unique < Mouse > ();
	m_mouse->initialise(0.0f, 0.0f);

	m_camera = std::make_unique < Camera > ();
	m_camera->position = glm::vec3(10.0f, 10.0f, 100.0f);

	// left/right/x-axis direction vector
	m_camera->orientation_vector_matrix[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// up/down/y-axis direction vector
	m_camera->orientation_vector_matrix[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	
	// front/back/z-axis direction vector
	m_camera->orientation_vector_matrix[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	
	m_camera->m_speed = 50.0f;
	
	reset_grid();
}


//--------------------------------------------------------------------------------
void State::update() {
	ZoneScoped;
	m_timer->update();
}

//--------------------------------------------------------------------------------
void State::framebuffer_size_callback(int width, int height) {
	ZoneScoped;
	glViewport(0, 0, width, height);
}

//--------------------------------------------------------------------------------
void State::process_input() {
	ZoneScoped;
	m_camera->m_speed = 50.0f * m_timer->m_delta_time;

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, true);
	}
	
	Camera_Move_Direction direction = UNDEFINED;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		direction = Camera_Move_Direction::FORWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		direction = Camera_Move_Direction::BACKWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		direction = Camera_Move_Direction::LEFT;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		direction = Camera_Move_Direction::RIGHT;
	}
	m_camera->move(direction);
}

//--------------------------------------------------------------------------------
void State::reset_grid() {
	ZoneScoped;
	int rows = 20;
	int columns = 20;
	
	int origin_row = 10;
	int origin_column = 10;

	grid = new Grid(rows, columns, origin_row, origin_column);

	grid->cells(origin_row, origin_column) = true;
	grid->cells(origin_row + 1, origin_column) = true;
	grid->cells(origin_row + 1, origin_column - 1) = true;
	grid->cells(origin_row + 1, origin_column - 2) = true;
	grid->cells(origin_row + 2, origin_column - 1) = true;

	grid->cells(origin_row, origin_column - 4) = true;
	grid->cells(origin_row, origin_column - 5) = true;
	grid->cells(origin_row, origin_column - 6) = true;
	grid->cells(origin_row + 1, origin_column - 5) = true;

	grid->update_neighbour_count();
}

//--------------------------------------------------------------------------------
State_Render_Data::State_Render_Data() {

}

//--------------------------------------------------------------------------------
State_Render_Data State::create_render_data() {
	ZoneScoped;
	State_Render_Data* render_data = new State_Render_Data();

	// Create camera render data
	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);
	render_data->camera_render_data = m_camera->create_render_data(window_width, window_height);

	// Create grid render data
	render_data->grid_render_data = grid->create_render_data();

	return *render_data;
}

//--------------------------------------------------------------------------------
Grid::Grid(int r, int c, int origin_r, int origin_c) : rows(r), columns(c), origin_row(origin_r), origin_column(origin_c), iteration(0) {
	ZoneScoped;
	cells.resize(rows, columns);
	cells.setConstant(false);
	neighbour_count.resize(rows, columns);
	neighbour_count.setConstant(0);
}

//--------------------------------------------------------------------------------
std::vector<Cube> Grid::create_cubes_for_alive_grid_cells() {
	ZoneScoped;
	std::vector<Cube> cubes;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				Cube* cube = new Cube();
				cube->m_position = glm::vec3((float) (origin_column - c), (float) (origin_row - r), -3.0f);
				cube->m_angle = 0.0f;
	
				cubes.push_back(*cube);
			}
		}
	}
	return cubes;
}

//--------------------------------------------------------------------------------
void Grid::update() {
	
}

//--------------------------------------------------------------------------------
void Grid::next_iteration() {
	ZoneScoped;

	iteration++;
	resize_if_needed();

	update_neighbour_count();

	// TODO: split this up and handle interior and border of the grid individually. If we do that we can incorporate the resize_if_needed() call into the border case computation
	for (size_t r = 0; r < rows; r++) {
		for (size_t c = 0; c < columns; c++) {
			unsigned int count = neighbour_count(r, c);
			if (cells(r, c)) {
				// a cell that is alive stays alive iff it has two or three neighbouring alive cells.
				if (count == 2 || count == 3) {
					cells(r, c) = true;
				} else {
					cells(r, c) = false;
				}
			} else {
				// a dead cell becomes alive exactly iff it has three neighbouring alive cells.
				if (count == 3) {
					cells(r, c) = true;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
void Grid::update_neighbour_count() {
	ZoneScoped;
	// @Speed: just memset to zero ?
	neighbour_count.setConstant(0);

	number_of_alive_cells = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			if (cells(r, c)) {
				number_of_alive_cells++;

				increment_if_valid_index(r - 1, c - 1, rows, columns, neighbour_count);
				increment_if_valid_index(r - 1, c, rows, columns, neighbour_count);
				increment_if_valid_index(r - 1, c + 1, rows, columns, neighbour_count);
				increment_if_valid_index(r, c - 1, rows, columns, neighbour_count);
				// dont increment urself.
				//neighbour_count->increment_if_valid_index(r, c);
				increment_if_valid_index(r, c + 1, rows, columns, neighbour_count);
				increment_if_valid_index(r + 1, c - 1, rows, columns, neighbour_count);
				increment_if_valid_index(r + 1, c, rows, columns, neighbour_count);
				increment_if_valid_index(r + 1, c + 1, rows, columns, neighbour_count);
			}
		}
	}
}

inline static void increment_if_valid_index(int r, int c, int rows, int columns, Eigen::Matrix < unsigned int, Eigen::Dynamic, Eigen::Dynamic > & m) {
	if (r < 0 || r > rows - 1 || c < 0 || c > columns - 1) return;
	m(r, c) += 1;
}

//--------------------------------------------------------------------------------
void Grid::resize_if_needed() {
	ZoneScoped;
	bool has_to_resize = false;
	for (int c = 0; c < columns; c++) {
		has_to_resize |= cells(0, c) || cells(rows - 1, c);
		if (has_to_resize) break;
	}
	for (int r = 0; r < rows; r++) {
		has_to_resize |= cells(r, 0) || cells(r, columns - 1);
		if (has_to_resize) break;
	}
	if (!has_to_resize) return;

	int new_rows = 3 * rows;
	int new_columns = 3 * columns;

	int row_offset = rows;
	int column_offset = columns;

	int new_origin_row = origin_row + row_offset;
	int new_origin_column = origin_column + column_offset;

	Eigen::Matrix < bool, Eigen::Dynamic, Eigen::Dynamic > new_cells(new_rows, new_columns);
	new_cells.setConstant(false);

	Eigen::Matrix < unsigned int, Eigen::Dynamic, Eigen::Dynamic > new_neighbour_count(new_rows, new_columns);
	new_neighbour_count.setConstant(0);

	// TODO faster
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			bool old_value = cells(r, c);
			int new_r = r + row_offset;
			int new_c = c + column_offset;
			new_cells(new_r, new_c) = old_value;
		}
	}

	rows = new_rows;
	columns = new_columns;

	origin_row = new_origin_row;
	origin_column = new_origin_column;

	cells = new_cells;
	neighbour_count = new_neighbour_count;
	std::cout << "resized grid. new rows: " << rows << ", new columns: " << columns << "\n";
	update_neighbour_count();
}

//--------------------------------------------------------------------------------
Grid_Render_Data* Grid::create_render_data() {
	ZoneScoped;
	Grid_Render_Data* grid_render_data = new Grid_Render_Data();
	
	// fill out grid info
	grid_render_data->grid_info = {};
	grid_render_data->grid_info.iteration = iteration;
	grid_render_data->grid_info.rows = rows;
	grid_render_data->grid_info.columns = columns;
	grid_render_data->grid_info.origin_row = origin_row;
	grid_render_data->grid_info.origin_column = origin_column;
	grid_render_data->grid_info.number_of_alive_cells = number_of_alive_cells;

	// cubes render data for each alive cell of the grid.
	std::vector<Cube> cubes = create_cubes_for_alive_grid_cells();
	for (Cube& cube: cubes) {
		Cube_Render_Data* cube_render_data = cube.create_render_data();
		grid_render_data->cubes_render_data.push_back(*cube_render_data);
	}
	return grid_render_data;
};

//--------------------------------------------------------------------------------
Grid_Render_Data::Grid_Render_Data() {
	
}

/*
//--------------------------------------------------------------------------------
template<typename T>
Matrix<T>::Matrix(size_t r, size_t c) : rows(r), columns(c), data({}) {
	data.reserve(rows * columns);
}

//--------------------------------------------------------------------------------
template<typename T>
Matrix<T>::Matrix(size_t r, size_t c, T default_value): rows(r), columns(c), data({}) {
	ZoneScoped;
	data.reserve(rows * columns);
	for (size_t r = 0; r < rows; r++) {
		for (size_t c = 0; c < columns; c++) {
			data.push_back(default_value);
		}
	}
}

//--------------------------------------------------------------------------------
template<typename T>
inline size_t Matrix<T>::index(size_t r, size_t c) {
	ZoneScoped;
	return r * columns + c;
}

//--------------------------------------------------------------------------------
template<typename T>
inline T Matrix<T>::get(size_t r, size_t c) {
	ZoneScoped;
	return data[index(r, c)];
}

//--------------------------------------------------------------------------------
template<typename T>
inline void Matrix<T>::set(size_t r, size_t c, T value) {
	ZoneScoped;
	data[index(r, c)] = value;
}

//--------------------------------------------------------------------------------
template<typename T>
void Matrix<T>::increment_if_valid_index(size_t r, size_t c) {
	ZoneScoped;
	if (r < 0 || r >= rows || c < 0 || c >= columns) return;
	T prev_value = get(r, c);
	set(r, c, prev_value + 1);
}
*/
//--------------------------------------------------------------------------------
Timer::Timer() :
	m_delta_time(0.0f),
m_last_frame_time(0.0f)
{

}

//--------------------------------------------------------------------------------
void Timer::update() {
	ZoneScoped;
	float m_current_frame_time = (float) glfwGetTime();
	m_delta_time = m_current_frame_time - m_last_frame_time;
	m_last_frame_time = m_current_frame_time;
}