#pragma once

#include <vector>
#include <iostream>
#include <unordered_map>
#include <array>

#include "opengl.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "grid.hpp"
#include "ui_state.hpp"


//--------------------------------------------------------------------------------
class World {
public:
	World();

	void initialise(GLFWwindow*);

	void update(double dt, Grid_UI_Controls_Info grid_ui_controls_info);

	void process_input(double dt);
	
	void framebuffer_size_callback(int, int);
	//--------------------------------------------------------------------------------
	// data
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Mouse> m_mouse;
	GLFWwindow* m_window;
	
	Grid_Manager* grid_manager;
};
