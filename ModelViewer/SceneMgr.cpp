#include <assert.h>
#include <math.h>
#include <windows.h>
#include "SceneMgr.h"
#include "Mesh.h"
#include "MainManager.h"
#include "Timer.h"

std::shared_ptr<I3DObject> SceneMgr::_calcualteRayIntersection(const Ray & ray, std::shared_ptr<OcTree> pOcTree, double& outCoordU, double& outCoordV, double& outCoordW)
{
	float fNearestIntersectionDistance = -1.0f;
	std::shared_ptr<I3DObject> pNearestIntersection = nullptr;
	double fIntersectU = -1.0f;
	double fIntersectV = -1.0f;
	double fIntersectW = -1.0f;

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
					fIntersectU = outU;
					fIntersectV = outV;
					fIntersectW = outW;

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
							fIntersectU = outU;
							fIntersectV = outV;
							fIntersectW = outW;
						}

						if (fDistance < fNearestIntersectionDistance)
						{
							fNearestIntersectionDistance = fDistance;
							pNearestIntersection = it3DObject;
							fIntersectU = outU;
							fIntersectV = outV;
							fIntersectW = outW;
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
			const float m_fDistance;

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

		// If ray original position is inside, calculate inside subTree firstly
		if (pInsideSubTree != nullptr)
		{
			double outU;
			double outV;
			double outW;

			auto pIntersection = _calcualteRayIntersection(ray, pInsideSubTree, outU, outV, outW);
			if (pIntersection != nullptr)
			{
				pNearestIntersection = pIntersection;
				fIntersectU = outU;
				fIntersectV = outV;
				fIntersectW = outW;
				break;
			}
		}

		for (auto it : intersectInfoVec)
		{
			double outU;
			double outV;
			double outW;

			auto pIntersection = _calcualteRayIntersection(ray, it.m_pSubTree, outU, outV, outW);
			if (pIntersection != nullptr)
			{
				float f3DObjectIntersectionDistance = it.m_fDistance;

				// First intersection
				if (fNearestIntersectionDistance < 0.0f)
				{
					// Assume triangle distance equals AABB distance
					fNearestIntersectionDistance = f3DObjectIntersectionDistance;
					pNearestIntersection = pIntersection;
					fIntersectU = outU;
					fIntersectV = outV;
					fIntersectW = outW;

					continue;
				}

				if (fNearestIntersectionDistance > f3DObjectIntersectionDistance)
				{
					fNearestIntersectionDistance = f3DObjectIntersectionDistance;
					pNearestIntersection = pIntersection;
					fIntersectU = outU;
					fIntersectV = outV;
					fIntersectW = outW;

					continue;
				}
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
	m_sceneObjectVec.clear();
	m_mainCamera = std::make_shared<Camera>();
	// _addTestSceneObject();
}

bool SceneMgr::InitSpaceAccelerateStruct()
{
	for (auto itSceneObject : m_sceneObjectVec)
	{
		if (!itSceneObject->InitSpaceAccelerateStruct())
			return false;

		m_pOcTreeVec.push_back(itSceneObject->m_pOcTree);
	}

	return true;
}

bool SceneMgr::InitGLData()
{
	for (auto pSceneObject : m_sceneObjectVec)
	{
		if (!pSceneObject->InitGLData())
		{
			Logger::Log(LogType::ERR, "Init GLData failed.");
			return false;
		}
	}

	return true;
}

void SceneMgr::Update()
{
	for (auto pSceneObject : m_sceneObjectVec)
	{
		pSceneObject->Update();
	}
}

bool SceneMgr::SetMainCamera(std::shared_ptr<Camera> camera)
{
	m_mainCamera = camera;

	return true;
}

bool SceneMgr::AddSceneObject(std::shared_ptr<ISceneObject> sceneObject)
{
	m_sceneObjectVec.push_back(sceneObject);

	return true;
}

bool SceneMgr::DelSceneObject(std::shared_ptr<ISceneObject> sceneObject)
{
	bool bDeleted = false;
	auto it = m_sceneObjectVec.begin();
	for (; it < m_sceneObjectVec.end(); it++)
	{
		if (*it == sceneObject)
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
	outPrimitiveData = m_sceneObjectVec[index]->GetPrimitiveData();

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
		Ray screenRay = GenerateScreenRay(posX, posY, static_cast<float>(RenderWindow::Instance().m_nWidth), static_cast<float>(RenderWindow::Instance().m_nHeight),
			currentTransform.projTransform.GetTransformMatrix(), currentTransform.viewTransform.GetTransformMatrix());
		Ray modelSpaceRay = _generateModelSpaceRay(screenRay, pSceneObject);

		if (pSceneObject->m_pOcTree == nullptr)
			continue;

		float fDistance = pSceneObject->m_pOcTree->AABBIntersectWithRay(modelSpaceRay);
		if (fDistance >= 0.0f)
		{
			if (fNearObjectDistance < 0.0f)
			{
				fNearObjectDistance = fDistance;
				pNearestSceneObject = pSceneObject;
				nearestModelSpaceRay = std::make_shared<Ray>();
				*nearestModelSpaceRay = modelSpaceRay;
				continue;
			}

			if (fNearObjectDistance > fDistance)
			{
				fNearObjectDistance = fDistance;
				pNearestSceneObject = pSceneObject;
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
	Timer::BeginTimer();

	Logger::Log(LogType::INFO, "Do PickObject.");

	double outU;
	double outV;
	double outW;

	auto pIntersection = CheckRayIntersect(pSceneObject, *pModelSpaceRay, outU, outV, outW);
	if (pIntersection != nullptr)
	{
		Select3DObject(pSceneObject, pIntersection, outU, outV, outW);
		MainManager::GetInstance()->AddString(Format("hit triangle, coordination = (%f, %f, %f)", outU, outV, outW).c_str());
	}

	Logger::Log(LogType::INFO, Format("PickObject finish, hit object: %s ,cost time: %f ms", (pIntersection != nullptr) ? "true" : "false", Timer::EndTimer() * 1000.0f));
}

void SceneMgr::RotateSceneObject(double deltaX, double deltaY)
{
	if (m_pCurrentSelectObject != nullptr)
		m_pCurrentSelectObject->SelfRotate(deltaX, deltaY);
}

void SceneMgr::Select3DObject(std::shared_ptr<ISceneObject> pSceneObject, std::shared_ptr<I3DObject> p3DObject, double IntersectCoordU, double IntersectCoordV, double IntersectCoordW)
{
	auto transform =
		pSceneObject->m_modelRotateTransform * pSceneObject->m_modelRotateTransform *	// world transform
		m_mainCamera->GetViewTransform() *												// view transform
		m_pProjTransformStruct->projTransform;											// proj transform

	int nVertexCount = p3DObject->GetVertexCount();

	std::shared_ptr<Mesh> pExtraMesh = std::make_shared<Mesh>();
	pExtraMesh->m_p3DObjectVec.push_back(p3DObject);
	pExtraMesh->m_defaultColor = { 0.0f, 1.0f, 1.0f };
	pExtraMesh->m_primitiveData.m_nFloatCountPerVertex = 6;
	pExtraMesh->m_nTriangleNumber = 1;
	pExtraMesh->m_modelRotateTransform = pSceneObject->m_modelRotateTransform;
	pExtraMesh->m_modelTranslateTransform = pSceneObject->m_modelTranslateTransform;
	pExtraMesh->InitGLData();
	
	m_selectStruct.m_selectObject = pExtraMesh;
	Vector3 intersectPosition =
		p3DObject->GetVertex3DCoordByIndex(0) * IntersectCoordU +
		p3DObject->GetVertex3DCoordByIndex(1) * IntersectCoordV +
		p3DObject->GetVertex3DCoordByIndex(2) * IntersectCoordW;

	/*Vector3 intersectPosition;
	intersectPosition.fX =
		p3DObject->GetVertex3DCoordByIndex(0).fX * IntersectCoordU +
		p3DObject->GetVertex3DCoordByIndex(1).fX * IntersectCoordV +
		p3DObject->GetVertex3DCoordByIndex(2).fX * IntersectCoordW;*/

	Logger::Log(LogType::INFO, Format("Calculate triangle position = (%f, %f, %f)",
		intersectPosition.fX, intersectPosition.fY, intersectPosition.fZ));

	m_selectStruct.m_selectPoint = std::make_shared<Point>(intersectPosition, Vector3(1.0f, 1.0f, 1.0f));
	m_selectStruct.m_selectPoint->m_modelRotateTransform = pSceneObject->m_modelRotateTransform;
	m_selectStruct.m_selectPoint->m_modelTranslateTransform = pSceneObject->m_modelTranslateTransform;
	if (!m_selectStruct.m_selectPoint->InitGLData())
		assert(false);
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

void SceneMgr::Render()
{
	auto currentTransform = MainManager::GetInstance()->GetCurrentTransform();

	for (int nIndex = 0; nIndex < m_sceneObjectVec.size(); nIndex++)
	{
		auto pSceneObject = m_sceneObjectVec[nIndex];
		pSceneObject->Render(currentTransform.viewTransform, currentTransform.projTransform);
	}

	if (m_selectStruct.m_selectObject != nullptr)
	{
		m_selectStruct.m_selectObject->Render(currentTransform.viewTransform, currentTransform.projTransform);
		m_selectStruct.m_selectPoint->Render(currentTransform.viewTransform, currentTransform.projTransform);
	}
}

void SceneMgr::ReSize(int nWidth, int nHeight)
{
	
}

Ray SceneMgr::GenerateScreenRay(double dblPosX, double dblPosY, float nWidth, float nHeight, Matrix4 projMatrix, Matrix4 viewMatrix)
{
	Ray ray;
	float fPosX = static_cast<float>(dblPosX / static_cast<double>(nWidth));
	float fPosY = static_cast<float>(dblPosY / static_cast<double>(nHeight));

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

void SceneMgr::_addTestSceneObject()
{
	// Test Init
	std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();
	
	auto triangleVertex = new float[216]
	{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	};

	for (int i = 0; i < 12; i++)
	{
		std::shared_ptr<TrianglePrimitive> m_p3DObject = std::make_shared<TrianglePrimitive>();
		TrianglePrimitive& object = *m_p3DObject;
		object.m_vertices[0][0] = triangleVertex[18 * i];
		object.m_vertices[0][1] = triangleVertex[18 * i + 1];
		object.m_vertices[0][2] = triangleVertex[18 * i + 2];
		
		object.m_vertices[1][0] = triangleVertex[18 * i + 6];
		object.m_vertices[1][1] = triangleVertex[18 * i + 7];
		object.m_vertices[1][2] = triangleVertex[18 * i + 8];

		object.m_vertices[2][0] = triangleVertex[18 * i + 12];
		object.m_vertices[2][1] = triangleVertex[18 * i + 13];
		object.m_vertices[2][2] = triangleVertex[18 * i + 14];

		object.m_normalVec[0] = triangleVertex[18 * i + 3];
		object.m_normalVec[0] = triangleVertex[18 * i + 4];
		object.m_normalVec[0] = triangleVertex[18 * i + 5];

		pMesh->m_p3DObjectVec.push_back(m_p3DObject);
	}
	pMesh->m_nTriangleNumber = 12;
	pMesh->m_primitiveData.m_nVertexCount = 36;
	pMesh->m_primitiveData.m_nFloatCountPerVertex = 6;
	
	AddSceneObject(pMesh);
}
