#include <assert.h>
#include "FontMesh.h"

PrimitiveData FontMesh::GetPrimitiveData()
{
	if (m_primitiveData.m_pfVertexData == nullptr)
	{
		m_primitiveData.m_nVertexCount = static_cast<int>(m_nTriangleNumber * 3);
		m_primitiveData.m_nFloatCountPerVertex = 5;

		// Vertex = {X, Y, Z, U, V} -- total 5 floats
		m_primitiveData.m_pfVertexData = (float*)malloc(m_primitiveData.m_nFloatCountPerVertex * m_primitiveData.m_nVertexCount * sizeof(float));
		for (int index = 0; index < m_nTriangleNumber; index++)
		{
			auto pTempTriangle = m_p3DObjectVec[index];

			m_primitiveData.m_pfVertexData[15 * index] = pTempTriangle->GetVertex3DCoordByIndex(0).fX;
			m_primitiveData.m_pfVertexData[15 * index + 1] = pTempTriangle->GetVertex3DCoordByIndex(0).fY;
			m_primitiveData.m_pfVertexData[15 * index + 2] = pTempTriangle->GetVertex3DCoordByIndex(0).fZ;

			m_primitiveData.m_pfVertexData[15 * index + 3] = pTempTriangle->GetUV(0).fX;
			m_primitiveData.m_pfVertexData[15 * index + 4] = pTempTriangle->GetUV(0).fY;

			m_primitiveData.m_pfVertexData[15 * index + 5] = pTempTriangle->GetVertex3DCoordByIndex(1).fX;
			m_primitiveData.m_pfVertexData[15 * index + 6] = pTempTriangle->GetVertex3DCoordByIndex(1).fY;
			m_primitiveData.m_pfVertexData[15 * index + 7] = pTempTriangle->GetVertex3DCoordByIndex(1).fZ;

			m_primitiveData.m_pfVertexData[15 * index + 8] = pTempTriangle->GetUV(1).fX;
			m_primitiveData.m_pfVertexData[15 * index + 9] = pTempTriangle->GetUV(1).fY;

			m_primitiveData.m_pfVertexData[15 * index + 10] = pTempTriangle->GetVertex3DCoordByIndex(2).fX;
			m_primitiveData.m_pfVertexData[15 * index + 11] = pTempTriangle->GetVertex3DCoordByIndex(2).fY;
			m_primitiveData.m_pfVertexData[15 * index + 12] = pTempTriangle->GetVertex3DCoordByIndex(2).fZ;

			m_primitiveData.m_pfVertexData[15 * index + 13] = pTempTriangle->GetUV(2).fX;
			m_primitiveData.m_pfVertexData[15 * index + 14] = pTempTriangle->GetUV(2).fY;
		}
	}
	return m_primitiveData;
}

bool FontMesh::InitSpaceAccelerateStruct()
{
	return false;
}

void FontMesh::Update()
{
}
