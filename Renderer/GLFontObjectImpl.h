#pragma once
#include "IGLObject.h"

class GLFontObjectImpl : public IGLObject
{
private:
	GLboolean _initVAO();

public:
	GLFontObjectImpl(GLint vertexCount, GLint floatCountPerVertex, void* pVertexData, GLint primitiveType = GL_TRIANGLES)
		:IGLObject(vertexCount, floatCountPerVertex, pVertexData, primitiveType) {}
	GLboolean Init();
};