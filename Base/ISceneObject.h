#pragma once
#include <memory>
#include "I3DObject.h"
#include "OcTree.h"
#include "Transform.h"
#include "ShaderMgr.h"

struct PrimitiveData
{
	int m_nVertexCount;
	int m_nFloatCountPerVertex;
	float* m_pfVertexData;
};

class ISceneObject
{
private:
	virtual bool InitShaderMgr() = 0;

public:
	std::vector<std::shared_ptr<I3DObject>> m_p3DObjectVec;
	std::shared_ptr<OcTree> m_pOcTree;
	std::shared_ptr<ShaderMgr> m_pShaderMgr;
	PrimitiveData m_primitiveData;
	Transform m_modelTranslateTransform;
	Transform m_modelRotateTransform;
	bool m_bSelected = false;
	ISceneObject() : m_primitiveData({0, 0, nullptr}), m_pOcTree(nullptr) {}
	void SetSelected(bool bSelected) { m_bSelected = bSelected; }
	void SelfRotate(double deltaX, double deltaY)
	{
		Vector3 rotateAxis;
		float fRotateAngle;
		Transform::GetSelfRotateInfo(deltaX, deltaY, 0.005f, rotateAxis, fRotateAngle);
		m_modelRotateTransform.Rotate(fRotateAngle, rotateAxis);
	}

	virtual PrimitiveData GetPrimitiveData() = 0;
	virtual bool InitSpaceAccelerateStruct() = 0;
	virtual bool InitGLData() = 0;
	virtual void Update() = 0;
	virtual void Render(Transform viewTransform, Transform projTransform) = 0;
};