#pragma once
//--------------------------------------------------------------------------------
#include "opengl.hpp"

//--------------------------------------------------------------------------------
class Cube {
public:
	Cube();

	glm::mat4 get_model_matrix();
	//--------------------------------------------------------------------------------
	// data
	glm::vec3 m_position;
	float m_angle;

	glm::mat4 cached_rotation_matrix;
};