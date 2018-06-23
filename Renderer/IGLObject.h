#pragma once

#include <GL/glew.h>
#include "Transform.h"

class IGLObject
{
protected:
	Transform m_rotateTransform;
	Transform m_translateTransform;
	GLboolean m_bIsInit;

	GLuint m_VAO;
	GLuint m_VBO;
	
	GLint m_nPrimitiveType;
	GLint m_nVertexCount;
	GLint m_nFloatCountPerVertex;
	GLvoid* m_pVertexData;

	Vector3 m_color;
public:
	virtual GLboolean Init() = 0;

	IGLObject(GLint vertexCount, GLint floatCountPerVertex, void* pVertexData, GLint primitiveType = GL_TRIANGLES, Vector3 color = {1.0f, 1.0f, 1.0f})
		: m_bIsInit(false), m_VAO(0), m_VBO(0), m_nPrimitiveType(primitiveType),
		m_nFloatCountPerVertex(floatCountPerVertex),m_nVertexCount(vertexCount),
		m_pVertexData(pVertexData) , m_color (color)
	{
	}

	GLuint GetVAO() { return m_VAO; };
	GLuint GetVBO() { return m_VBO; };
	Transform GetModelTransform() { return m_rotateTransform * m_translateTransform; };
	GLint GetPrimitiveType() { return m_nPrimitiveType; };
	GLint GetVertexCount() { return m_nVertexCount; };
	GLint GetFloatCountPerVertex() { return m_nFloatCountPerVertex; };
	GLvoid* GetVertexData() { return m_pVertexData; };
	Vector3 GetColor() { return m_color; };

	void SetTranslate(Transform transform) { m_translateTransform = transform; };
	void SetRotate(Transform transform) { m_rotateTransform = transform; };
	void SetColor(Vector3 color) { m_color = color; }
};