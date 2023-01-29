#pragma once

#include "Tracy.hpp"
//#define EIGEN_NO_DEBUG

// I have TRACY_ENABLE defined in the preprocessor settings of visual studio, hence do not define it here again.
//#define TRACY_ENABLE

// General remark for tracy: Set the /Zi compiler flag in visual studio, or otherwise there will be a vs
// studio bug/feature regarding macro expansions. Otherwise the macros of Tracy, e.g. ZoneScoped, FrameMark wont work.

//--------------------------------------------------------------------------------
#include <iostream>
#include <vector>

#include "renderer.hpp"

#include "opengl.hpp"
#include "state.hpp"
#include "texture.hpp"
#include "ui_renderer.hpp"

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
Texture* load_texture(const std::string&, unsigned int&);
void register_and_load_textures(World_Renderer*);
GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height);
int main(int, char[]);

//--------------------------------------------------------------------------------
State* g_state = new State();
Renderer* g_renderer = new Renderer();
bool g_ui_captures_io = false;

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

//--------------------------------------------------------------------------------
GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height) {
	ZoneScoped;
	std::cout << "Hello, Sailor!" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "GridOfLife3D", NULL, NULL);

	set_input_callbacks(window);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return nullptr;
	}
	
	return window;
}

//--------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ZoneScoped;

	if (window == g_state->m_window) {
		g_state->framebuffer_size_callback(width, height);
	}
}

//--------------------------------------------------------------------------------
int main(int argc, char argv[]) {
	GLFWwindow* window = init_glfw_glad_and_create_window(1920, 1080);
	if (window == nullptr) {
		std::cout << "Failed to initialize glfw and create a window." << std::endl;
		return -1;
	}
	g_renderer->initialise(window);
	g_state->initialise(window);

	// ------------------------------------------------------------------
	double timer = 0.0f;
	double time_since_last_iteration = 0.0f;
	while (!glfwWindowShouldClose(g_state->m_window)) {	
		glfwPollEvents();
		UI_State* current_ui_state = g_renderer->ui_renderer->m_ui_state;

		g_ui_captures_io = g_renderer->ui_renderer->imgui_wants_to_capture_io();
		if (!g_ui_captures_io) {
			g_state->process_input();
		}


		// ------------------------------------------------------------------
		// update state
		
		g_state->update(current_ui_state->event_queue);
		//current_ui_state->event_queue.clear();

		/*
		time_since_last_iteration += g_state->m_timer->m_delta_time;
		bool grid_is_in_next_iteration = false;
		if (current_ui_state->m_grid_is_running) {
			float threshold = 1.0f / current_ui_state->m_grid_update_speed;
			if (time_since_last_iteration >= threshold) {
				g_state->grid->next_iteration();
				grid_is_in_next_iteration = true;
				time_since_last_iteration = 0.0f;
			}
		} else {
			if (current_ui_state->m_update_grid) {
				current_ui_state->m_update_grid = false;
				g_state->grid->next_iteration();
				grid_is_in_next_iteration = true;
			}
		}

		if (current_ui_state->m_grid_should_reset) {
			current_ui_state->m_grid_should_reset = false;
			current_ui_state->m_update_grid = false;
			current_ui_state->m_grid_is_running = false;
			g_state->create_new_grid();
		}
		*/
		// ------------------------------------------------------------------
		// draw everything
		State_Render_Data state_render_data = g_state->create_render_data();
		/*
		if (grid_is_in_next_iteration) {
			current_ui_state->add_cell_number((float) state_render_data.grid_render_data->grid_info.number_of_alive_cells);
		}
		*/
		g_renderer->render_frame(state_render_data);
		FrameMark;
	}
	
	glfwTerminate();
	return 0;
}
