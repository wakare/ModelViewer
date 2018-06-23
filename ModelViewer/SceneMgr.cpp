#include <assert.h>
#include <math.h>
#include <windows.h>
#include "SceneMgr.h"
#include "Mesh.h"
#include "MainManager.h"
#include "Timer.h"
#include "GL3DObjectImpl.h"

std::shared_ptr<I3DObject> SceneMgr::_calcualteRayIntersection(const Ray & ray, std::shared_ptr<OcTree>& pOcTree, double& outCoordU, double& outCoordV, double& outCoordW)
{
	std::shared_ptr<I3DObject> pNearestIntersection = nullptr;
	float fNearestIntersectionDistance = -1.0f;
	double fIntersectU = -1.0f;
	double fIntersectV = -1.0f;
	float fIntersectW = -1.0f;

	// Ray intersection do not exist
	if (pOcTree->AABBIntersectWithRay(ray) < 0.0f)
		return pNearestIntersection;

	do
	{
		// Have no subTree situation
		if (!pOcTree->m_bHasSubTree)
		{
			if (pOcTree->m_pNodeData != nullptr)
			{
				double outU;
				double outV;
				double outW;

				float fDistance = pOcTree->m_pNodeData->HasIntersectionAlongRay(ray, outU, outV, outW);
				if (fDistance >= 0.0f)
				{
					fNearestIntersectionDistance = fDistance;
					pNearestIntersection = pOcTree->m_pNodeData;
					fIntersectU = static_cast<float>(outU);
					fIntersectV = static_cast<float>(outV);
					fIntersectW = static_cast<float>(outW);

					break;
				}
			}

			if (pOcTree->m_pNodeDataVec != nullptr && pOcTree->m_pNodeDataVec->size() > 0)
			{
				for (auto it3DObject : *(pOcTree->m_pNodeDataVec))
				{
					double outU;
					double outV;
					double outW;

					auto fDistance = it3DObject->HasIntersectionAlongRay(ray, outU, outV, outW);
					if (fDistance >= 0.0f)
					{
						if (fNearestIntersectionDistance < 0.0f)
						{
							fNearestIntersectionDistance = fDistance;
							pNearestIntersection = it3DObject;
							fIntersectU = static_cast<float>(outU);
							fIntersectV = static_cast<float>(outV);
							fIntersectW = static_cast<float>(outW);
						}

						if (fDistance < fNearestIntersectionDistance)
						{
							fNearestIntersectionDistance = fDistance;
							pNearestIntersection = it3DObject;
							fIntersectU = static_cast<float>(outU);
							fIntersectV = static_cast<float>(outV);
							fIntersectW = static_cast<float>(outW);
						}
					}
				}
			}

			break;
		}

		// SubTree exist situation -- Calculate whether subOcTree has ray intersection
		assert(pOcTree->m_subOcTreeVec.size() > 0);

		struct _tempIntersectInfo
		{
			std::shared_ptr<OcTree> m_pSubTree;
			float m_fDistance;

			_tempIntersectInfo(std::shared_ptr<OcTree> pOcTree, float fDistance) :
				m_pSubTree(pOcTree), m_fDistance(fDistance) {}
		};

		std::shared_ptr<OcTree> pInsideSubTree = nullptr;
		std::vector<_tempIntersectInfo> intersectInfoVec;

		for (auto itSubTree : pOcTree->m_subOcTreeVec)
		{
			Vector3 minCoord, maxCoord;
			if (!itSubTree->GetOcTreeRangeCoord(minCoord, maxCoord))
			{
				Logger::Log(LogType::FATAL, "Can not get OcTree range");
				return pNearestIntersection;
			}

			if (itSubTree->IsObjInsideAABB(ray.m_startPosition, minCoord, maxCoord))
			{
				//assert(pInsideSubTree == nullptr);
				if (pInsideSubTree == nullptr)
				{
					pInsideSubTree = itSubTree;
					continue;
				}
			}

			auto fDistance = itSubTree->AABBIntersectWithRay(ray);
			if (fDistance >= 0.0f)
				intersectInfoVec.push_back({ itSubTree, fDistance });
		}

		// Ray inside, calculate inside subTree firstly
		if (pInsideSubTree != nullptr)
		{
			double outU;
			double outV;
			double outW;

			auto pIntersection = _calcualteRayIntersection(ray, pInsideSubTree, outU, outV, outW);
			if (pIntersection != nullptr)
			{
				pNearestIntersection = pIntersection;
				fIntersectU = static_cast<float>(outU);
				fIntersectV = static_cast<float>(outV);
				fIntersectW = static_cast<float>(outW);
				break;
			}
		}

		// Simple swap lambda function
		auto swapElements = [=](decltype(intersectInfoVec)& intersectInfoVec, int index1, int index2)
		{
			auto tempSubTree = intersectInfoVec[index1].m_pSubTree;
			intersectInfoVec[index1].m_pSubTree = intersectInfoVec[index2].m_pSubTree;
			intersectInfoVec[index2].m_pSubTree = tempSubTree;

			auto tempDistance = intersectInfoVec[index1].m_fDistance;
			intersectInfoVec[index1].m_fDistance = intersectInfoVec[index2].m_fDistance;
			intersectInfoVec[index2].m_fDistance = tempDistance;
		};

		// Sort intersect point by distance
		if (intersectInfoVec.size() > 1)
		{
			for (int i = 0; i < intersectInfoVec.size() - 1; i++)
			{
				if (intersectInfoVec[i].m_fDistance > intersectInfoVec[i + 1].m_fDistance)
				{
					swapElements(intersectInfoVec, i, i + 1);
					i -= 2;
					i = (i < -1) ? -1 : i;
				}
			}
		}

		// Ray outside
		for (int i = 0; i < intersectInfoVec.size(); i++)
		{
			double outU;
			double outV;
			double outW;

			auto pIntersection = _calcualteRayIntersection(ray, intersectInfoVec[i].m_pSubTree, outU, outV, outW);
			if (pIntersection != nullptr)
			{
				fNearestIntersectionDistance = intersectInfoVec[i].m_fDistance;
				pNearestIntersection = pIntersection;
				fIntersectU = static_cast<float>(outU);
				fIntersectV = static_cast<float>(outV);
				fIntersectW = static_cast<float>(outW);
				break;
			}
		}

	} while (false);

	if (pNearestIntersection)
	{
		outCoordU = fIntersectU;
		outCoordV = fIntersectV;
		outCoordW = fIntersectW;
	}

	return pNearestIntersection;
}

