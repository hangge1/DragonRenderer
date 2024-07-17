
#ifndef _RENDER_PerspectiveCamera_H_
#define _RENDER_PerspectiveCamera_H_

#include "abstract_camera.h"

class PerspectiveCamera : public AbstractCamera
{
public:
	PerspectiveCamera(float fovy, float aspect, float n, float f, const glm::vec3& top);
	virtual ~PerspectiveCamera() = default;

public:
	virtual void OnRightMouseDown(int x, int y) override;
	virtual void OnRightMouseUp(int x, int y) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnKeyDown(uint32_t key) override;
	virtual void OnKeyUp(uint32_t key) override;

	virtual void Update() override;

protected:
	void Pitch(int yoffset);

	void Yaw(int xoffset);

protected:
	//摄像机pitch张角
	float		pitch_angle_ { 0.0f };
	//摄像机yaw张角
	float		yaw_angle_ { -90.0f };
};

#endif