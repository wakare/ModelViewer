#include "Transform.h"
#include <assert.h>

Transform::Transform()
{
	float matrix[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Init standard matrix4
	m_transformMatrix = matrix;
}

void Transform::Rotate(float fRotateAngle, Vector3 Axis)
{
	Axis.Normalize();

	const float cosAngle	= cosf(fRotateAngle);
	const float sinAngle	= sinf(fRotateAngle);
	const Vector3 temp		= Axis * (1.0f - cosAngle);

	float Rotate[3][3] =
	{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};

	Rotate[0][0] = cosAngle + temp.fX * Axis.fX;
	Rotate[0][1] = temp.fX * Axis.fY + sinAngle * Axis.fZ;
	Rotate[0][2] = temp.fX * Axis.fZ - sinAngle * Axis.fY;

	Rotate[1][0] = temp.fY * Axis.fX - sinAngle * Axis.fZ;
	Rotate[1][1] = cosAngle + temp.fY * Axis.fY;
	Rotate[1][2] = temp.fY * Axis.fZ + sinAngle * Axis.fX;

	Rotate[2][0] = temp.fZ * Axis.fX + sinAngle * Axis.fY;
	Rotate[2][1] = temp.fZ * Axis.fY - sinAngle * Axis.fX;
	Rotate[2][2] = cosAngle + temp.fZ * Axis.fZ;

	auto VectorA = m_transformMatrix[0] * Rotate[0][0] + m_transformMatrix[1] * Rotate[0][1] + m_transformMatrix[2] * Rotate[0][2];
	auto VectorB = m_transformMatrix[0] * Rotate[1][0] + m_transformMatrix[1] * Rotate[1][1] + m_transformMatrix[2] * Rotate[1][2];
	auto VectorC = m_transformMatrix[0] * Rotate[2][0] + m_transformMatrix[1] * Rotate[2][1] + m_transformMatrix[2] * Rotate[2][2];
	auto VectorD = m_transformMatrix[3];

	float Result[4][4] = 
	{
		VectorA.fX, VectorA.fY, VectorA.fZ, VectorA.fW,
		VectorB.fX, VectorB.fY, VectorB.fZ, VectorB.fW,
		VectorC.fX, VectorC.fY, VectorC.fZ, VectorC.fW,
		VectorD.fX, VectorD.fY, VectorD.fZ, VectorD.fW,
	};

	m_transformMatrix = Result;
}

void Transform::Translate(float fX, float fY, float fZ)
{
	auto VectorD =
		m_transformMatrix[0] * fX +
		m_transformMatrix[1] * fY +
		m_transformMatrix[2] * fZ +
		m_transformMatrix[3];

	m_transformMatrix.m_fMatrixArray[0][3] = VectorD.fX;
	m_transformMatrix.m_fMatrixArray[1][3] = VectorD.fY;
	m_transformMatrix.m_fMatrixArray[2][3] = VectorD.fZ;
	m_transformMatrix.m_fMatrixArray[3][3] = VectorD.fW;
}

void Transform::Scale(float fX, float fY, float fZ)
{
	auto vectorA = m_transformMatrix[0] * fX;
	auto vectorB = m_transformMatrix[1] * fY;
	auto vectorC = m_transformMatrix[2] * fZ;
	auto vectorD = m_transformMatrix[3];

	float Result[4][4] = 
	{
		vectorA.fX, vectorA.fY, vectorA.fZ, vectorA.fW,
		vectorB.fX, vectorB.fY, vectorB.fZ, vectorB.fW,
		vectorC.fX, vectorC.fY, vectorC.fZ, vectorC.fW,
		vectorD.fX, vectorD.fY, vectorD.fZ, vectorD.fW
	};

	m_transformMatrix = Result;
}

const Transform Transform::operator* (const Transform& otherTransform) const
{
	Transform resultTransform;
	resultTransform.m_transformMatrix = this->m_transformMatrix;
	resultTransform.m_transformMatrix *= otherTransform.m_transformMatrix;

	return resultTransform;
}

const Transform& Transform::operator*= (const Transform& otherTransform)
{
	m_transformMatrix = m_transformMatrix * otherTransform.m_transformMatrix;
	return (*this);
}

void Transform::SetTransformMatrix(Matrix4 matrix)
{
	m_transformMatrix = matrix;
}

void Transform::SetTransformMatrix(float matrix[16])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_transformMatrix.m_fMatrixArray[i][j] = matrix[i * 4 + j];
		}
	}
	
}

Matrix4 Transform::GetTransformMatrix() const
{
	return m_transformMatrix;
}

void Transform::GetSelfRotateInfo(double deltaX, double deltaY, float fRotateSpeed, Vector3 & outRotateAxis, float& outfRotateAngle)
{
	float fRotateRadiusX = static_cast<float>(deltaX) * fRotateSpeed;
	float fRotateRadiusY = static_cast<float>(deltaY) * fRotateSpeed;

	const float fSinX = sinf(fRotateRadiusX);
	const float fSinY = sinf(fRotateRadiusY);
	const float fCosX = cosf(fRotateRadiusX);
	const float fCosY = cosf(fRotateRadiusY);

	float fCoordX = fSinX * fCosY;
	float fCoordY = fSinY * fCosX;
	float fCoordZ = 1 - fCoordX * fCoordX - fCoordY * fCoordY;

	Vector3 rotatedPosition = { fCoordX, fCoordY, fCoordZ };
	Vector3 originalPosition = { 0.0f, 0.0f, 1.0f };
	rotatedPosition.Normalize();

	// Calculate vector cross to get rotateAxis 
	Vector3 rotateAxis = rotatedPosition.Cross(originalPosition);
	rotateAxis.Normalize();

	float fCosRotateAngle = rotatedPosition * originalPosition;
	float fRotateAngle = acosf(fCosRotateAngle);

	outRotateAxis = rotateAxis;
	outfRotateAngle = fRotateAngle;
}

// Set Projection matrix (fov --> radians)
bool Transform::SetProjectionTransform(float fov, float aspect, float nearZ, float farZ)
{
	assert(0.0f < nearZ && nearZ < farZ);
	auto tanHalfFovy = tanf(fov * 0.5f);

	float Result[4][4] = { 0 };
	Result[0][0] = 1.0f / (aspect * tanHalfFovy);
	Result[1][1] = 1.0f / (tanHalfFovy);
	Result[2][2] = -(farZ + nearZ) / (farZ - nearZ);
	Result[2][3] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
	Result[3][2] = -1.0f;

	m_transformMatrix = Result;
	return true;
}