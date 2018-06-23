#include <assert.h>
#include "GLTextureUnit.h"

GLTextureUnit::GLTextureUnit(GLint nTexWidth, GLint nTexHeight, GLvoid * pTexData, GLuint shaderProgram):
	m_nTexWidth(nTexWidth), m_nTexHeight(nTexHeight), m_pTexData(pTexData), m_shaderProgram(shaderProgram)
{
	
}

bool GLTextureUnit::Init()
{
	assert(m_nTexWidth > 0 && m_nTexHeight > 0 && m_pTexData != nullptr);

	glUseProgram(m_shaderProgram);
	glGenTextures(1, &m_textureObject);

	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			m_nTexWidth, m_nTexHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, m_pTexData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

GLuint GLTextureUnit::GetTextureUnit()
{
	return m_textureObject;
}
