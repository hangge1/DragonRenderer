
#include "camera.h"

#include "glm/ext.hpp"

Camera::Camera(float fovy, float aspect, float n, float f, const glm::vec3& top) 
{
	top_ = top;
	projection_matrix_ = glm::perspective(fovy, aspect, n, f);
	update();
}

Camera::~Camera() {}

void Camera::onRMouseDown(int x, int y) 
{
	is_mouse_moving_ = true;
	current_mouse_x_ = x;
	current_mouse_y_ = y;
}

void Camera::onRMouseUp(int x, int y) 
{
	is_mouse_moving_ = false;
}

void Camera::onMouseMove(int x, int y) 
{
	if (is_mouse_moving_) {
		int xOffset = x - current_mouse_x_;
		int yOffset = y - current_mouse_y_;

		current_mouse_x_ = x;
		current_mouse_y_ = y;

		pitch(-yOffset);
		yaw(xOffset);
	}
}

void Camera::onKeyDown(uint32_t key) 
{
	switch (key) {
	case KEY_W:
		move_state_ |= MOVE_FRONT;
		break;
	case KEY_A:
		move_state_ |= MOVE_LEFT;
		break;
	case KEY_S:
		move_state_ |= MOVE_BACK;
		break;
	case KEY_D:
		move_state_ |= MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::onKeyUp(uint32_t key) 
{
	switch (key) {
	case KEY_W:
		move_state_ &= ~MOVE_FRONT;
		break;
	case KEY_A:
		move_state_ &= ~MOVE_LEFT;
		break;
	case KEY_S:
		move_state_ &= ~MOVE_BACK;
		break;
	case KEY_D:
		move_state_ &= ~MOVE_RIGHT;
		break;
	default:
		break;
	}
}

void Camera::pitch(int yoffset) 
{
	pitch_ += yoffset * mouse_sensitivity_;

	if (pitch_ >= 89.0f)
	{
		pitch_ = 89.0f;
	}

	if (pitch_ <= -89.0f)
	{
		pitch_ = -89.0f;
	}

	front_.y = sin(glm::radians(pitch_));
	front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

	front_ = glm::normalize(front_);
}

void Camera::yaw(int xoffset) 
{
	yaw_ += xoffset * mouse_sensitivity_;

	front_.y = sin(glm::radians(pitch_));
	front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

	front_ = glm::normalize(front_);
}

void Camera::update() 
{
	glm::vec3 moveDirection = {0.0f, 0.0f, 0.0f};

	glm::vec3 front = front_;
	glm::vec3 right = glm::normalize(glm::cross(front_, top_));

	if (move_state_ & MOVE_FRONT) 
    {
		moveDirection += front;
	}

	if (move_state_ & MOVE_BACK) 
    {
		moveDirection += -front;
	}

	if (move_state_ & MOVE_LEFT) 
    {
		moveDirection += -right;
	}

	if (move_state_ & MOVE_RIGHT) 
    {
		moveDirection += right;
	}

	if (glm::length(moveDirection) != 0) 
    {
		moveDirection = glm::normalize(moveDirection);
		position_ += move_speed_ * moveDirection;
	}

	view_matrix_ = glm::lookAt(position_, position_ + front_, top_);
}
