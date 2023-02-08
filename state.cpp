#include "state.hpp"


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
	ZoneScoped;
	timer = std::make_unique < Timer > ();

	ui_state = std::make_unique < UI_State > ();

	renderer = std::make_unique < Renderer > ();

	world = std::make_shared < World > ();

	cube_system = std::make_shared < Cube_System > ();
}

//--------------------------------------------------------------------------------
void State::update() {
	ZoneScoped;

	glfwPollEvents();

	timer->update();

	double dt = timer->dt;

	world->update(dt, ui_state->ui_info);

	cube_system->update();

	// this sets up a new IMGUI-Frame! But we call the imgui render function only in our renderer! We always have to call ui_state->update() before the imgui render function,otherwise imgui didnt start a new frame!

	ui_state->update(*world->grid_manager->grid_info);

	renderer->render_frame(world, cube_system);
}

//--------------------------------------------------------------------------------
void State::initialise(GLFWwindow* w) {
	ZoneScoped;
	window = w;

	renderer->initialise(window);
	world->initialise(window);
	ui_state->initialise(window);
	cube_system->initialise(world->grid_manager);
}

//--------------------------------------------------------------------------------
void State::framebuffer_size_callback(int width, int height) {
	ZoneScoped;
	glViewport(0, 0, width, height);
}


bool State::should_quit() {
	ZoneScoped;
	//assert(window);
	return glfwWindowShouldClose(window) || world->grid_manager->grid->iteration > 10000;
}