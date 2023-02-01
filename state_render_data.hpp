#pragma once

#include "world.hpp"

class Cube_System {
public:
	Cube_System();

	void update(Grid_Manager* grid_manager);

	void clear();

	Cube* create_new_cube();
	//--------------------------------------------------------------------------------
	// data
	int current_number_of_cubes;
	constexpr static int MAX_NUMBER_OF_CUBES = 50000;
	std::array<Cube, MAX_NUMBER_OF_CUBES> cubes_array;

	Grid_Render_Data* grid_render_data;
};


//--------------------------------------------------------------------------------
class World_Render_Data {
public:
	World_Render_Data();

	void update(std::shared_ptr<World> world_ptr, std::shared_ptr<Cube_System> cube_system);
	//--------------------------------------------------------------------------------
	// data
	Camera_Render_Data* camera_render_data;

	Grid_Render_Data* grid_render_data;
};