SceneMgr::SceneMgr()
{
	m_pOcTreeVec.clear();
	m_sceneObjectVec.clear();

	m_mainCamera = std::make_shared<Camera>();
	m_pOriginalMeshSubProcess = std::make_shared<MeshSubProcess>();
	
	m_pCurrentSelectObject = nullptr;
	m_pSelectedMeshSubProcess = nullptr;
}

bool SceneMgr::InitSpaceAccelerateStruct()
{
	for (auto itSceneObject : m_sceneObjectVec)
	{
		if (!itSceneObject.first->InitSpaceAccelerateStruct())
			return false;

		m_pOcTreeVec.push_back(itSceneObject.first->m_pOcTree);
	}

	return true;
}

bool SceneMgr::_initRenderSubProcess(std::shared_ptr<MeshSubProcess> pSubProcess, 
	std::vector<std::pair<std::shared_ptr<ISceneObject>, std::shared_ptr<IGLObject>>>& sceneObjectVec)
{
	pSubProcess->EnableOrDisableGLState(GL_DEPTH_TEST, true);
	pSubProcess->AddStateFuncClosure(std::make_pair(glDepthFunc, FuncVar(GL_LEQUAL, 0)));
	for (auto& sceneObject : sceneObjectVec)
	{
		auto& primitiveData = sceneObject.first->GetPrimitiveData();
		std::shared_ptr<IGLObject> pGLObject = std::make_shared<GL3DObjectImpl>
			(primitiveData.m_nVertexCount, primitiveData.m_nFloatCountPerVertex, primitiveData.m_pfVertexData, GL_TRIANGLES);

		if (!pGLObject->Init())
			return false;
		pGLObject->SetTranslate(sceneObject.first->m_modelTranslateTransform);
		pGLObject->SetRotate(sceneObject.first->m_modelRotateTransform);

		sceneObject.second = pGLObject;
		pSubProcess->AddGLObject(pGLObject);
	}

	if (!pSubProcess->Init())
		return false;
	
	return true;
}

