#pragma once

#include "camera.hpp"
#include "grid.hpp"


//--------------------------------------------------------------------------------
class State_Render_Data {
public:
	State_Render_Data();
	//--------------------------------------------------------------------------------
	// data
	Camera_Render_Data* camera_render_data;

	Grid_Render_Data* grid_render_data;
};
