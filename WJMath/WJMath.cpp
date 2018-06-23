#include "WJMath.h"
#include <math.h>
#include <assert.h>

inline int lineInterpolate(int start, int end, float k)
{
	return (int)(start + k*(end - start) + 0.5f);
}

float Vector4::length()
{
	float length = fX*fX + fY*fY + fZ*fZ + fW * fW;
	length = sqrtf(length);

	return length;
}

void Vector4::DivideW()
{
	fX = fX / fW;
	fY = fY / fW;
	fZ = fZ / fW;
	fW = fW	/ fW;
}

Vector4 Vector4::Cross(const Vector4& _V)
{
	// only use for direction vector
	Vector4 result;

	result.fW = 0;
	result.fX = fY*_V.fZ - fZ*_V.fY;
	result.fY = fZ*_V.fX - fX*_V.fZ;
	result.fZ = fX*_V.fY - fY*_V.fX;

	return result;
}

Vector4::Vector4(Vector3 vector3, float W)
{
	fX = vector3.fX;
	fY = vector3.fY;
	fZ = vector3.fZ;
	fW = W;
}

Vector4 & Vector4::operator*=(float f)
{
	fX *= f;
	fY *= f;
	fZ *= f;
	fW *= f;

	return *this;
}

Vector4 & Vector4::operator=(const Vector4& v)
{
	fX = v.fX;
	fY = v.fY;
	fZ = v.fZ;
	fW = v.fW;

	return *this;
}

Vector4 Vector4::operator*(const Matrix4& _M)
{
	Vector4 result;

	result.fX = fX*_M.m_fMatrixArray[0][0] + fY*_M.m_fMatrixArray[1][0] + fZ*_M.m_fMatrixArray[2][0] + fW*_M.m_fMatrixArray[3][0];
	result.fY = fX*_M.m_fMatrixArray[0][1] + fY*_M.m_fMatrixArray[1][1] + fZ*_M.m_fMatrixArray[2][1] + fW*_M.m_fMatrixArray[3][1];
	result.fZ = fX*_M.m_fMatrixArray[0][2] + fY*_M.m_fMatrixArray[1][2] + fZ*_M.m_fMatrixArray[2][2] + fW*_M.m_fMatrixArray[3][2];
	result.fW = fX*_M.m_fMatrixArray[0][3] + fY*_M.m_fMatrixArray[1][3] + fZ*_M.m_fMatrixArray[2][3] + fW*_M.m_fMatrixArray[3][3];
	
	return result;
}

Vector4 & Vector4::operator*=(const Matrix4& _M)
{
	float _x = fX, _y = fY, _z = fZ, _w = fW;

	fX = _x*_M.m_fMatrixArray[0][0] + _y*_M.m_fMatrixArray[1][0] + _z*_M.m_fMatrixArray[2][0] + _w*_M.m_fMatrixArray[3][0];
	fY = _x*_M.m_fMatrixArray[0][1] + _y*_M.m_fMatrixArray[1][1] + _z*_M.m_fMatrixArray[2][1] + _w*_M.m_fMatrixArray[3][1];
	fZ = _x*_M.m_fMatrixArray[0][2] + _y*_M.m_fMatrixArray[1][2] + _z*_M.m_fMatrixArray[2][2] + _w*_M.m_fMatrixArray[3][2];
	fW = _x*_M.m_fMatrixArray[0][3] + _y*_M.m_fMatrixArray[1][3] + _z*_M.m_fMatrixArray[2][3] + _w*_M.m_fMatrixArray[3][3];
	
	return *(this);
}

Vector4 & Vector4::operator+=(const Vector4& v)
{
	fX += v.fX;
	fY += v.fY;
	fZ += v.fZ;
	fW += v.fW;

	return *this;
}

Vector4 & Vector4::operator/=(float f)
{
	float rhf = 1.0f / f;

	fX *= rhf;
	fY *= rhf;
	fZ *= rhf;
	fW *= rhf;

	return *this;

}

Vector4 Vector4::operator*(float f)
{
	Vector4 result;

	result.fX = fX * f;
	result.fY = fY * f;
	result.fZ = fZ * f;
	result.fW = fW * f;

	return result;
}

Vector4 Vector4::operator+(const Vector4& v)
{
	Vector4 result;

	result.fX = fX + v.fX;
	result.fY = fY + v.fY;
	result.fZ = fZ + v.fZ;
	result.fW = fW + v.fW;

	return result;
}

float Vector4::operator*(Vector4 _V)
{
	return (fX*_V.fX + fY * _V.fY + fZ * _V.fZ + fW * _V.fW);
}

Vector4 Vector4::operator- (const Vector4& v)
{
	Vector4 result;

	result.fX = fX - v.fX;
	result.fY = fY - v.fY;
	result.fZ = fZ - v.fZ;
	result.fW = fW - v.fW;

	return result;
}

