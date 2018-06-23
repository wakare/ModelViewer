#pragma once
#include <GL/glew.h>

class GLTextureUnit
{
private:
	GLuint m_textureObject;
	GLint m_nTexWidth;
	GLint m_nTexHeight;
	GLvoid * m_pTexData;
	GLuint m_shaderProgram;

public:
	GLTextureUnit(GLint nTexWidth, GLint nTexHeight, GLvoid* pTexData, GLuint shaderProgram);
	bool Init();
	GLuint GetTextureUnit();
};