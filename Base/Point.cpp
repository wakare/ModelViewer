#include <assert.h>
#include "Point.h"

PrimitiveData Point::GetPrimitiveData()
{
	if (m_primitiveData.m_pfVertexData == nullptr)
	{
		m_primitiveData.m_nVertexCount = 1;
		m_primitiveData.m_nFloatCountPerVertex = 6;

		m_primitiveData.m_pfVertexData = (GLfloat*)malloc(sizeof(GLfloat) * m_primitiveData.m_nVertexCount * m_primitiveData.m_nFloatCountPerVertex);
		m_primitiveData.m_pfVertexData[0] = m_position.fX;
		m_primitiveData.m_pfVertexData[1] = m_position.fY;
		m_primitiveData.m_pfVertexData[2] = m_position.fZ;

		m_primitiveData.m_pfVertexData[3] = m_color.fX;
		m_primitiveData.m_pfVertexData[4] = m_color.fY;
		m_primitiveData.m_pfVertexData[5] = m_color.fZ;
	}

	return m_primitiveData;
}

bool Point::InitSpaceAccelerateStruct()
{
	return false;
}

void Point::Update()
{
}

void Point::SetPosition(const Vector3 & position)
{
	m_position = position;
}

void Point::SetColor(const Vector3 & color)
{
	m_color = color;
}
