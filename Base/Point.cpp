#include <assert.h>
#include "Point.h"

bool Point::InitShaderMgr()
{
	m_pShaderMgr = std::make_shared<ShaderMgr>("Resources/Point/VertexShader.glsl", "Resources/Point/FragmentShader.glsl");
	assert(m_pShaderMgr != nullptr);

	if (!m_pShaderMgr->Init("Resources/Point/VertexShader.glsl", "Resources/Point/FragmentShader.glsl"))
		return false;

	if (!m_pShaderMgr->CompileShader())
		return false;

	if (!m_pShaderMgr->LinkProgram())
		return false;

	return true;
}

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

bool Point::InitGLData()
{
	if (!InitShaderMgr())
		return false;

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_primitiveData.m_nFloatCountPerVertex * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set color vector format
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_primitiveData.m_nFloatCountPerVertex * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return true;
}

void Point::Update()
{
}

void Point::Render(Transform viewTransform, Transform projTransform)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glPointSize(10);

	glUseProgram(m_pShaderMgr->GetShaderProgram());
	glBindVertexArray(m_VAO);

	auto modelTransform = m_modelRotateTransform * m_modelTranslateTransform;
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "modelMatrix", modelTransform.GetTransformMatrix());
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "viewMatrix", viewTransform.GetTransformMatrix());
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "projectionMatrix", projTransform.GetTransformMatrix());
	
	glDrawArrays(GL_POINTS, 0, m_primitiveData.m_nVertexCount * m_primitiveData.m_nFloatCountPerVertex);

	glBindVertexArray(0);
	glUseProgram(0);
	glPointSize(1);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
}