bool SceneMgr::_addSubProcessToRenderMgr(std::shared_ptr<MeshSubProcess> subProcess)
{
	if (!MainManager::GetInstance()->GetRenderMgr()->RegisterSubProcess(RenderSubProcessLevel::LEVEL_1, subProcess))
		return false;

	return true;
}

bool SceneMgr::Init()
{
	if (!_initRenderSubProcess(m_pOriginalMeshSubProcess, m_sceneObjectVec))
		return false;

	return true;
}

void SceneMgr::AfterInit()
{
	if (!_addSubProcessToRenderMgr(m_pOriginalMeshSubProcess ))
		Logger::Log(LogType::ERR, "Add SubProcess failed!");
}

void SceneMgr::Update()
{
	for (auto pSceneObject : m_sceneObjectVec)
	{
		pSceneObject.first->Update();
		pSceneObject.second->SetRotate(pSceneObject.first->m_modelRotateTransform);
		pSceneObject.second->SetTranslate(pSceneObject.first->m_modelTranslateTransform);
	}

	auto ploygonMode = (MainManager::GetInstance()->m_renderType == RenderType::FILLED) ? GL_FILL : GL_LINE;
	if (m_pOriginalMeshSubProcess)
	{
		m_pOriginalMeshSubProcess->SetPolygonMode(ploygonMode);
		for (auto sceneObj : m_sceneObjectVec)
		{
			if (sceneObj.first->m_bSelected)
				sceneObj.second->SetColor({ 1.0, 0.0f, 0.0f });
		}
	}

	if (m_pSelectedMeshSubProcess)
	{
		m_pSelectedMeshSubProcess->SetPolygonMode(GL_LINE);
	}
}

bool SceneMgr::SetMainCamera(std::shared_ptr<Camera> camera)
{
	m_mainCamera = camera;

	return true;
}

bool SceneMgr::AddSceneObject(std::shared_ptr<ISceneObject> sceneObject)
{
	m_sceneObjectVec.push_back(std::make_pair(sceneObject, nullptr));

	return true;
}

bool SceneMgr::DelSceneObject(std::shared_ptr<ISceneObject> sceneObject)
{
	bool bDeleted = false;
	auto it = m_sceneObjectVec.begin();
	for (; it < m_sceneObjectVec.end(); it++)
	{
		if ((*it).first == sceneObject)
			break;
	}

	if (it != m_sceneObjectVec.end())
	{
		m_sceneObjectVec.erase(it);
		bDeleted = true;
	}

	return bDeleted;
}

bool SceneMgr::GetPrimitiveDataByIndex(int index, PrimitiveData& outPrimitiveData)
{
	assert(0 <= index && index < m_sceneObjectVec.size());
	outPrimitiveData = m_sceneObjectVec[index].first->GetPrimitiveData();

	return true;
}

std::shared_ptr<I3DObject> SceneMgr::CheckRayIntersect(std::shared_ptr<ISceneObject> pSceneObject, const Ray & modelSpaceRay, double& outU, double& outV, double& outW)
{
	auto pOcTree = pSceneObject->m_pOcTree;

	auto fDistance = pOcTree->AABBIntersectWithRay(modelSpaceRay);
	if (fDistance < 0.0f)
		return nullptr;

	auto pIntersection = _calcualteRayIntersection(modelSpaceRay, pOcTree, outU, outV, outW);
	
	return pIntersection;
}

