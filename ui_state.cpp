#pragma once

//--------------------------------------------------------------------------------
#include "ui_state.hpp"


//--------------------------------------------------------------------------------
UI_State::UI_State() :
	ui_info({}),
	m_window(nullptr)
{
}


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
void UI_State::setup_ui_for_current_frame(const Grid_Info& grid_info) {
	ZoneScoped;

	float sz = ImGui::GetTextLineHeight();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 base_pos = viewport->Pos;

	ImVec2 menu_bar_size;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	// reset the ui controls structure!
	ui_info.button_type = GRID_NO_BUTTON_PRESSED;
	{
		// need to change name below as well if you change it here.
		ImGui::Begin("Grid controls window", NULL, window_flags);
		menu_bar_size = ImGui::GetWindowSize();
		
		if (ImGui::Button("Start/Stop")) {
			ui_info.button_type = GRID_START_STOP_BUTTON_PRESSED;
		}

		ImGui::SameLine();
		if (ImGui::Button("Next iteration")) {
			ui_info.button_type = GRID_NEXT_ITERATION_BUTTON_PRESSED;
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			ui_info.button_type = GRID_RESET_BUTTON_PRESSED;
		}

		ImGui::SameLine();
		const char* show_or_hide_grid_info_button_text;
		if (ui_info.m_show_grid_info) {
			show_or_hide_grid_info_button_text = "Hide Grid info";
		} else {
			show_or_hide_grid_info_button_text = "Show Grid info";
		}
		if (ImGui::Button(show_or_hide_grid_info_button_text)) {
			ui_info.m_show_grid_info = !ui_info.m_show_grid_info;
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
	if (ui_info.m_show_grid_info) {
		if (!ImGui::Begin("Grid info", &ui_info.m_show_grid_info, window_flags)) {
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

		ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;
		slider_flags |= ImGuiSliderFlags_NoInput;

		bool grid_speed_slider_value_changed = ImGui::SliderFloat("Grid update speed", &ui_info.grid_speed_slider_value, ui_info.min_grid_speed_slider_value, ui_info.max_grid_speed_slider_value, NULL, slider_flags);   
		
		bool show_demo_window_checkbox_changed = ImGui::Checkbox("Demo Window", &ui_info.m_show_demo_window);   

		bool show_chunk_borders_checkbox_changed = ImGui::Checkbox("Show chunk borders", &ui_info.show_chunk_borders); 

		bool run_grid_at_max_possible_speed_checkbox_changed = ImGui::Checkbox("Run simulation at maximal speed", &ui_info.run_grid_at_max_possible_speed);

		bool number_of_grid_iterations_per_single_frame_slider_changed = ImGui::SliderInt(
			"Update screen every %d iterations", 
			&ui_info.number_of_grid_iterations_per_single_frame, ui_info.min_number_of_grid_iterations_per_single_frame, ui_info.max_number_of_grid_iterations_per_single_frame, "%d Iterations per frame", slider_flags);

		if (ui_info.m_show_demo_window) {
			ImGui::ShowDemoWindow(&ui_info.m_show_demo_window);
		}

		// ui logic handling here, ie different sliders/checkbox interact with eachother/ are coupled.


		// handle all ui control state changes

		// handle each state change separately first and then at the end do a state synchronization, ie if max speed slider is set to max then also set the slider to max and vice versa ( this is needed for the first iteration, ie if we didnt change any slider/checkbox but still want to synchronize them if their default values warrant it).
		if (grid_speed_slider_value_changed) {
			// if the user changed the speed slider and decreased the value from the max value then we set the run at max speed checkbox to false and set the number of iterations per frame to its minimum value (which for now is 1 iteration per frame).
			if (ui_info.grid_speed_slider_value < ui_info.max_grid_speed_slider_value) {
				ui_info.run_grid_at_max_possible_speed = false;
				ui_info.number_of_grid_iterations_per_single_frame = ui_info.min_number_of_grid_iterations_per_single_frame;
			}
		}
		if (run_grid_at_max_possible_speed_checkbox_changed) {
			if (ui_info.run_grid_at_max_possible_speed) {
				// set slider to max if the user ticks the max speed checkbox to true
				ui_info.grid_speed_slider_value = ui_info.max_grid_speed_slider_value;
			}
		}
		if (number_of_grid_iterations_per_single_frame_slider_changed) {
			if (ui_info.number_of_grid_iterations_per_single_frame > ui_info.min_number_of_grid_iterations_per_single_frame) {
				// set simulation to max speed when we want to run more than a single iteration per frame, we only allow it like this.
				ui_info.grid_speed_slider_value = ui_info.max_grid_speed_slider_value;
				ui_info.run_grid_at_max_possible_speed = true;
			} else {
				//assert(ui_info.ui_info.number_of_grid_iterations_per_single_frame == ui_info.min_number_of_grid_iterations_per_single_frame);
			}
		}

		// ui controls synchronization

		// synchronize the controls explicitetly again, this is for robustness to sync all controls, mainly for the case of initial ui settings values and
		if (ui_info.run_grid_at_max_possible_speed || ui_info.grid_speed_slider_value == ui_info.max_grid_speed_slider_value || ui_info.number_of_grid_iterations_per_single_frame > ui_info.min_number_of_grid_iterations_per_single_frame) {
			// if the user set the speed to max, either via the speed slider or via the run at max speed checkbox, or if the use set the number of iterations per frame to higher than the minimum (which is 1 iteration per frame), then we set all the other settings to their max value as well
			ui_info.run_grid_at_max_possible_speed = true;
			ui_info.grid_speed_slider_value = ui_info.max_grid_speed_slider_value;
		}

		ImGui::End();
		
	}
}

void UI_State::update(const Grid_Info& grid_info) {
	ZoneScoped;
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	setup_ui_for_current_frame(grid_info);
}