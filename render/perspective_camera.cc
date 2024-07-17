#include "perspective_camera.h"

#include "glm/ext.hpp"

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float n, float f, const glm::vec3& top)
{
	top_ = top;
	projection_matrix_ = glm::perspective(fovy, aspect, n, f);
	Update();
}

void PerspectiveCamera::OnRightMouseDown(int x, int y) 
{
	is_mouse_moving_ = true;
	last_mouse_x_ = x;
	last_mouse_y_ = y;
}

void PerspectiveCamera::OnRightMouseUp(int x, int y) 
{
	is_mouse_moving_ = false;
}

void PerspectiveCamera::OnMouseMove(int x, int y) 
{
	if (is_mouse_moving_) 
	{
		int xOffset = x - last_mouse_x_;
		int yOffset = y - last_mouse_y_;

		last_mouse_x_ = x;
		last_mouse_y_ = y;

		Pitch(-yOffset);
		Yaw(xOffset);
	}
}

void PerspectiveCamera::OnKeyDown(uint32_t key) 
{
	switch (key) 
	{
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

void PerspectiveCamera::OnKeyUp(uint32_t key) 
{
	switch (key) 
	{
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

void PerspectiveCamera::Pitch(int yoffset) 
{
	pitch_angle_ += yoffset * mouse_sensitivity_factor_;

	if (pitch_angle_ >= 89.0f)
	{
		pitch_angle_ = 89.0f;
	}

	if (pitch_angle_ <= -89.0f)
	{
		pitch_angle_ = -89.0f;
	}

	front_.y = sin(glm::radians(pitch_angle_));
	front_.x = cos(glm::radians(yaw_angle_)) * cos(glm::radians(pitch_angle_));
	front_.z = sin(glm::radians(yaw_angle_)) * cos(glm::radians(pitch_angle_));

	front_ = glm::normalize(front_);
}

void PerspectiveCamera::Yaw(int xoffset) 
{
	yaw_angle_ += xoffset * mouse_sensitivity_factor_;

	front_.y = sin(glm::radians(pitch_angle_));
	front_.x = cos(glm::radians(yaw_angle_)) * cos(glm::radians(pitch_angle_));
	front_.z = sin(glm::radians(yaw_angle_)) * cos(glm::radians(pitch_angle_));

	front_ = glm::normalize(front_);
}

void PerspectiveCamera::Update() 
{
	glm::vec3 moveDirection = { 0.0f, 0.0f, 0.0f };

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

	if (moveDirection != glm::vec3(0.0, 0.0, 0.0)) 
    {
		moveDirection = glm::normalize(moveDirection);
		position_ += move_speed_factor_ * moveDirection;
	}

	view_matrix_ = glm::lookAt(position_, position_ + front_, top_);
}
