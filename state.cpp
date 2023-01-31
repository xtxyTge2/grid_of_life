#pragma once

//--------------------------------------------------------------------------------
#include "state.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
Timer::Timer() :
	dt(0.0f),
m_last_frame_time(0.0f)
{

}

//--------------------------------------------------------------------------------
void Timer::update() {
	ZoneScoped;
	float m_current_frame_time = (float) glfwGetTime();
	dt = m_current_frame_time - m_last_frame_time;
	m_last_frame_time = m_current_frame_time;
}


//--------------------------------------------------------------------------------
State::State() : window(nullptr), timer(nullptr), ui_state(nullptr), renderer(nullptr),
world(nullptr)
{
	timer = std::make_unique < Timer > ();

	ui_state = std::make_unique < UI_State > ();

	renderer = std::make_unique < Renderer > ();

	world = std::make_shared < World > ();

	render_data = std::make_unique < World_Render_Data > ();
}

//--------------------------------------------------------------------------------
void State::update() {
	glfwPollEvents();

	timer->update();

	double dt = timer->dt;

	//world_render_data->update();

	world->update(dt, ui_state->grid_ui_controls_info);

	render_data->update(world);
	// nocheckin
	//World_Render_Data render_data = world->create_render_data();
	// this sets up a new IMGUI-Frame! But we call the imgui render function only in our renderer! We always have to call ui_state->update() before the imgui render function,otherwise imgui didnt start a new frame!
	ui_state->update(render_data->grid_render_data->grid_info);
}

//--------------------------------------------------------------------------------
void State::initialise(GLFWwindow* w) {
	window = w;

	renderer->initialise(window);
	world->initialise(window);
	ui_state->initialise(window);
}

//--------------------------------------------------------------------------------
void State::framebuffer_size_callback(int width, int height) {
	ZoneScoped;
	glViewport(0, 0, width, height);
}


void State::render_frame() {
	renderer->render_frame(*render_data);
}

bool State::should_quit() {
	//assert(window);
	return glfwWindowShouldClose(window);
}


World_Render_Data::World_Render_Data() {

}

//--------------------------------------------------------------------------------
void World_Render_Data::update(std::shared_ptr<World> world_ptr) {
	ZoneScoped;

	// Create camera render data
	int window_width, window_height;
	glfwGetWindowSize(world_ptr->m_window, &window_width, &window_height);
	camera_render_data = world_ptr->m_camera->create_render_data(window_width, window_height);

	// Create grid render data
	grid_render_data = world_ptr->grid_manager->create_render_data();
}