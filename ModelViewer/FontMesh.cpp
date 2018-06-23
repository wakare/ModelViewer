#include <assert.h>
#include "FontMesh.h"

bool FontMesh::_initTexture(int nWidth, int nHeight, unsigned char* pTextureData)
{
	assert(nWidth > 0 && nHeight > 0 && pTextureData != nullptr);

	glUseProgram(m_pShaderMgr->GetShaderProgram());
	glGenTextures(1, &m_textureObject);

	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			nWidth, nHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, pTextureData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum e = glGetError();
	return true;
}

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

bool FontMesh::InitGLData()
{
	if(!InitShaderMgr())
		return false;

	_initTexture(m_nTexWidth, m_nTexHeight, m_pTextureData);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	// Create a buffer to store vertex.
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Bind the triangle vertex data to buffer.
	if (m_primitiveData.m_pfVertexData == nullptr)
		GetPrimitiveData();
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_primitiveData.m_nFloatCountPerVertex * m_primitiveData.m_nVertexCount, m_primitiveData.m_pfVertexData, GL_STATIC_DRAW);

	// Set position vector format
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set normal vector format
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return true;
}

bool FontMesh::InitShaderMgr()
{
	m_pShaderMgr = std::make_shared<ShaderMgr>("Resources/FontShader/VertexShader.glsl", "Resources/FontShader/FragmentShader.glsl");
	assert(m_pShaderMgr != nullptr);

	if (!m_pShaderMgr->Init("Resources/FontShader/VertexShader.glsl", "Resources/FontShader/FragmentShader.glsl"))
		return false;

	if (!m_pShaderMgr->CompileShader())
		return false;

	if (!m_pShaderMgr->LinkProgram())
		return false;

	return true;
}

void FontMesh::Update()
{
}

void FontMesh::Render(Transform viewTransform, Transform projTransform)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(m_pShaderMgr->GetShaderProgram());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	glUniform1i(glGetUniformLocation(m_pShaderMgr->GetShaderProgram(), "fontTexture"), 0);

	glBindVertexArray(m_VAO);

	glDrawArrays(GL_TRIANGLES, 0, m_primitiveData.m_nVertexCount * m_primitiveData.m_nFloatCountPerVertex);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}
