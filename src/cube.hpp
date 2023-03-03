#pragma once
//--------------------------------------------------------------------------------
#include <Tracy/Tracy.hpp>

#include "opengl.hpp"

//--------------------------------------------------------------------------------
class Cube {
public:
	Cube();

	Cube(glm::vec3 pos, float angle);

	const glm::mat4 compute_model_matrix_no_rotation() const;
	
	const glm::mat4 compute_model_matrix_with_rotation() const;
	//--------------------------------------------------------------------------------
	// data
	glm::vec3 m_position;
	float m_angle;
};