Vector4 Vector4::operator-()
{
	Vector4 result;

	result.fX = -fX;
	result.fY = -fY;
	result.fZ = -fZ;
	result.fW = -fW;

	return result;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	float resultArray[4][4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			resultArray[i][j] = 
				m_fMatrixArray[i][0] * other.m_fMatrixArray[0][j] + 
				m_fMatrixArray[i][1] * other.m_fMatrixArray[1][j] + 
				m_fMatrixArray[i][2] * other.m_fMatrixArray[2][j] + 
				m_fMatrixArray[i][3] * other.m_fMatrixArray[3][j];
		}
	}
	Matrix4 result = Matrix4(resultArray);
	return result;
}

Matrix4& Matrix4::operator*=(const Matrix4 & other)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_fMatrixArray[i][j] = 
				m_fMatrixArray[i][0] * other.m_fMatrixArray[0][j] + 
				m_fMatrixArray[i][1] * other.m_fMatrixArray[1][j] + 
				m_fMatrixArray[i][2] * other.m_fMatrixArray[2][j] + 
				m_fMatrixArray[i][3] * other.m_fMatrixArray[3][j];
		}
	}
	return (*this);
}

Matrix4::Matrix4(float matrixArray[4][4])
{
	assert(matrixArray);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			m_fMatrixArray[i][j] = matrixArray[i][j];
	}
}

Matrix4::Matrix4(float matrixArray[16])
{
	assert(matrixArray);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_fMatrixArray[i][j] = matrixArray[i * 4 + j];
		}
	}
}

Matrix4::Matrix4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_fMatrixArray[i][j] = 0.0f;
		}
	}
}

Vector4	Matrix4::operator[](int index) const
{
	assert(0 <= index && index <= 3);
	return m_fMatrixArray[index];
}

Vector4 Matrix4::operator*(const Vector4 & vector) const
{
	// Assume vector is row vector
	float _result[4] = { 0.0f };
	for (int i = 0; i < 4; i++)
	{
		_result[i] = 
			m_fMatrixArray[i][0] * vector.fX + 
			m_fMatrixArray[i][1] * vector.fY +
			m_fMatrixArray[i][2] * vector.fZ + 
			m_fMatrixArray[i][3] * vector.fW;
	}

	Vector4 result = _result;
	return result;
}

float* Matrix4::ToFloatArray()
{
	return reinterpret_cast<float*>(m_fMatrixArray);
}

Matrix4 Matrix4::Transposition() const
{
	Matrix4 result;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m_fMatrixArray[j][i] = m_fMatrixArray[i][j];
		}
	}

	return result;
}

Vector3 & Vector3::operator*=(float f)
{
	fX *= f;
	fY *= f;
	fZ *= f;
	return *this;
}

Vector3 & Vector3::operator/=(float f)
{
	float _f = 1.0f / f;

	fX *= _f;
	fY *= _f;
	fZ *= _f;

	return *this;
}

Vector3 & Vector3::operator=(const Vector3 & other)
{
	fX = other.fX;
	fY = other.fY;
	fZ = other.fZ;

	return *this;
}

Vector3 & Vector3::operator=(const Vector4& other)
{
	fX = other.fX;
	fY = other.fY;
	fZ = other.fZ;

	return *this;
}

Vector3 & Vector3::operator+=(const Vector3 & other)
{
	fX += other.fX;
	fY += other.fY;
	fZ += other.fZ;

	return *this;
}

Vector3 Vector3::operator*(float f) const
{
	Vector3 result;

	result.fX = fX * f;
	result.fY = fY * f;
	result.fZ = fZ * f;

	return result;
}

Vector3 Vector3::operator+(const Vector3 & other) const
{
	Vector3 result;

	result.fX = fX + other.fX;
	result.fY = fY + other.fY;
	result.fZ = fZ + other.fZ;

	return result;
}

Vector3 Vector3::operator-(const Vector3 & other) const 
{
	Vector3 result;

	result.fX = fX - other.fX;
	result.fY = fY - other.fY;
	result.fZ = fZ - other.fZ;

	return result;
}

Vector3 Vector3::operator-()
{
	Vector3 result;

	result.fX = -fX;
	result.fY = -fY;
	result.fZ = -fZ;

	return result;
}

float Vector3::operator*(const Vector3 other) const
{
	return (fX*other.fX + fY*other.fY + fZ*other.fZ);
}

float Vector3::operator[](int nIndex) const
{
	if (0 <= nIndex && nIndex < 3)
		return (nIndex == 0) ? fX : ((nIndex == 1) ? fY : fZ);

	// Error situation
	return -FLOAT_INFINITE;
}


void Vector3::Normalize()
{
	float length = fX*fX + fY*fY + fZ*fZ;
	if (length > FLOAT_DEVIATION)
	{
		length = sqrtf(length);
		fX = fX / length;
		fY = fY / length;
		fZ = fZ / length;
	}
}

Vector3 Vector3::Cross(const Vector3 & other)
{
	Vector3 result;

	result.fX = fY*other.fZ - fZ*other.fY;
	result.fY = fZ*other.fX - fX*other.fZ;
	result.fZ = fX*other.fY - fY*other.fX;

	return result;
}

float Vector3::Length()
{
	float length = fX*fX + fY*fY + fZ*fZ;
	length = sqrtf(length);

	return length;
}