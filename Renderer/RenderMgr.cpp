#include "RenderMgr.h"
#include "Logger.h"

int RenderMgr::RenderSubProcessLevelCount = 2;

void RenderMgr::Render()
{
	if (!m_pWindow)
	{
		Logger::Log(LogType::ERR, "Render window is invalid");
		return;
	}

	// Set Global state
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render subProcess
	for (int nIndex = 0; nIndex < RenderSubProcessLevelCount; nIndex++)
	{
		RenderSubProcessLevel currentLevel = static_cast<RenderSubProcessLevel>(nIndex);
		for (auto subProcess : m_subProcessVec)
		{
			if (subProcess.first == currentLevel)
				subProcess.second->Render();
		}
	}

	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

void RenderMgr::Update()
{
	for (auto subProcess : m_subProcessVec)
	{
		subProcess.second->SetViewTransform(m_refSceneTransform.viewTransform);
		subProcess.second->SetProjTransform(m_refSceneTransform.projTransform);
	}
}

bool RenderMgr::RegisterSubProcess(RenderSubProcessLevel eLevel, std::shared_ptr<IRenderSubProcess> pSubProcess)
{
	if (!pSubProcess->Init())
		return false;

	m_subProcessVec.push_back({ eLevel, pSubProcess });

	return true;
}

void RenderMgr::SetRenderWindow(GLFWwindow * pWindow)
{
	m_pWindow = pWindow;
}
