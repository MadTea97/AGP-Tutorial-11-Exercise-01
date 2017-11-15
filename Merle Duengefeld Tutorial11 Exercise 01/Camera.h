#pragma once
#include <d3d11.h>
#include <math.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
class Camera
{
private:
	float m_x, m_y, m_z, m_dx, m_dz, m_camera_rotation;
	XMVECTOR m_position, m_lookat, m_up;

public: 
	Camera(float x, float y, float z, float rotation);
	void Rotate(float degrees);
	void Forward(float distance);
	void Strafe(float distance);
	XMMATRIX GetViewMatrix();

	//GET
	float GetX();
	float GetY();
	float GetZ();
};