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

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
Texture* load_texture(const std::string&, unsigned int&);
void register_and_load_textures(Renderer*);
GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height);
int main(int, char[]);

//--------------------------------------------------------------------------------
State* g_state = new State();

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

	if (window == g_state->window) {
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
	g_state->initialise(window);

	// ------------------------------------------------------------------
	while (!g_state->should_quit()) {	
		g_state->update();

		g_state->render_frame();

		FrameMark;
	}
	
	glfwTerminate();
	return 0;
}
