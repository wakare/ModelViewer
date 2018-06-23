#pragma once
#include "IGLObject.h"

class GL3DObjectImpl : public IGLObject
{
private:
	GLboolean _initVAO();

public:
	GL3DObjectImpl(GLint vertexCount, GLint floatCountPerVertex, void* pVertexData, GLint primitiveType = GL_TRIANGLES)
		:IGLObject(vertexCount, floatCountPerVertex, pVertexData, primitiveType) {}
	GLboolean Init();
};