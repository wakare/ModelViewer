#include "GL3DObjectImpl.h"

GLboolean GL3DObjectImpl::_initVAO()
{
	// Init VAO
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	/*glUseProgram(m_pShaderMgr->GetShaderProgram());*/

	// Create a buffer to store vertex.
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Bind the triangle vertex data to buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_nFloatCountPerVertex * m_nVertexCount, m_pVertexData, GL_STATIC_DRAW);

	// Set position vector format
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set normal vector format
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return true;
}

GLboolean GL3DObjectImpl::Init()
{
	if (!_initVAO())
		return false;
	
	m_bIsInit = true;
	return true;
}