void SceneMgr::MouseClick(double posX, double posY)
{
	auto currentTransform = MainManager::GetInstance()->GetCurrentTransform();
	float fNearObjectDistance = -1.0f;
	std::shared_ptr<ISceneObject> pNearestSceneObject = nullptr;
	std::shared_ptr<Ray> nearestModelSpaceRay = nullptr;

	for (auto pSceneObject : m_sceneObjectVec)
	{
		Ray screenRay = _generateScreenRay(posX, posY, static_cast<float>(RenderWindow::Instance().m_nWidth), static_cast<float>(RenderWindow::Instance().m_nHeight),
			currentTransform.projTransform.GetTransformMatrix(), currentTransform.viewTransform.GetTransformMatrix());
		Ray modelSpaceRay = _generateModelSpaceRay(screenRay, pSceneObject.first);

		if (pSceneObject.first->m_pOcTree == nullptr)
			continue;

		float fDistance = pSceneObject.first->m_pOcTree->AABBIntersectWithRay(modelSpaceRay);
		if (fDistance >= 0.0f)
		{
			if (fNearObjectDistance < 0.0f)
			{
				fNearObjectDistance = fDistance;
				pNearestSceneObject = pSceneObject.first;
				nearestModelSpaceRay = std::make_shared<Ray>();
				*nearestModelSpaceRay = modelSpaceRay;
				continue;
			}

			if (fNearObjectDistance > fDistance)
			{
				fNearObjectDistance = fDistance;
				pNearestSceneObject = pSceneObject.first;
				*nearestModelSpaceRay = modelSpaceRay;
				continue;
			}
		}
	}

	// Clear selected object
	if (pNearestSceneObject == nullptr)
	{
		if (m_pCurrentSelectObject != nullptr)
		{
			m_pCurrentSelectObject->SetSelected(false);
			m_pCurrentSelectObject = nullptr;
		}

		Logger::Log(LogType::INFO, "Select no sceneObject.");
		return;
	}

	// Select different scene object
	if (pNearestSceneObject.get() != m_pCurrentSelectObject.get())
	{
		if (m_pCurrentSelectObject != nullptr)
			m_pCurrentSelectObject->SetSelected(false);
		pNearestSceneObject->SetSelected(true);

		m_pCurrentSelectObject = pNearestSceneObject;
		Logger::Log(LogType::INFO, "Change selected sceneObject.");
		return;
	}
	
	// Check if any sceneObject be selected
	if (m_pCurrentSelectObject)
	{
		PickSceneObject(m_pCurrentSelectObject, nearestModelSpaceRay);
		return;
	}
}

void SceneMgr::PickSceneObject(std::shared_ptr<ISceneObject> pSceneObject, std::shared_ptr<Ray> pModelSpaceRay)
{
	Logger::Log(LogType::INFO, "Do PickObject.");

	double outU;
	double outV;
	double outW;

	Timer::BeginTimer();
	auto pIntersection = CheckRayIntersect(pSceneObject, *pModelSpaceRay, outU, outV, outW);
	Logger::Log(LogType::INFO, Format("PickObject finish, hit object: %s ,cost time: %f ms", (pIntersection != nullptr) ? "true" : "false", Timer::EndTimer() * 1000.0f));

	if (pIntersection != nullptr)
	{
		Select3DObject(pSceneObject, pIntersection, outU, outV, outW);
		MainManager::GetInstance()->AddString(Format("hit triangle, coordination = (%f, %f, %f)", outU, outV, outW).c_str());
	}
}

void SceneMgr::RotateSceneObject(double deltaX, double deltaY)
{
	if (m_pCurrentSelectObject != nullptr)
	{
		m_pCurrentSelectObject->SelfRotate(deltaX, deltaY);
		for (auto it : m_sceneObjectVec)
		{
			if (it.first.get() == m_pCurrentSelectObject.get())
			{
				assert(it.second);
				if (it.second)
				{
					it.second->SetTranslate(m_pCurrentSelectObject->m_modelTranslateTransform);
					it.second->SetRotate(m_pCurrentSelectObject->m_modelRotateTransform);
				}
			}
		}
	}
}

