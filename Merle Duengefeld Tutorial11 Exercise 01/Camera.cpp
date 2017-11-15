#include "Camera.h"

Camera::Camera(float x, float y, float z, float rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = rotation;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
	
}
void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;
	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
}
void Camera::Forward(float distance)
{
	m_x += distance * m_dx;
	m_z += distance * m_dz;
}
void Camera::Strafe(float distance)
{
	XMVECTOR strafeVector = XMVector3Cross((m_position - m_lookat), (m_up - m_position));

	m_x += strafeVector.x * distance;
	m_z += strafeVector.z * distance;
}
XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y , m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}
//GET
float Camera::GetX()
{
	return m_x;
}
float Camera::GetY()
{
	return m_y;
}
float Camera::GetZ()
{
	return m_z;
}