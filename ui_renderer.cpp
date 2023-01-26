#pragma once

//--------------------------------------------------------------------------------
#include "ui_renderer.h"
#include "imgui_internal.h"
#include "Tracy.hpp"


//--------------------------------------------------------------------------------
UI_Renderer::UI_Renderer() :
	m_window(nullptr),
m_ui_state(nullptr)
{}

//--------------------------------------------------------------------------------
UI_Renderer::~UI_Renderer() {
	ZoneScoped;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//--------------------------------------------------------------------------------
void UI_Renderer::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	m_ui_state = new UI_State();
}

//--------------------------------------------------------------------------------
bool UI_Renderer::imgui_wants_to_capture_io() {
	ZoneScoped;
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	return io.WantTextInput || io.WantSetMousePos || io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantSaveIniSettings || io.WantCaptureMouseUnlessPopupClose;
}

//--------------------------------------------------------------------------------
void UI_Renderer::render_frame(State_Render_Data& state_render_data) {
	ZoneScoped;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_ui_state->update_render_data(state_render_data);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------------------------------
UI_State::UI_State() :
	m_grid_is_running(false),
m_grid_update_speed(50.0f),
m_update_grid(false),
m_show_demo_window(false),
m_grid_should_reset(false),
m_show_grid_info(true),
m_current_index(0)
{}

//--------------------------------------------------------------------------------
void UI_State::update_render_data(State_Render_Data& state_render_data) {
	ZoneScoped;
	float sz = ImGui::GetTextLineHeight();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 base_pos = viewport->Pos;

	ImVec2 menu_bar_size;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	{
		// need to change name below as well if you change it here.
		ImGui::Begin("Grid controls window", NULL, window_flags);
		menu_bar_size = ImGui::GetWindowSize();

		const char* start_stop_button_text;
		if (m_grid_is_running) {
			start_stop_button_text = "Start/Stop";
		} else {
			start_stop_button_text = "Start/Stop";
		}
		
		if (ImGui::Button(start_stop_button_text)) {
			m_grid_is_running = !m_grid_is_running;
		}

		ImGui::SameLine();
		if (ImGui::Button("Next iteration")) {
			m_update_grid = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			m_grid_should_reset = true;
		}

		ImGui::SameLine();
		const char* show_or_hide_grid_info_button_text;
		if (m_show_grid_info) {
			show_or_hide_grid_info_button_text = "Hide Grid info";
		} else {
			show_or_hide_grid_info_button_text = "Show Grid info";
		}
		if (ImGui::Button(show_or_hide_grid_info_button_text)) {
			m_show_grid_info = !m_show_grid_info;
		}

		float window_width = ImGui::GetWindowWidth();
		
		float viewport_middle_x = viewport->Size.x / 2.0f;
		float window_position_x = viewport_middle_x - window_width / 2.0f;

		ImGuiWindow* grid_controls_window = ImGui::FindWindowByName("Grid controls window"); // this is the __current__ window we define here.
		ImGui::SetWindowPos(grid_controls_window, ImVec2(window_position_x, base_pos.y));
		ImGui::End();
	}
	
	ImGui::SetNextWindowPos(ImVec2(base_pos.x, base_pos.y), ImGuiCond_Always);

	// Grid info window
	if (m_show_grid_info) {
		if (!ImGui::Begin("Grid info", &m_show_grid_info, window_flags)) {
			// user has closed grid info window
			// so we early out here.
			ImGui::End();
			return;
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		// Grid info
		Grid_Info grid_info = state_render_data.grid_render_data->grid_info;

		ImGui::Text("Grid iteration: %d", grid_info.iteration);
		ImGui::Text("Grid rows: %d, columns: %d", grid_info.rows, grid_info.columns);
		ImGui::Text("Origin row: %d, Origin column: %d", grid_info.origin_row, grid_info.origin_column);
		ImGui::Text("Number of alive cells: %d", grid_info.number_of_alive_cells);

		ImGui::SliderFloat("Grid update speed", &m_grid_update_speed, 1.0f, 100.0f);   
	
		ImGui::Checkbox("Demo Window", &m_show_demo_window);   

		if (m_show_demo_window) {
			ImGui::ShowDemoWindow(&m_show_demo_window);
		}
		
		ImGui::PlotLines("Lines", m_cell_number_values, IM_ARRAYSIZE(m_cell_number_values));
		ImGui::End();
	}
}

//--------------------------------------------------------------------------------
void UI_State::add_cell_number(float value) {
	ZoneScoped;
	m_cell_number_values[m_current_index] = value;
	m_current_index++;
	m_current_index = m_current_index % m_cell_number_values_size;
}