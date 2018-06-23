#pragma once
#include "IRenderSubProcess.h"
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

struct GLFWwindow;
class RenderSubProcess;

enum class RenderSubProcessLevel
{
	LEVEL_1 = 0,
	LEVEL_2 = 1,
};

struct SceneTransfrom
{
	Transform viewTransform;
	Transform projTransform;
};

class RenderMgr
{
private:
	SceneTransfrom & m_refSceneTransform;
	using SubProcessPair = std::pair<RenderSubProcessLevel, std::shared_ptr<IRenderSubProcess>>;
	std::vector<SubProcessPair> m_subProcessVec;
	GLFWwindow* m_pWindow;

public:
	RenderMgr(SceneTransfrom& transform, GLFWwindow* pWindow) :
		m_refSceneTransform(transform), m_pWindow(pWindow) {}
	static int RenderSubProcessLevelCount;

	void Render();
	void Update();
	bool RegisterSubProcess(RenderSubProcessLevel, std::shared_ptr<IRenderSubProcess>);
	void SetRenderWindow(GLFWwindow*);
};