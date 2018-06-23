#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <GL/glew.h>
#include "Shader.h"
#include "WJMath.h"

class ShaderMgr
{
private:
	bool bIsOpenCompileErrorLog = true;

	std::map<ShaderType, std::shared_ptr<Shader>> m_shaderObjects;
	std::map<ShaderType, const char*> m_szShaderPathVec;
	std::vector<GLuint>	m_linkedShaderProgram;
	GLuint m_shaderProgram;

public:
	ShaderMgr(const char* szVertexShaderPath, const char* szFragmentShaderPath);
	ShaderMgr();
	~ShaderMgr();

	bool Init(const char* szVertexShaderPath, const char* szFragmentShaderPath);
	bool UnInit();
	bool InitShaderPath(const char* szVertexShaderPath, const char* szFragmentShaderPath);
	bool InitShaderObject();

	bool CompileShader();
	bool LinkProgram();
	GLuint GetShaderProgram();
	static void SetUniformVariableMatrix4(GLuint shaderProgram, const char* uniformName, Matrix4 transformMatrix);
	static void SetUniformVariableFloat1(GLuint shaderProgram, const char* uniformName, GLfloat value);
	static void SetUniformVariableFloat3(GLuint shaderProgram, const char* uniformName, Vector3 value);
	static void SetUniformVariableInt1(GLuint shaderProgram, const char* uniformName, GLboolean value);
};