void SceneMgr::Select3DObject(std::shared_ptr<ISceneObject> pSceneObject, std::shared_ptr<I3DObject> p3DObject, double IntersectCoordU, double IntersectCoordV, double IntersectCoordW)
{
	auto transform =
		pSceneObject->m_modelRotateTransform * pSceneObject->m_modelRotateTransform *	// world transform
		m_mainCamera->GetViewTransform() *												// view transform
		m_pProjTransformStruct->projTransform;											// proj transform

	int nVertexCount = p3DObject->GetVertexCount();

	Vector3 intersectPosition =
		p3DObject->GetVertex3DCoordByIndex(0) * static_cast<float>(IntersectCoordU) +
		p3DObject->GetVertex3DCoordByIndex(1) * static_cast<float>(IntersectCoordV) +
		p3DObject->GetVertex3DCoordByIndex(2) * static_cast<float>(IntersectCoordW);

	if (!m_selectStruct.m_selectObject)
	{
		assert((!m_selectStruct.m_selectObject) && (!m_selectStruct.m_selectPoint));

		auto selectedMesh = std::make_shared<Mesh>();
		selectedMesh->m_nTriangleNumber = 1;
		selectedMesh->m_defaultColor = { 0.0f, 1.0f, 1.0f };
		m_selectStruct.m_selectObject = selectedMesh;

		auto selectedPoint = std::make_shared<Point>(intersectPosition, Vector3(1.0f, 1.0f, 1.0f));
		selectedPoint->m_modelRotateTransform = pSceneObject->m_modelRotateTransform;
		selectedPoint->m_modelTranslateTransform = pSceneObject->m_modelTranslateTransform;
		m_selectStruct.m_selectPoint = selectedPoint;
	}

	m_selectStruct.m_selectObject->m_p3DObjectVec.clear();
	m_selectStruct.m_selectObject->m_p3DObjectVec.push_back(p3DObject);
	m_selectStruct.m_selectObject->m_primitiveData.m_nFloatCountPerVertex = 6;
	m_selectStruct.m_selectObject->m_modelRotateTransform = pSceneObject->m_modelRotateTransform;
	m_selectStruct.m_selectObject->m_modelTranslateTransform = pSceneObject->m_modelTranslateTransform;

	m_selectStruct.m_selectPoint->SetPosition(intersectPosition);
	m_selectStruct.m_selectPoint->m_modelRotateTransform = pSceneObject->m_modelRotateTransform;
	m_selectStruct.m_selectPoint->m_modelTranslateTransform = pSceneObject->m_modelTranslateTransform;

	m_selectStruct.m_selectObject->ClearPrimitiveData();
	m_selectStruct.m_selectPoint->ClearPrimitiveData();

	// Init SelectedMesh sub process
	if (m_pSelectedMeshSubProcess == nullptr)
	{
		m_pSelectedMeshSubProcess = std::make_shared<MeshSubProcess>();
		m_pSelectedMeshSubProcess->EnableOrDisableGLState(GL_DEPTH_TEST, true);
		m_pSelectedMeshSubProcess->AddStateFuncClosure(std::make_pair(glDepthFunc, FuncVar(GL_ALWAYS, 0)));

		if (!m_pSelectedMeshSubProcess->Init())
			return;

		_addSubProcessToRenderMgr(m_pSelectedMeshSubProcess);
		return;
	}

	// Update Selected object
	m_pSelectedMeshSubProcess->ClearGLObjects();
	for (int i = 0; i < 2; i++)
	{
		std::shared_ptr<ISceneObject> sceneObject = nullptr;
		if (i == 0)
			sceneObject = m_selectStruct.m_selectObject;
		else
			sceneObject = m_selectStruct.m_selectPoint;

		auto& primitiveData = sceneObject->GetPrimitiveData();
		std::shared_ptr<IGLObject> pGLObject = std::make_shared<GL3DObjectImpl>
			(primitiveData.m_nVertexCount, primitiveData.m_nFloatCountPerVertex, primitiveData.m_pfVertexData, (i == 0) ? GL_TRIANGLES : GL_POINTS);

		if (!pGLObject->Init())
		{
			Logger::Log(LogType::ERR, "Init GL object failed.");
			return;
		}

		pGLObject->SetTranslate(sceneObject->m_modelTranslateTransform);
		pGLObject->SetRotate(sceneObject->m_modelRotateTransform);
		pGLObject->SetColor({ 0.0f, 1.0f, 1.0f });

		m_pSelectedMeshSubProcess->AddGLObject(pGLObject);
	}
}

