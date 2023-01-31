#pragma once

#include "world.hpp"

//--------------------------------------------------------------------------------
class World_Render_Data {
public:
	World_Render_Data();

	void update(std::shared_ptr<World> world_ptr);
	//--------------------------------------------------------------------------------
	// data
	Camera_Render_Data* camera_render_data;

	Grid_Render_Data* grid_render_data;
};