#include "camera.hpp"


//--------------------------------------------------------------------------------
float clip(float value, float lower, float higher) {
	ZoneScoped;
	assert(lower <= higher); 

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
Camera::Camera() :
	position(glm::vec3(0.0f)),
	target_position(glm::vec3(0.0f)),
	fov(50.0f),
	direction(Camera_Move_Direction::NO_DIRECTION)
{}


//--------------------------------------------------------------------------------
glm::mat4 Camera::get_view_matrix() {
	ZoneScoped;

	glm::vec3 front_direction = glm::normalize(target_position - position);
	glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);

	return glm::lookAt(position, position + front_direction, up_direction);
}


//--------------------------------------------------------------------------------
glm::mat4 Camera::get_projection_matrix(int viewport_width, int viewport_height) {
	ZoneScoped;

	return glm::perspective(glm::radians(fov), static_cast<float>(viewport_width) / static_cast<float>(viewport_height), 0.3f, 10000.0f);
}

void Camera::rotate_around_y_axis(double dt) {
	// convert to polar coordinates
	double radius = sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
	// angle in radians
	double theta = atan2(position.z, position.x);

	double base_rotation_speed = 1.0f;

	// calculate change in angle
	double dtheta = base_rotation_speed * dt;
	// update angle, radius stays the same
	theta += dtheta;

	// convert position back to cartesian coordinates
	position.x = radius * cos(theta);
	position.z = radius * sin(theta);
}


//--------------------------------------------------------------------------------
void Camera::move_in_current_direction(double dt) {
	ZoneScoped;

	float movement_speed = 100.0f * static_cast<float>(dt);
	glm::vec3 dposition = glm::vec3(0.0f);

	glm::vec3 front_direction = glm::normalize(target_position - position);
	glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);

	// minus so its left instead of right ( arbitrary choice ).
	glm::vec3 left_direction = -glm::normalize(glm::cross(up_direction, front_direction));

	switch (direction) {
		case Camera_Move_Direction::NO_DIRECTION:
			break;
		case Camera_Move_Direction::FORWARD:
			dposition = movement_speed * front_direction;
			break;
		case Camera_Move_Direction::BACKWARD:
			dposition = -movement_speed * front_direction;
			break;
		case Camera_Move_Direction::LEFT:
			dposition = -movement_speed * left_direction;
			break;
		case Camera_Move_Direction::RIGHT:
			dposition = movement_speed * left_direction;
			break;
		case Camera_Move_Direction::ROTATE_AROUND_Y_AXIS:
			rotate_around_y_axis(dt);
			break;
		default:
			break;
	}

	// if we move with wasd-keys we also have to move the target position for desired behaviour, in the sense that
	// we do not want to rotate our view but just move laterally. If we do not update target_position we would rotate
	// our view, since we would recalculate our front direction with the unchanged target_position.
	position += dposition;
	target_position += dposition;
}

//--------------------------------------------------------------------------------
void Camera::add_offset_and_clip_fov(float yoffset) {
	fov = clip(fov + yoffset, 1.0f, 110.0f);
}

void Camera::update(double dt) {
	
	move_in_current_direction(dt);

	// reset direction after moving, if we are not rotating
	if (direction != Camera_Move_Direction::ROTATE_AROUND_Y_AXIS) {
		direction = Camera_Move_Direction::NO_DIRECTION;
	}
}


//---------------------------------------------------a-----------------------------
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