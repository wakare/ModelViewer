#include "MeshSubProcess.h"
#include "Logger.h"

bool MeshSubProcess::_initShaderMgr()
{
	// Init shaderMgr
	m_pShaderMgr = std::make_shared<ShaderMgr>();
	if (!m_pShaderMgr->Init("Resources/VertexShader.glsl", "Resources/FragmentShader.glsl"))
		return false;

	if (!m_pShaderMgr->CompileShader())
		return false;

	if (!m_pShaderMgr->LinkProgram())
		return false;

	return true;
}

bool MeshSubProcess::Init()
{
	if (!m_bIsInit)
	{
		if (!_initShaderMgr())
		{
			Logger::Log(LogType::ERR, "Init shader failed.");
			return false;
		}
	}

	m_bIsInit = true;
	return true;
}

void MeshSubProcess::Render()
{
	if (!m_bEnableRender)
		return;

	// Set renderType
	glPolygonMode(GL_FRONT_AND_BACK, m_nPolygonMode);

	// Set enable and disable
	for (auto enable : m_enableStateVec)
		glEnable(enable);

	for (auto disable : m_disableStateVec)
		glDisable(disable);

	// Set state function
	for (auto closureObj : m_OneVarStateSetFuncVec)
	{
		closureObj.first(closureObj.second.var1);
	}

	for (auto closureObj : m_TwoVarStateSetFuncVec)
	{
		closureObj.first(closureObj.second.var1, closureObj.second.var2);
	}

	auto program = m_pShaderMgr->GetShaderProgram();
	if (program == 0)
	{
		Logger::Log(LogType::ERR, "Not Init shader program");
		return;
	}
	glUseProgram(program);

	for (auto Object : m_GLObjectVec)
	{
		auto VAO = Object->GetVAO();
		if (VAO == 0)
		{
			Logger::Log(LogType::ERR, "Not Init VAO");
			return;
		}
		glBindVertexArray(VAO);

		// TODO: Modify uniform 
		ShaderMgr::SetUniformVariableMatrix4(program, "modelMatrix", Object->GetModelTransform().GetTransformMatrix());
		ShaderMgr::SetUniformVariableMatrix4(program, "viewMatrix", m_viewTransform.GetTransformMatrix());
		ShaderMgr::SetUniformVariableMatrix4(program, "projectionMatrix", m_projTransform.GetTransformMatrix());
		ShaderMgr::SetUniformVariableFloat3(program, "vertexColor", Object->GetColor() );
		ShaderMgr::SetUniformVariableInt1(program, "enableLight", m_bEnableLight ? 1 : 0);
		
		glDrawArrays(Object->GetPrimitiveType(), 0, Object->GetFloatCountPerVertex() * Object->GetVertexCount());

		glBindVertexArray(0);
	}
	glUseProgram(0);

	for (auto enable : m_enableStateVec)
		glDisable(enable);
}
