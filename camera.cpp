#pragma once
//--------------------------------------------------------------------------------

#include "camera.hpp"
#include <iostream>
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
float clip(float value, float lower, float higher) {
	ZoneScoped;
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
	ZoneScoped;
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
glm::mat4 Camera::get_view_matrix() {
	ZoneScoped;
	glm::vec3 up_direction = orientation_vector_matrix[1];
	glm::vec3 front_direction = orientation_vector_matrix[2];
	return glm::lookAt(position, position + front_direction, up_direction);
}

//--------------------------------------------------------------------------------
glm::mat4 Camera::get_projection_matrix(int viewport_width, int viewport_height) {
	ZoneScoped;
	return glm::perspective(glm::radians(50.0f), ((float)viewport_width) / ((float) viewport_height), 0.3f, 10000.0f);
}

//--------------------------------------------------------------------------------
void Camera::move(Camera_Move_Direction direction) {
	ZoneScoped;
	glm::vec3 left_direction = orientation_vector_matrix[0];
	glm::vec3 front_direction = orientation_vector_matrix[2];
	switch (direction) {
		case Camera_Move_Direction::FORWARD:
			position += m_speed * front_direction;
			break;
		case Camera_Move_Direction::BACKWARD:
			position -= m_speed * front_direction;
			break;
		case Camera_Move_Direction::LEFT:
			position -= m_speed * left_direction;
			break;
		case Camera_Move_Direction::RIGHT:
			position += m_speed * left_direction;
			break;
		case Camera_Move_Direction::UNDEFINED:
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------------------------
Mouse::Mouse() :
	m_x(0.0f),
	m_y(0.0f)
{

}

//--------------------------------------------------------------------------------
void Mouse::initialise(float x, float y) {
	m_x = x;
	m_y = y;
}

void Mouse::update() {
	
}