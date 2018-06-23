#pragma once
#include <vector>
#include <functional>
#include "ShaderMgr.h"
#include "IGLObject.h"

struct FuncVar
{
	GLenum var1;
	GLenum var2;

	FuncVar(GLenum i, GLenum j): var1(i), var2(j) {}
};

class IRenderSubProcess
{
protected:
	bool m_bEnableRender;
	bool m_bEnableLight;

	GLint m_nPolygonMode;
	Transform m_viewTransform;
	Transform m_projTransform;

	std::shared_ptr<ShaderMgr> m_pShaderMgr;
	std::vector<std::shared_ptr<IGLObject>> m_GLObjectVec;
	std::vector<GLenum> m_enableStateVec;
	std::vector<GLenum> m_disableStateVec;

	using OneVarFunc = std::function<void(GLenum)>;
	using TwoVarFunc = std::function<void(GLenum, GLenum)>;

	std::vector<std::pair<OneVarFunc, FuncVar>> m_OneVarStateSetFuncVec;
	std::vector<std::pair<TwoVarFunc, FuncVar>> m_TwoVarStateSetFuncVec;

public:
	IRenderSubProcess(): m_pShaderMgr(nullptr) , m_nPolygonMode(GL_FILL), m_bEnableRender(true), m_bEnableLight(false)
	{
		m_GLObjectVec.clear();
		m_enableStateVec.clear();
		m_disableStateVec.clear();
	}

	void SetEnableRender(bool bEnable) { m_bEnableRender = bEnable; };
	void EnableOrDisableGLState(GLenum state, bool EnableOrDisable) { (EnableOrDisable) ? m_enableStateVec.push_back(state) : m_disableStateVec.push_back(state); };
	void EnableOrDisableLight() { m_bEnableLight = !m_bEnableLight; }
	void SetViewTransform(Transform& viewTransform) { m_viewTransform = viewTransform; }
	void SetProjTransform(Transform& projTransform) { m_projTransform = projTransform; }
	void SetPolygonMode(GLint PloygonMode) { m_nPolygonMode = PloygonMode; }
	void ClearGLObjects() { m_GLObjectVec.clear(); }

	void AddStateFuncClosure(OneVarFunc func, FuncVar var) { m_OneVarStateSetFuncVec.push_back(std::make_pair(func, var)); };
	void AddStateFuncClosure(std::pair<OneVarFunc, FuncVar> closure) { m_OneVarStateSetFuncVec.push_back(closure); };
	void AddStateFuncClosure(TwoVarFunc func, FuncVar var) { m_TwoVarStateSetFuncVec.push_back(std::make_pair(func, var)); };
	void AddStateFuncClosure(std::pair<TwoVarFunc, FuncVar> closure) { m_TwoVarStateSetFuncVec.push_back(closure); };
	void AddGLObject(std::shared_ptr<IGLObject> object) { m_GLObjectVec.push_back(object); };

	std::vector<std::shared_ptr<IGLObject>>& GetGLObjVecRef() { return m_GLObjectVec; }

	virtual bool Init() = 0;
	virtual void Render() = 0;
};