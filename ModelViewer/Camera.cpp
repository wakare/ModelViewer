#include <mutex>
#include <memory>
#include "Camera.h"

Camera::Camera(Vector3 cameraPosition): m_fMoveSpeed(0.1f), m_fRotateSpeed(0.001f), m_cameraPosition({ 0, 0, 0 })
{
	SetCameraPosition(cameraPosition);
}

Camera::Camera() :m_fMoveSpeed(0.1f), m_fRotateSpeed(0.001f), m_cameraPosition({ 0, 0, 0 })
{
	SetCameraPosition(m_cameraPosition);
}

const Vector3& Camera::GetCameraPosition() const
{
	return m_cameraPosition;
}

const Transform& Camera::GetViewTransform() const
{
	return m_viewTransform;
}

void Camera::_setViewTransform(Vector3 look, Vector3 up, Vector3 right)
{
	look.Normalize();
	up.Normalize();
	right.Normalize();

	m_lookDirection		= look;
	m_upDirection		= up;
	m_rightDirection	= right;

	float Result[4][4] =
	{
		m_rightDirection.fX,		m_rightDirection.fY,		m_rightDirection.fZ,		-m_rightDirection * m_cameraPosition,
		m_upDirection.fX,			m_upDirection.fY,			m_upDirection.fZ,			-m_upDirection * m_cameraPosition,
		-m_lookDirection.fX,		-m_lookDirection.fY,		-m_lookDirection.fZ,		m_lookDirection * m_cameraPosition,
		0,							0,							0,							1,
	};

	m_viewTransform.SetTransformMatrix(Result);
}

void Camera::LookAtTarget(Vector3 targetPosition, Vector3 up)
{
	up.Normalize();

	Vector3 look = targetPosition - m_cameraPosition;
	look.Normalize();
	
	Vector3 right = look.Cross(up);

	_setViewTransform(look, up, right);
}

void Camera::SetCameraPosition(Vector3 cameraPosition)
{
	m_cameraPosition = cameraPosition;
}

void Camera::SelfMove(Vector3 deltaMovement)
{
	auto rightTranslate = m_rightDirection * deltaMovement.fX;
	auto upTranslate = m_upDirection * deltaMovement.fY;
	auto lookTranslate = m_lookDirection * deltaMovement.fZ;

	auto cameraTranslate = upTranslate + lookTranslate + rightTranslate;
	m_cameraPosition += cameraTranslate;

	LookAtTarget(m_cameraPosition + m_lookDirection, m_upDirection);
}

void Camera::SelfRotate(double deltaX, double deltaY)
{
	Vector3 rotateAxis;
	float fRotateAngle = 0.0f;

	Transform::GetSelfRotateInfo(deltaX, deltaY, m_fRotateSpeed, rotateAxis, fRotateAngle);

	Transform rotateTransform;
	rotateTransform.Rotate(fRotateAngle, rotateAxis);
	Matrix4 rotateMatrix = rotateTransform.GetTransformMatrix();

	Vector4 _lookVec = Vector4(m_lookDirection, 0.0f);
	Vector4 _rightVec = Vector4(m_rightDirection, 0.0f);
	Vector4 _upVec = Vector4(m_upDirection, 0.0f);

	m_lookDirection = rotateMatrix * _lookVec;
	m_upDirection = rotateMatrix * _upVec;
	m_rightDirection = rotateMatrix * _rightVec;

	LookAtTarget(m_cameraPosition + m_lookDirection, m_upDirection);
}
