#include "GLFontObjectImpl.h"

GLboolean GLFontObjectImpl::_initVAO()
{
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	// Create a buffer to store vertex.
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Bind the triangle vertex data to buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_nFloatCountPerVertex * m_nVertexCount, m_pVertexData, GL_STATIC_DRAW);
	
	// Set position vector format
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set tex vector format
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return true;
}

GLboolean GLFontObjectImpl::Init()
{
	if (!_initVAO())
		return false;

	m_bIsInit = true;
	return true;
}
