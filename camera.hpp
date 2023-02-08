#pragma once

//--------------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

//--------------------------------------------------------------------------------
enum class Camera_Move_Direction {
	UNDEFINED,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//--------------------------------------------------------------------------------
class Camera {
public:
	Camera();

	void initialise();

	void move(Camera_Move_Direction);

	glm::mat4 get_view_matrix();

	glm::mat4 get_projection_matrix(int viewport_width, int viewport_height);
	//--------------------------------------------------------------------------------
	glm::vec3 position;
	glm::mat3 orientation_vector_matrix;
	float m_speed;
};

//--------------------------------------------------------------------------------
class Mouse {
public:
	Mouse();

	void initialise(float x, float y);

	void update();
 
	//--------------------------------------------------------------------------------
	float m_x;
	float m_y;
};