int SceneMgr::GetSceneObjectCount()
{
	return static_cast<int>(m_sceneObjectVec.size());
}

std::shared_ptr<Camera> SceneMgr::GetMainCamera()
{
	return m_mainCamera;
}

void SceneMgr::ClearScene()
{
	m_sceneObjectVec.clear();
}

void SceneMgr::EnableOrDisableLight()
{
	m_pOriginalMeshSubProcess->EnableOrDisableLight();
}

void SceneMgr::ReSize(int nWidth, int nHeight)
{
	
}

Ray SceneMgr::_generateScreenRay(double dblPosX, double dblPosY, float fWidth, float fHeight, Matrix4 projMatrix, Matrix4 viewMatrix)
{
	Ray ray;
	float fPosX = static_cast<float>(dblPosX / static_cast<double>(fWidth));
	float fPosY = static_cast<float>(dblPosY / static_cast<double>(fHeight));

	assert(m_pProjTransformStruct);
	if (m_pProjTransformStruct == nullptr)
		return ray;

	float fAspect = m_pProjTransformStruct->fAspect;
	float fDepth = tanf((PI - m_pProjTransformStruct->fFov) * 0.5f);
	
	Vector4 viewDirectionCoord;
	viewDirectionCoord.fX = 2 * fAspect * fPosX - fAspect;
	viewDirectionCoord.fY = 1 - 2 * fPosY;
	viewDirectionCoord.fZ = -fDepth;
	viewDirectionCoord.fW = 0.0f;

	Vector4 worldDirection = viewMatrix.Transposition() * viewDirectionCoord;
	Vector3 rayDirection = { worldDirection.fX, worldDirection.fY, worldDirection.fZ };
	rayDirection.Normalize();

	ray.m_startPosition = m_mainCamera->GetCameraPosition();
	ray.m_direction = rayDirection;
	ray.m_bIsSegmentLine = false;

	return ray;
}

Ray SceneMgr::_generateModelSpaceRay(const Ray & screenSpaceRay, std::shared_ptr<ISceneObject> pSceneObject)
{
	// Transform world space ray to model space ray
	Vector4 rayPositionV4 = { screenSpaceRay.m_startPosition, 1.0f };
	Vector4 rayDirectionV4 = { screenSpaceRay.m_direction, 0.0f };

	Matrix4 translateTransform = pSceneObject->m_modelTranslateTransform.GetTransformMatrix();
	Matrix4 rotateTransform = pSceneObject->m_modelRotateTransform.GetTransformMatrix().Transposition();

	translateTransform.m_fMatrixArray[0][3] = -translateTransform.m_fMatrixArray[0][3];
	translateTransform.m_fMatrixArray[1][3] = -translateTransform.m_fMatrixArray[1][3];
	translateTransform.m_fMatrixArray[2][3] = -translateTransform.m_fMatrixArray[2][3];

	Matrix4 worldInverseTransform = translateTransform * rotateTransform;

	rayPositionV4 = worldInverseTransform * rayPositionV4;
	rayDirectionV4 = worldInverseTransform * rayDirectionV4;

	rayPositionV4.DivideW();
	Vector3 rayPositionV3 = { rayPositionV4.fX, rayPositionV4.fY, rayPositionV4.fZ };
	Vector3 rayDirectionV3 = { rayDirectionV4.fX, rayDirectionV4.fY, rayDirectionV4.fZ };
	rayDirectionV3.Normalize();

	Ray modelSpaceRay(rayPositionV3, rayDirectionV3);
	return modelSpaceRay;
}