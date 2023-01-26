#pragma once
//--------------------------------------------------------------------------------

#include "camera.h"
#include <iostream>


//--------------------------------------------------------------------------------
float clip(float value, float lower, float higher) {
	//assert(lower <= higher);

	// dont do anything if lower > higher.
	if (lower > higher) return value;

	float result = value;
	if (value <= lower) {
		result = lower;
	}
	// result contains max(value, lower) at this point.

	if (result >= higher) {
		result = higher;
	}
	// result = max(result, higher) at this point.

	return result;
}

//--------------------------------------------------------------------------------
static void print_mat3(glm::mat3 matrix) {
	std::cout << "matrix: \n";
	for (int r = 0; r < 3; r++) {
		std::cout << matrix[r].x << " " << matrix[r].y << " " << matrix[r].z << "\n";
	}
}

//--------------------------------------------------------------------------------
Camera::Camera() :
	position(glm::vec3(0.0f, 0.0f, 0.0f)),
	m_speed(0.0f),
	orientation_vector_matrix(glm::mat3(1.0f))
{}

//--------------------------------------------------------------------------------
Camera::~Camera() {
	
}


//--------------------------------------------------------------------------------
glm::mat4 Camera::get_view_matrix() {
	glm::vec3 up_direction = orientation_vector_matrix[1];
	glm::vec3 front_direction = orientation_vector_matrix[2];
	return glm::lookAt(position, position + front_direction, up_direction);
}

//--------------------------------------------------------------------------------
glm::mat4 Camera::get_projection_matrix(int viewport_width, int viewport_height) {
	return glm::perspective(glm::radians(50.0f), ((float)viewport_width) / ((float) viewport_height), 0.3f, 10000.0f);
}

//--------------------------------------------------------------------------------
void Camera::move(Camera_Move_Direction direction) {
	glm::vec3 left_direction = orientation_vector_matrix[0];
	glm::vec3 front_direction = orientation_vector_matrix[2];
	switch (direction) {
		case FORWARD:
			position += m_speed * front_direction;
			break;
		case BACKWARD:
			position -= m_speed * front_direction;
			break;
		case LEFT:
			position -= m_speed * left_direction;
			break;
		case RIGHT:
			position += m_speed * left_direction;
			break;
		case UNDEFINED:
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------------------------
Camera_Render_Data* Camera::create_render_data(int viewport_width, int viewport_height) {
	Camera_Render_Data* render_data = new Camera_Render_Data();

	render_data->model = glm::mat4(1.0f);
	render_data->view = get_view_matrix();
	render_data->projection = get_projection_matrix(viewport_width, viewport_height);

	return render_data;
}

//--------------------------------------------------------------------------------
Camera_Render_Data::Camera_Render_Data() {

}

//--------------------------------------------------------------------------------
Camera_Render_Data::~Camera_Render_Data() {

}