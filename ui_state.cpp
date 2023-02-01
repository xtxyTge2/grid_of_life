#pragma once

//--------------------------------------------------------------------------------
#include "ui_state.hpp"
#include "imgui_internal.h"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
UI_State::UI_State() :
	grid_ui_controls_info({}),
	m_show_demo_window(false),
	m_show_grid_info(true),
	m_window(nullptr)
{}


//--------------------------------------------------------------------------------
UI_State::~UI_State() {
	ZoneScoped;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//--------------------------------------------------------------------------------
void UI_State::initialise(GLFWwindow* window) {
	ZoneScoped;

	m_window = window;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

//--------------------------------------------------------------------------------
bool UI_State::wants_to_capture_io() {
	ZoneScoped;

	ImGuiIO& io = ImGui::GetIO(); (void) io;
	return io.WantTextInput || io.WantSetMousePos || io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantSaveIniSettings || io.WantCaptureMouseUnlessPopupClose;
}

//--------------------------------------------------------------------------------
void UI_State::setup_ui_for_current_frame(Grid_Info grid_info) {
	ZoneScoped;

	float sz = ImGui::GetTextLineHeight();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 base_pos = viewport->Pos;

	ImVec2 menu_bar_size;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	// reset the ui controls structure!
	grid_ui_controls_info.button_type = GRID_NO_BUTTON_PRESSED;
	{
		// need to change name below as well if you change it here.
		ImGui::Begin("Grid controls window", NULL, window_flags);
		menu_bar_size = ImGui::GetWindowSize();
		
		if (ImGui::Button("Start/Stop")) {
			grid_ui_controls_info.button_type = GRID_START_STOP_BUTTON_PRESSED;
		}

		ImGui::SameLine();
		if (ImGui::Button("Next iteration")) {
			grid_ui_controls_info.button_type = GRID_NEXT_ITERATION_BUTTON_PRESSED;
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			grid_ui_controls_info.button_type = GRID_RESET_BUTTON_PRESSED;
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

		ImGui::Text("Grid iteration: %d", grid_info.iteration);
		ImGui::Text("Grid rows: %d, columns: %d", grid_info.rows, grid_info.columns);
		ImGui::Text("Origin row: %d, Origin column: %d", grid_info.origin_row, grid_info.origin_column);
		ImGui::Text("Number of alive cells: %d", grid_info.number_of_alive_cells);

		ImGui::SliderFloat("Grid update speed", &grid_ui_controls_info.grid_speed_slider_value, 1.0f, 100.0f);   
		
		ImGui::Checkbox("Demo Window", &m_show_demo_window);   

		ImGui::Checkbox("Show chunk borders", &grid_ui_controls_info.show_chunk_borders); 

		if (m_show_demo_window) {
			ImGui::ShowDemoWindow(&m_show_demo_window);
		}
		
		ImGui::End();
	}
}

void UI_State::update(Grid_Info grid_info) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	setup_ui_for_current_frame(grid_info);
}