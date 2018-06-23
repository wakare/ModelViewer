#pragma once
#include <vector>
#include <memory>
#include "ISceneObject.h"
#include "Camera.h"
#include "OcTree.h"
#include "Ray.h"
#include "Logger.h"
#include "Point.h"
#include "Mesh.h"
#include "MeshSubProcess.h"

struct selectedObjStruct
{
	std::shared_ptr<Mesh> m_selectObject;
	std::shared_ptr<Point> m_selectPoint;
};

struct ProjTransformStruct
{
	float fFov;
	float fAspect;
	float fNearZ;
	float fFarZ;

	Transform projTransform;
	ProjTransformStruct(float fov, float aspect, float nearZ, float farZ) :
		fFov(fov), fAspect(aspect), fNearZ(nearZ), fFarZ(farZ) 
	{
		CalTransform(fFov, fAspect, fNearZ, fFarZ);
	};

	void ReSize(int nWidth, int nHeight)
	{
		fAspect = static_cast<float>(nWidth) / static_cast<float>(nHeight);
		CalTransform(fFov, fAspect, fNearZ, fFarZ);
	}

	void CalTransform(float fov, float aspect, float nearZ, float farZ)
	{
		projTransform.SetProjectionTransform(fFov, fAspect, fNearZ, fFarZ);
	}
};

class SceneMgr
{
private:
	std::vector<std::shared_ptr<OcTree>> m_pOcTreeVec;
	std::vector<std::pair<std::shared_ptr<ISceneObject>, std::shared_ptr<IGLObject>>> m_sceneObjectVec;

	std::shared_ptr<MeshSubProcess> m_pOriginalMeshSubProcess;
	std::shared_ptr<MeshSubProcess> m_pSelectedMeshSubProcess;

	std::shared_ptr<Camera> m_mainCamera;
	std::shared_ptr<ISceneObject> m_pCurrentSelectObject;
	selectedObjStruct m_selectStruct;

	std::shared_ptr<I3DObject> _calcualteRayIntersection(const Ray& ray, std::shared_ptr<OcTree>& ocTree, double& outCoordU, double& outCoordV, double& outCoordW);
	Ray _generateModelSpaceRay(const Ray& screenSpaceRayconst, std::shared_ptr<ISceneObject> pSceneObject);
	Ray _generateScreenRay(double posX, double posY, float fWidth, float fHeight, Matrix4 projMatrix, Matrix4 viewMatrix);

	bool _initRenderSubProcess(std::shared_ptr<MeshSubProcess> pSubProcess, 
		std::vector<std::pair<std::shared_ptr<ISceneObject>, std::shared_ptr<IGLObject>>>& sceneObjectVec);
	bool _addSubProcessToRenderMgr(std::shared_ptr<MeshSubProcess> subProcess);
public:
	std::shared_ptr<ProjTransformStruct> m_pProjTransformStruct;

	SceneMgr();

	std::shared_ptr<Camera> GetMainCamera();
	int	 GetSceneObjectCount();
	bool GetPrimitiveDataByIndex(int index, PrimitiveData& outPrimitiveData);
	
	bool SetMainCamera(std::shared_ptr<Camera> camera);
	bool AddSceneObject(std::shared_ptr<ISceneObject> sceneObject);
	bool DelSceneObject(std::shared_ptr<ISceneObject> sceneObject);

	bool Init();
	void AfterInit();
	void Update();
	void ReSize(int nWidth, int nHeight);

	bool InitSpaceAccelerateStruct();
	void ClearScene();
	void EnableOrDisableLight();
	
	std::shared_ptr<I3DObject> CheckRayIntersect(std::shared_ptr<ISceneObject> pSceneObject, const Ray & modelSpaceRay, double& outU, double& outV, double& outW);
	void MouseClick(double posX, double posY);
	void PickSceneObject(std::shared_ptr<ISceneObject> pSceneObject, std::shared_ptr<Ray> pModelSpaceRay);
	void RotateSceneObject(double deltaX, double deltaY);
	void Select3DObject(std::shared_ptr<ISceneObject> pSceneObject, std::shared_ptr<I3DObject> p3DObject, double IntersectCoordU, double IntersectCoordV, double IntersectCoordW);
};