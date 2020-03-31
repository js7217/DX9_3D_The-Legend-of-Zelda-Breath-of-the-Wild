#include "PhysXMgr.h"
#include "TimeMgr.h"

#include "Chiken.h"
#include "WizzRobe.h"
#include "Enemy_Giant.h"
#include "Ladder.h"
#include "IceCube.h"
#include "Bokoblin.h"
#include "WizzRobe.h"
#include "NxPlayer.h"
#include "StoneBall.h"
#include "TestBomb.h"
#include "SpurGear_A.h"
#include "AdvanceCam.h"
#include "MagnetBullet.h"
#include "Bullet_ArrowNormal.h"
#include "StoneBall_Big.h"
#include "DgnObj_IronDoorL.h"
#include "DgnObj_IronDoorR.h"
#include "Trigger_WindZone.h"
#include "Weapon_LargeMasterSword.h"
#include "Key_UI.h"
#include "Inventory_UI.h"
#include "Weapon_Melee.h"
#include "Weapon_Shield.h"
#include "Weapon_MeleeOneHand.h"
#include "Stone_Treasure_Box.h"
#include "Obj_TreeApple.h"
#include "Obj_Log_TreeApple.h"
#include "Obj_TreeMaple.h"
#include "Obj_Log_TreeMaple.h"
#include "Shop_Expendable.h"
#include "Enemy_Giant_Stone.h"
#include "FldObj_EntranceElevator.h"
#include "FldObj_MiniGame_LostKokko.h"
#include "Chiken.h"
#include "Trigger_BowlingZone.h"
#include "BowlPin.h"

#include "FireBall.h"
#include "Meteor.h"
#include "Shop_Tailor.h"
#include "TreasureBox_UI.h"
#include "Weapon_Ranged.h"
#include "Enemy_Giant.h"
#include "Quest_Ballon_UI.h"
#include "Quest_Koko_UI.h"
#include "Quest_Bowling_UI.h"
#include "MainQuest.h"

#include "Npc.h"
#include "DungeonOut_UI.h"
#include "Iron_Treasure_Box.h"
#include "MainQuest.h"
#include "Heart.h"

IMPLEMENT_SINGLETON(CPhysXMgr)

CPhysXMgr::CPhysXMgr()
	: m_pNxPhysicsSDK(nullptr)
{
	for (_uint i = 0; i < STAGE_END; ++i)
		m_pNxScene[i] = nullptr;

}

NxScene * CPhysXMgr::Get_NxScene(_uint iType)
{
	//if (99 == iType)
	//	return m_pNxScene[iType];
	//else
	return m_pNxScene[m_eScene];
}

HRESULT CPhysXMgr::SetUp_NxPhysX(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	NxSceneDesc SceneDesc;
	/*
	SceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;
	SceneDesc.internalThreadCount = 2;*/

	// PhysicsSDK 초기화
	m_pNxPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (nullptr == m_pNxPhysicsSDK)
	{
		MSG_BOX("m_pNxPhysicsSDK Create Failed");
		return E_FAIL;
	}

	// 충돌판정을 위한 경계의 두께? 설정
	// 정수일때의 박힌다.
	m_pNxPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

	m_pNxPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 3);	// Actor의 3축의 표시상 스케일 값.
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);	// 충돌 판정의 대상이 되는 형태. (0이면 표시하지않음)
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);	// 액터의 3충 표시(0이면 표시하지 않음)

																//하드웨어 유무 확인
	NxHWVersion HWVersion = m_pNxPhysicsSDK->getHWVersion();
	//사용가능하므로 플래그를 설정한다.

	if (HWVersion != NX_HW_VERSION_NONE)
		m_NxSceneDesc.simType = NX_SIMULATION_HW;	// 하드웨어로 사용.

													//중력
	m_NxSceneDesc.gravity = NxVec3(0.f, -19.62f, 0.f);
	//	m_NxSceneDesc.gravity = NxVec3(0.f, -9.62f, 0.f);

	//충돌 체크 콜백 
	m_pCallBack = new ContactCallBack();
	m_pTrigger = new TriggerCallBack();

	return S_OK;
}

HRESULT CPhysXMgr::SetUp_NxScene(PHYSX_SCENE eScene)
{
	m_eScene = eScene;
	//NxScene 작성
	m_pNxScene[eScene] = m_pNxPhysicsSDK->createScene(m_NxSceneDesc);

	if (nullptr == m_pNxScene[eScene])
	{
		MSG_BOX("m_pNxScene Create Failed");
		return E_FAIL;
	}

	// NxScene 에 이벤트 추가
	m_pNxScene[eScene]->setUserContactReport(m_pCallBack);
	// NxScene 에 Trigger 이벤트 추가
	m_pNxScene[eScene]->setUserTriggerReport(m_pTrigger);

	//메테리얼
	defaultMaterial = m_pNxScene[eScene]->getMaterialFromIndex(0);	//디폴트 메테리얼 가져오기
	defaultMaterial->setRestitution(0.0f);	// 반발계수(복원력)	(:: 1.f에 가까울수록 충돌 후 속도를 보존)
	defaultMaterial->setDynamicFriction(1.f); // 운동마찰계수
	defaultMaterial->setStaticFriction(1.f);	// 정지마찰계수

												//defaultMaterial->setFrictionCombineMode(NX_CM_MIN);				// 마찰의 기준.

	m_pNxScene[eScene]->setGroupCollisionFlag(0, 1, false);				// 0, 1번 그룹끼리는 서로 충돌하지 않는다.
	m_pNxScene[eScene]->setGroupCollisionFlag(1, 2, false);
	return S_OK;
}

HRESULT CPhysXMgr::Create_PhysxPlane()
{
	//평평한 물리 평면(지면==터레인)
	NxActorDesc actorDesc;
	actorDesc.setToDefault();
	actorDesc.name = "Terrain";
	actorDesc.group = 0;

	NxPlaneShapeDesc planeDesc;
	planeDesc.d = 0.f;	//평면의 높이
	planeDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING;
	planeDesc.name = "Terrain";
	actorDesc.shapes.pushBack(&planeDesc);

	m_UserDataPlane.ContactPairFlag = 0;
	actorDesc.userData = &m_UserDataPlane;

	//actorDesc.name = "Plane";
	//Physx Device에 물리적용 시켜달라고 등록
	if (m_eScene != CPhysXMgr::STAGE_END)
		m_pActorPlane = m_pNxScene[m_eScene]->createActor(actorDesc);

	//m_pNxScene->createMaterial(material)->getMaterialIndex();	//메테리얼 생성

	return S_OK;
}

void CPhysXMgr::Update_PhysxManager(const _double & TimeDelta)
{
	//호출 시 NxScene에 포함된 Actor들이 물리 작용을 시작함
	if (m_eScene != CPhysXMgr::STAGE_END)
	{
		m_pNxScene[m_eScene]->simulate(NxReal(TimeDelta));
		m_pNxScene[m_eScene]->flushStream();
	}

}

HRESULT CPhysXMgr::Render_PhysxCollider()
{
	Safe_Delete_Array(m_DebugRenderVertex);

	if (m_eScene == CPhysXMgr::STAGE_END)
		return E_FAIL;

	DWORD dwCnt = 0;
	const NxDebugRenderable* data = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->getDebugRenderable();

	if (nullptr == m_pDevice || nullptr == data)
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////
	// 버텍스 그리기
	//////////////////////////////////////////////////////////////////////////
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	_uint NbPoints = data->getNbPoints();
	if (NbPoints)
	{
		// 버텍스 카운트만큼 영역확보
		m_DebugRenderVertex = new CPhysXMgr::DEBUG_RENDER_VERTEX[NbPoints];
		if (m_DebugRenderVertex == nullptr)
			return E_FAIL;

		// 버텍스 데이터 취득
		const NxDebugPoint* Points = data->getPoints();
		dwCnt = 0;

		while (NbPoints--)
		{
			m_DebugRenderVertex[dwCnt].vPos = _vec3(Points->p.x, Points->p.y, Points->p.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Points->color;
			dwCnt++;
			Points++;
		}

		Render_Buffer(D3DPT_LINELIST, data->getNbLines());

		Safe_Delete_Array(m_DebugRenderVertex);
	}

	//////////////////////////////////////////////////////////////////////////
	// 폴리곤 렌더링
	//////////////////////////////////////////////////////////////////////////

	_uint NbTriangles = data->getNbTriangles();
	if (NbTriangles)
	{
		//폴리곤 개수만큼 영역확보
		m_DebugRenderVertex = new CPhysXMgr::DEBUG_RENDER_VERTEX[NbTriangles * 3];
		if (m_DebugRenderVertex == nullptr)
			return E_FAIL;

		const NxDebugTriangle* Triangles = data->getTriangles();

		dwCnt = 0;

		while (NbTriangles--)
		{
			m_DebugRenderVertex[dwCnt].vPos = _vec3(Triangles->p0.x, Triangles->p0.y, Triangles->p0.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Triangles->color;
			dwCnt++;

			m_DebugRenderVertex[dwCnt].vPos = _vec3(Triangles->p1.x, Triangles->p1.y, Triangles->p1.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Triangles->color;
			dwCnt++;

			m_DebugRenderVertex[dwCnt].vPos = _vec3(Triangles->p2.x, Triangles->p2.y, Triangles->p2.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Triangles->color;
			dwCnt++;

			Triangles++;
		}

		Render_Buffer(D3DPT_TRIANGLELIST, data->getNbTriangles());

		Safe_Delete_Array(m_DebugRenderVertex);
	}

	//////////////////////////////////////////////////////////////////////////
	// 선 그리기
	//////////////////////////////////////////////////////////////////////////

	_uint NbLines = data->getNbLines();
	if (NbLines)
	{
		m_DebugRenderVertex = new CPhysXMgr::DEBUG_RENDER_VERTEX[NbLines * 2];
		if (m_DebugRenderVertex == NULL)
			return E_FAIL;

		const NxDebugLine* Lines = data->getLines();

		dwCnt = 0;

		while (NbLines--)
		{
			m_DebugRenderVertex[dwCnt].vPos = _vec3(Lines->p0.x, Lines->p0.y, Lines->p0.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Lines->color;
			dwCnt++;

			m_DebugRenderVertex[dwCnt].vPos = _vec3(Lines->p1.x, Lines->p1.y, Lines->p1.z);
			m_DebugRenderVertex[dwCnt].color = (DWORD)Lines->color;
			dwCnt++;

			Lines++;
		}

		Render_Buffer(D3DPT_LINELIST, data->getNbLines());

		Safe_Delete_Array(m_DebugRenderVertex);
	}

	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	return S_OK;
}

HRESULT CPhysXMgr::Render_Buffer(const D3DPRIMITIVETYPE Type, const int VertexCount)
{
	DWORD					m_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pDevice->SetTexture(0, NULL);
	m_pDevice->SetFVF(m_FVF);
	m_pDevice->DrawPrimitiveUP(Type, VertexCount, m_DebugRenderVertex, sizeof(CPhysXMgr::DEBUG_RENDER_VERTEX));
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CPhysXMgr::Render_PhysxEnd()
{
	if (m_eScene != CPhysXMgr::STAGE_END)
	{
		m_pNxScene[m_eScene]->fetchResults(NX_RIGID_BODY_FINISHED, TRUE);
	}
}

_bool CPhysXMgr::NxPicking_IceCube(HWND hWnd, _vec3 * pOutPos)
{
	POINT ptMouse = {};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos{};
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pDevice->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 1.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);

	RAYCAST_ICECUBE raycastCallBack;
	NxRay			WorldRay;

	WorldRay.dir = NxVec3(vRayDir);
	WorldRay.dir.normalize();
	WorldRay.orig = NxVec3(vRayPos);

	m_pNxScene[m_eScene]->raycastAllShapes(WorldRay, raycastCallBack, NX_STATIC_SHAPES);

	if (raycastCallBack.Get_IsHit())
	{
		*pOutPos = raycastCallBack.Get_Pos();
		return true;
	}
	else
	{
		pOutPos = nullptr;
		return false;
	}
}

_bool CPhysXMgr::NxPicking_TimeStop(HWND hWnd, _vec3 * pOutPos)
{
	POINT ptMouse = {};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos{};
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pDevice->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);

	RAYCAST_TIMESTOP raycastCallBack;
	NxRay			WorldRay;

	WorldRay.dir = NxVec3(vRayDir);
	WorldRay.dir.normalize();
	WorldRay.orig = NxVec3(vRayPos);


	m_pNxScene[m_eScene]->raycastAllShapes(WorldRay, raycastCallBack, NX_DYNAMIC_SHAPES);

	if (raycastCallBack.Get_IsHit())
	{
		*pOutPos = raycastCallBack.Get_Pos();
		return true;
	}
	else
	{
		pOutPos = nullptr;
		return false;
	}
}

_bool CPhysXMgr::NxPicking_Ladder(const NxVec3 & vOrig, const NxVec3 & vDir)
{
	RAYCAST_LADDER Raycast;
	NxRay			WorldRay;

	WorldRay.dir = vDir;
	WorldRay.orig = vOrig;

	GET_INSTANCE(CPhysXMgr)->Get_NxScene()->raycastAllShapes(WorldRay, Raycast, NX_STATIC_SHAPES);
	return true;
}

_bool CPhysXMgr::RayCast_ClosestShapes(_vec3 * pOutPos, CAMERA_DESC pDesc)
{
	NxRay		CamRay;
	NxVec3	nxLook = (pDesc.vEye - pDesc.vAt);
	NxReal	rDistance = nxLook.magnitude();
	nxLook.normalize();

	CamRay.orig = (NxVec3)pDesc.vAt;
	CamRay.dir = nxLook;

	NxRaycastHit pHit;

	m_pNxScene[m_eScene]->raycastClosestShape(CamRay, NX_ALL_SHAPES, pHit);

	if (pHit.shape != nullptr && pHit.distance < rDistance)
	{
		const char* pName = pHit.shape->getActor().getName();

		if (strcmp(pName, "Camera_Eye") != 0 && strcmp(pName, "Player") != 0 && strcmp(pName, "Camera") != 0)
		{
			*pOutPos = { pHit.worldImpact.x,  pHit.worldImpact.y, pHit.worldImpact.z };
			return TRUE;
		}
		pOutPos = nullptr;
		return FALSE;
	}
	else
	{
		pOutPos = nullptr;
		return FALSE;
	}

	return FALSE;
}

_bool CPhysXMgr::RayCast_RefelectClosest(_vec3* pFireBallPos, _vec3* pFireBallDir, _vec3* pOutPos, _vec3* pOutNormal)
{
	NxRay	RayCast;
	NxVec3	nxRayCastLook = *pFireBallDir;
	NxReal	rDist = nxRayCastLook.magnitude();
	nxRayCastLook.normalize();

	RayCast.orig = (NxVec3)*pFireBallPos;
	RayCast.dir = nxRayCastLook;

	NxRaycastHit pHit;

	m_pNxScene[m_eScene]->raycastClosestShape(RayCast, NX_ALL_SHAPES, pHit);

	if (pHit.shape != nullptr && pHit.distance < rDist)
	{
		const char* pName = pHit.shape->getActor().getName();

		if (strcmp(pName, "FireBall") != 0 && strcmp(pName, "WizzRobe") != 0)
		{
			if (strcmp(pName, "Terrain") == 0)
			{
				_vec3 vHitWorldNormal = { pHit.worldNormal.x, pHit.worldNormal.y, pHit.worldNormal.z };
				_vec3 vReflectedDir;

				vReflectedDir = *pFireBallDir + 2 * vHitWorldNormal * D3DXVec3Dot(&(*pFireBallDir * -1), &vHitWorldNormal);

				*pOutPos = { pHit.worldImpact.x, pHit.worldImpact.y, pHit.worldImpact.z };
				*pOutNormal = vReflectedDir;
			}
			else
			{
				*pOutPos = { pHit.worldImpact.x, pHit.worldImpact.y, pHit.worldImpact.z };
				*pOutNormal = { pHit.worldNormal.x, pHit.worldNormal.y, pHit.worldNormal.z };
			}

			return true;
		}
	}
	else
	{
		pOutPos = nullptr;
		pOutNormal = nullptr;
		return false;
	}
	return FALSE;
}

_bool CPhysXMgr::RayCast_FowardObjectNormal(_vec3* pPlayerPos, _vec3* pPlayerDir, _vec3* pOutPos, _vec3* pOutNormal)
{
	NxRay	RayCast;
	NxVec3	nxRayCastLook = *pPlayerPos;
	//NxReal	rDist = nxRayCastLook.magnitude();
	NxReal	rDist = 2.f;
	nxRayCastLook.normalize();

	RayCast.orig = (NxVec3)*pPlayerPos;
	RayCast.dir = nxRayCastLook;

	NxRaycastHit pHit;

	if (STAGE_TEST == m_eScene)
		return false;

	m_pNxScene[m_eScene]->raycastClosestShape(RayCast, NX_ALL_SHAPES, pHit);

	if (pHit.shape != nullptr && pHit.distance < rDist)
	{
		const char* pName = pHit.shape->getActor().getName();

		if (strcmp(pName, "Terrain") == 0)
		{
			*pOutPos = { pHit.worldImpact.x, pHit.worldImpact.y, pHit.worldImpact.z };
			*pOutNormal = { pHit.worldNormal.x, pHit.worldNormal.y, pHit.worldNormal.z };

			return true;
		}
	}
	else
	{
		pOutPos = nullptr;
		pOutNormal = nullptr;
		return false;
	}

	return FALSE;
}

_bool CPhysXMgr::RayCast_ClosestShapes_Back(CAMERA_DESC pDesc)
{
	NxRay		CamRay;
	NxVec3	nxLook = (pDesc.vEye - pDesc.vAt);
	nxLook.normalize();

	CamRay.orig = (NxVec3)pDesc.vAt;
	CamRay.dir = nxLook;

	NxRaycastHit pHit;

	m_pNxScene[m_eScene]->raycastClosestShape(CamRay, NX_ALL_SHAPES, pHit);

	if (pHit.shape != nullptr && pHit.distance < 6.f)
	{
		const char* pName = pHit.shape->getActor().getName();

		if (strcmp(pName, "Camera_Eye") != 0 && strcmp(pName, "Player") != 0 && strcmp(pName, "Camera") != 0 &&
			strcmp(pName, "Weapon_Shield") != 0 && strcmp(pName, "Shield_Sub") != 0)
			return FALSE;

	}

	return TRUE;
}

_bool CPhysXMgr::RayCast_ClosestTerrain(_vec3 * pOutPos, _vec3 vPos)
{
	NxRay		CamRay;
	NxVec3	nxLook(0.f, -1.f, 0.f);
	nxLook.normalize();

	CamRay.orig = (NxVec3)vPos;
	CamRay.dir = nxLook;

	NxRaycastHit pHit;

	m_pNxScene[m_eScene]->raycastClosestShape(CamRay, NX_ALL_SHAPES, pHit);

	if (pHit.shape != nullptr)
	{
		const char* pName = pHit.shape->getActor().getName();

		if (strcmp(pName, "Camera_Eye") != 0 && strcmp(pName, "Player") != 0 && strcmp(pName, "Camera") != 0)
		{
			*pOutPos = { pHit.worldImpact.x,  pHit.worldImpact.y, pHit.worldImpact.z };
			return TRUE;
		}
		pOutPos = nullptr;
		return FALSE;
	}
	else
	{
		pOutPos = nullptr;
		return FALSE;
	}

	return FALSE;
}

_bool CPhysXMgr::Raycast_ClosestIceCube(HWND hWnd, _vec3 * pOutPos)
{
	POINT ptMouse = {};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos{};
	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pDevice->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 1.f;

	//투영 -> 뷰스페이스
	_matrix matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	_vec3 vRayDir, vRayPos;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);

	//RAYCAST_ICECUBE raycastCallBack;
	NxRaycastHit pHit;
	NxRay			WorldRay;

	WorldRay.dir = NxVec3(vRayDir);
	WorldRay.dir.normalize();
	WorldRay.orig = NxVec3(vRayPos);

	m_pNxScene[m_eScene]->raycastClosestShape(WorldRay, NX_STATIC_SHAPES, pHit);

	if (pHit.shape != nullptr)
	{
		const char* pName = pHit.shape->getActor().getName();
		const char* pShapeName = pHit.shape->getName();

		if (strcmp(pName, "Camera_Eye") != 0 && strcmp(pName, "Player") != 0 && strcmp(pName, "Camera") != 0)
		{
			if (nullptr != pShapeName)
			{
				if (strcmp(pShapeName, "Trigger_Water") == 0)
				{
					*pOutPos = { pHit.worldImpact.x,  pHit.worldImpact.y, pHit.worldImpact.z };
					return TRUE;
				}
			}
		}
	}

	pOutPos = nullptr;
	return FALSE;
}



//HRESULT CPhysXMgr::Insert_MapActor(const _tchar* szName, NxActor * pActor) 
//{
//	auto& iter = m_MapActor[m_eScene].find(szName);
//
//	if (iter == m_MapActor[m_eScene].end())
//	{
//		list<NxActor*> pList;
//		pList.emplace_back(pActor);
//
//		//m_MapActor[m_eScene].insert({szName, pList});
//		m_MapActor[m_eScene].emplace(szName, pList);
//	}
//	else if (iter != m_MapActor[m_eScene].end())
//	{
//		iter->second.emplace_back(pActor);
//	}
//	
//	return S_OK;
//}
//
//list<NxActor*>& CPhysXMgr::Get_ActorList(const _tchar * szName)
//{
//	auto iter_find = m_MapActor[m_eScene].find(szName);
//
//	return iter_find->second;
//}
//
//NxActor* CPhysXMgr::Find_MapActor(const _tchar * szName, _uint iIndex)
//{
//	auto iter = m_MapActor[m_eScene].find(szName);
//
//	if (iter == m_MapActor[m_eScene].end())
//		return nullptr;
//
//	auto& listFinder = iter->second.front();
//
//	for (_uint i = 0; i < iIndex; ++i)
//		++listFinder;
//		
//	return listFinder;
//}

HRESULT CPhysXMgr::Set_ContactActor(const char * Actor1, const char * Actor2)
{
	size_t iCnt = m_pNxScene[m_eScene]->getNbActors();

	const char* pActor1;
	const char* pActor2;

	for (_uint i = 0; i < iCnt; ++i)
	{
		pActor1 = m_pNxScene[m_eScene]->getActors()[i]->getName();

		if (strcmp(pActor1, Actor1) == 0)
		{
			for (_uint j = 0; j < iCnt; ++j)
			{
				pActor2 = m_pNxScene[m_eScene]->getActors()[j]->getName();

				if (strcmp(pActor2, Actor2) == 0)
				{
					m_pNxScene[m_eScene]->setActorPairFlags(*m_pNxScene[m_eScene]->getActors()[i], *m_pNxScene[m_eScene]->getActors()[j], NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH);
				}
			}
		}
	}


	return S_OK;
}

void CPhysXMgr::Free(void)
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pDevice);
	Safe_Delete(m_pCallBack);
	Safe_Delete(m_pTrigger);


	if (m_pNxPhysicsSDK != nullptr)
	{
		if (m_pNxScene != nullptr)
		{
			for (_uint i = 0; i < STAGE_END; ++i)
			{
				if (m_pNxScene[i] == nullptr)
					continue;

				NxU32 actorCount = m_pNxScene[i]->getNbActors();

				for (NxU32 j = 0; j < actorCount; ++j)
				{
					NxActor* actor = m_pNxScene[i]->getActors()[j];
					_int iShapeCnt = actor->getNbShapes();

					for (_int i = 0; i < iShapeCnt; ++i)
						actor->releaseShape(*actor->getShapes()[i]);

					m_pNxScene[i]->releaseActor(*actor);
				}

				m_pNxPhysicsSDK->releaseScene(*m_pNxScene[i]);
				m_pNxScene[i] = nullptr;
			}

		}

		NxReleasePhysicsSDK(m_pNxPhysicsSDK);
		m_pNxPhysicsSDK = nullptr;
	}

}

void CPhysXMgr::ContactCallBack::onContactNotify(NxContactPair & pair, NxU32 events)
{
	NxVec3 nxVelocity_0, nxVelocity_1;
	NxReal Real_0, Real_1;

	const char* pActor1 = pair.actors[0]->getName();
	const char* pActor2 = pair.actors[1]->getName();
	const char* pTime = pair.actors[1]->getShapes()[0]->getName();

	switch (events)
	{
		// 충돌 시작 이벤트 통지
	case NX_NOTIFY_ON_START_TOUCH:
		if (nullptr != pTime)
		{
			if (strcmp(pActor1, "Player") != 0 && strcmp(pTime, "TimeStop") == 0)
			{
				//CStoneBall_Big* pBall = dynamic_cast<CStoneBall_Big*>((CGameObject*)pair.actors[0]->userData);

				nxVelocity_0 = pair.actors[1]->getLinearVelocity();
				Real_0 = nxVelocity_0.magnitude();

				if (Real_0 > 10.f)
				{
					CGameObject* pObject = (CGameObject*)pair.actors[0]->userData;

					if (pObject != nullptr)
						pObject->Set_GameObject(TRUE);
				}
			}
		}

		if (strcmp(pActor1, "Player") == 0 && strcmp(pActor2, "StoneBall_Big") == 0)
		{
			nxVelocity_0 = pair.actors[0]->getLinearVelocity();
			Real_0 = nxVelocity_0.magnitude();

			nxVelocity_1 = pair.actors[1]->getLinearVelocity();
			Real_1 = nxVelocity_1.magnitude();

			if (Real_1 > 5.0)
				break;
		}

		if (strcmp(pActor1, "Player") == 0
			|| strcmp(pActor2, "Component_Mesh_Dungeon41") == 0
			|| strcmp(pActor2, "Component_Mesh_Dungeon65") == 0
			|| strcmp(pActor2, "Component_Mesh_Dungeon9") == 0
			|| strcmp(pActor2, "Component_Mesh_Dungeon38") == 0)
		{
			pair.actors[0]->setLinearVelocity(NxVec3(0.f, 0.f, 0.f));
			break;
		}

		if (strcmp(pActor1, "DgnObj_DungeonEntrance") == 0 && strcmp(pActor2, "Player") == 0)
		{
			_int iNum = *(_int*)pair.actors[0]->getShapes()[1]->userData;
			GET_INSTANCE(CPhysXMgr)->Set_SceneNum(iNum);

		}
		break;

		// 충돌 완료 이벤트통지
	case NX_NOTIFY_ON_END_TOUCH:
		break;

	case  NX_NOTIFY_ON_TOUCH:
		break;

	}
}

void CPhysXMgr::TriggerCallBack::onTrigger(NxShape & triggerShape, NxShape & otherShape, NxTriggerFlag status)
{
	if (status & NX_TRIGGER_ON_STAY)
	{
		// 사다리 타기
		const char* szSrc = triggerShape.getActor().getName();
		const char* szDest = otherShape.getActor().getName();
		const char* pDestShape = otherShape.getName();
		const char* pSrcShape = triggerShape.getName();

		if (szDest != nullptr && szSrc != nullptr)
		{

			if (strcmp(szDest, "Player") == 0)
			{

				if (strcmp(szSrc, "IceCube_Ladder") == 0)
				{
					GET_INSTANCE(CPhysXMgr)->Set_Ladder(TRUE);

					NxMat34 NxMat = triggerShape.getGlobalPose();
					if (NxMat.t.y > otherShape.getGlobalPose().t.y)
					{
						NxMat = otherShape.getGlobalPose();
						GET_INSTANCE(CPhysXMgr)->Set_NxMatLadder(NxMat);
					}
					else if (NxMat.t.y < otherShape.getGlobalPose().t.y)
					{
						NxMat = otherShape.getGlobalPose();
						GET_INSTANCE(CPhysXMgr)->Set_NxMatLadder(NxMat);
					}
				}

				if (strcmp(szSrc, "Camera_Eye") == 0)
				{
					CNxPlayer* pDest = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);
					pDest->Set_RenderState(FALSE);
				}

				if (strcmp(szSrc, "Obj_TouchTerminal") == 0)
				{
					CNxPlayer* pSrc = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);

					if (pSrc->Get_GrabState())
					{
						pSrc->Flag_On_State(FLAG_TOUCH);
						pSrc->Set_GrabState(FALSE);
					}
				}

				if (m_bIsTouch_Create_SeekerSkill)
				{
					CNxPlayer* pSrc = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);

					if (pSrc->Get_GrabState())
					{
						pSrc->Flag_On_State(FLAG_TOUCH);
						pSrc->Set_GrabState(FALSE);
					}
				}

				// 필드 던전  엘리베이터 작동 트리거
				if (strcmp(szSrc, "FldObj_EntranceElevator") == 0 && strcmp(szDest, "Player") == 0)
				{
					CNxPlayer* pSrc = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);
					CFldObj_EntranceElevator* pElevator = dynamic_cast<CFldObj_EntranceElevator*>((CGameObject*)triggerShape.getActor().userData);
					if (pSrc->Get_GrabState())
					{
						pSrc->Flag_On_State(FLAG_DUNGEON_ENTRANCE);
						pSrc->Set_GrabState(FALSE);
					}
					GET_INSTANCE(CManagement)->Set_WarpPoint(pElevator->Get_WarpPoint());
				}


				if (pSrcShape != nullptr && strcmp(pSrcShape, "Dgn_End") == 0)	//던전 탈출 팝업 처리
				{
					//GET_INSTANCE(CPhysXMgr)->Set_SceneChangeNum(4);	//필드 Scene
				}

				if (strcmp(szSrc, "Trigger_Door") == 0 || strcmp(szSrc, "Trigger_HotelDoor") == 0)
				{

					if (strcmp(pSrcShape, "Door_In") == 0)
					{
						CGameObject* pGameObj = (CGameObject*)triggerShape.getActor().userData;
						pGameObj->Set_OnHand(TRUE);
					}

					if (strcmp(pSrcShape, "Door_Out") == 0)
					{
						CGameObject* pGameObj = (CGameObject*)triggerShape.getActor().userData;

						pGameObj->Set_OnHand(FALSE);

					}
				}

				if (strcmp(szSrc, "Shop_Milk") == 0)	//Shop
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.
					CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
					if (pShop_Expendable == nullptr)
						return;
					pShop_Expendable->Set_UI_RenderMilk(true);
					pShop_Expendable->Set_CurFoodState(0);
					pShop_Expendable->Set_UI_Name(L"신선한 우유");

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Expendable->Get_m_bIsDialog() && !pShop_Expendable->Get_bIsEnter())
						pKey_UI->Set_IsDraw(true, 0);
					//
				}
				if (strcmp(szSrc, "Mashroom_Max") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.
					CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
					if (pShop_Expendable == nullptr)
						return;
					pShop_Expendable->Set_UI_RenderMax(true);
					pShop_Expendable->Set_CurFoodState(1);
					pShop_Expendable->Set_UI_Name(L"맥스트러플");
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Expendable->Get_m_bIsDialog() && !pShop_Expendable->Get_bIsEnter())
						pKey_UI->Set_IsDraw(true, 0);


				}
				if (strcmp(szSrc, "MashroomStemina") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.
					CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
					if (pShop_Expendable == nullptr)
						return;
					pShop_Expendable->Set_UI_RenderStemina(true);
					pShop_Expendable->Set_CurFoodState(2);
					pShop_Expendable->Set_UI_Name(L"원기 버섯");

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Expendable->Get_m_bIsDialog() && !pShop_Expendable->Get_bIsEnter())
						pKey_UI->Set_IsDraw(true, 0);
				}
				if (strcmp(szSrc, "Bird_Egg") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.
					CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
					if (pShop_Expendable == nullptr)
						return;
					pShop_Expendable->Set_UI_RenderEgg(true);
					pShop_Expendable->Set_CurFoodState(3);
					pShop_Expendable->Set_UI_Name(L"새의 알");

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Expendable->Get_m_bIsDialog() && !pShop_Expendable->Get_bIsEnter())
						pKey_UI->Set_IsDraw(true, 0);
				}
				if (strcmp(szSrc, "hylian_rice") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.
					CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
					if (pShop_Expendable == nullptr)
						return;
					pShop_Expendable->Set_UI_RenderRice(true);
					pShop_Expendable->Set_CurFoodState(4);
					pShop_Expendable->Set_UI_Name(L"헤이랄 쌀");

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Expendable->Get_m_bIsDialog() && !pShop_Expendable->Get_bIsEnter())
						pKey_UI->Set_IsDraw(true, 0);
				}

				if (strcmp(szSrc, "Shop_Head_Front") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);



					pShop_Tailor->Set_TailorType(0);
					pShop_Tailor->Set_BoardRender(true);
				}
				if (strcmp(szSrc, "Shop_Lower_Front") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					pShop_Tailor->Set_TailorType(1);
					pShop_Tailor->Set_BoardRender(true);

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);
				}
				if (strcmp(szSrc, "Shop_Upper_Front") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					pShop_Tailor->Set_TailorType(2);
					pShop_Tailor->Set_BoardRender(true);

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);

				}
				if (strcmp(szSrc, "Shop_Head_Behind") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					pShop_Tailor->Set_TailorType(3);
					pShop_Tailor->Set_BoardRender(true);

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);

				}
				if (strcmp(szSrc, "Shop_Upper_Behind") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					pShop_Tailor->Set_TailorType(4);
					pShop_Tailor->Set_BoardRender(true);

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);

				}
				if (strcmp(szSrc, "Shop_Lower_Behind") == 0)
				{
					// 아이템을 이벤토리로 넣어준다
					CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

					_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
					if (pShop_Tailor == nullptr)
						return;

					pShop_Tailor->Set_TailorType(5);
					pShop_Tailor->Set_BoardRender(true);

					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					if (!pShop_Tailor->Get_DialogRender())
						pKey_UI->Set_IsDraw(true, 0);

				}

			}///

			if (strcmp(szSrc, "Player_Grab") == 0 && strcmp(szDest, "StoneBox") == 0)	//Stay
			{
				CGameObject* pDestObj = (CGameObject*)otherShape.getActor().userData;
				CStone_Treasure_Box* pStoneBox = dynamic_cast<CStone_Treasure_Box*>(pDestObj);

				CGameObject* pGameObj = (CGameObject*)triggerShape.getActor().userData;
				CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pGameObj);

				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CTreasureBox_UI* pTreasureBox_UI = (CTreasureBox_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_TreasureBox_UI", 0);
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pTreasureBox_UI == nullptr || pInven_UI == nullptr)
					return;

				pInven_UI->Set_ItemBox(pStoneBox);

				//if(pStoneBox->Get_iNum() == 0)
				//던전1에서 나오는 무기들 // 9번 씬
				if (iSceneNum == 9)
				{
					pTreasureBox_UI->Set_IsNum(pStoneBox->Get_iNum());

				}
				//던전 2에서 나오는 무기들 // 10번 씬
				if (iSceneNum == 10)
					pTreasureBox_UI->Set_IsNum(pStoneBox->Get_iNum());
				//던전 3에서 나오는 무기들 // 11번 씬
				if (iSceneNum == 11)
					pTreasureBox_UI->Set_IsNum(pStoneBox->Get_iNum());
				//던전 4에서 나오는 무기들 // 12번 씬
				if (iSceneNum == 12)
					pTreasureBox_UI->Set_IsNum(pStoneBox->Get_iNum());
				if (iSceneNum == 8)
					pTreasureBox_UI->Set_IsNum(pStoneBox->Get_iNum());

				if (pPlayer->Get_GrabState())
				{
					//pPlayer->Set_pGameObjectHolder(pDestObj, L"StoneBox");
					if (false == pStoneBox->Get_bIsOpened())
					{
						pPlayer->Set_eActionStatus(STATE_EVENTOPENBOX);
						pStoneBox->Set_Animation(2);
						pStoneBox->Set_CurrentAniIndex(2);
						pPlayer->Set_GrabState(FALSE);
					}
				}
			}

			if (strcmp(szSrc, "Player_Grab") == 0 && strcmp(szDest, "Player") != 0)
			{

				CNxPlayer* pSrc = dynamic_cast<CNxPlayer*>((CGameObject*)triggerShape.getActor().userData);

				if (pSrc->Get_GrabState())
				{
					auto& iter = m_DistMap.begin();

					if (iter != m_DistMap.end())
					{
						pSrc->Set_GrabState(FALSE);
						iter->second->Set_GrabState(TRUE);
						m_DistMap.erase(iter);
					}

				}
			}///

			if (strcmp(szSrc, "Trigger_WindZone") == 0)
			{
				//if (strcmp(szDest, "WindTrail") == 0)
				//{
				//	CTrigger_WindZone* pZone = (CTrigger_WindZone*)triggerShape.getActor().userData;
				//	NxVec3 nxWind = pZone->Get_NxWindForec();
				//	NxVec3 nxVelocity = otherShape.getActor().getLinearVelocity();
				//	nxVelocity += nxWind;
				//	nxVelocity.y += NxMath::rand(-0.25f, 0.25f);
				//	if (nxVelocity.x > 8.f)
				//		nxVelocity.x = 8.f;
				//	else if (nxVelocity.x < -8.f)
				//		nxVelocity.x = -8.f;
				//	//otherShape.getActor().addForce(nxVelocity, NX_SMOOTH_VELOCITY_CHANGE, true);
				//	otherShape.getActor().setLinearVelocity(nxVelocity);
				//}

				if (strcmp(szDest, "Obj_YabusameTarget_A") == 0)
				{
					CTrigger_WindZone* pZone = (CTrigger_WindZone*)triggerShape.getActor().userData;

					NxVec3 nxWind = pZone->Get_NxWindForec();
					NxVec3 nxVelocity = otherShape.getActor().getLinearVelocity();
					nxVelocity += nxWind;
					nxVelocity.y += 0.01f;
					if (nxVelocity.x > 4.f)
						nxVelocity.x = 4.f;
					else if (nxVelocity.x < -4.f)
						nxVelocity.x = -4.f;
					//otherShape.getActor().addForce(nxVelocity, NX_SMOOTH_VELOCITY_CHANGE, true);
					otherShape.getActor().setLinearVelocity(nxVelocity);
				}

				if (strcmp(szDest, "Player") == 0)
				{
					CNxPlayer* pDest = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);

					if (pDest->m_iPlayerFlags & FLAG_PARSTOLE)
					{
						CTrigger_WindZone* pZone = (CTrigger_WindZone*)triggerShape.getActor().userData;
						NxVec3 nxWind = pZone->Get_NxWindForec();

						NxVec3 nxVelocity = otherShape.getActor().getLinearVelocity();
						nxVelocity += nxWind;
						if (nxVelocity.x > 4.f)
							nxVelocity.x = 4.f;
						else if (nxVelocity.x < -4.f)
							nxVelocity.x = -4.f;
						otherShape.getActor().setLinearVelocity(nxVelocity);
					}
				}
			}

			//폭발 시 부서지는 블럭 없에는 부분
			if (strcmp(szSrc, "KaBomb") == 0 && strcmp(szDest, "BlockCrack_A") == 0)
			{
				//		otherShape.getActor().setGlobalPosition(NxVec3(-100.f, -100.f, 0.f));
				otherShape.getActor().setName("Dead");
			}


		}
	}

	if (status & NX_TRIGGER_ON_ENTER)
	{
		const char* szSrc = triggerShape.getActor().getName();
		const char* szDest = otherShape.getActor().getName();

		const char* pSrcShape = triggerShape.getName();
		const char* pDestShape = otherShape.getName();

		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

		if (strcmp(szDest, "Player") == 0)
		{
			const char* pSrcShape = triggerShape.getName();

			if (pSrcShape != nullptr && strcmp(pSrcShape, "Obj_Log_TreeApple") == 0)
			{
				CNxPlayer* pPlayer = (CNxPlayer*)otherShape.getActor().userData;
				CObj_Log_TreeApple* pLog = (CObj_Log_TreeApple*)triggerShape.getActor().userData;

				if (TRUE == pLog->Get_bCheckHit())
				{
					NxVec3 vDir = otherShape.getActor().getGlobalPosition() - triggerShape.getGlobalPosition();
					vDir.y = 0;
					vDir.normalize();

					pPlayer->Set_HitAction();
					pPlayer->Set_HitPushDir(_vec3(vDir.x, vDir.y, vDir.z));
					pPlayer->Set_PlayerHp(4);
				}
			}
			if (pSrcShape != nullptr && strcmp(pSrcShape, "Obj_Log_TreeMaple") == 0)
			{
				CNxPlayer* pPlayer = (CNxPlayer*)otherShape.getActor().userData;
				CObj_Log_TreeMaple* pLog = (CObj_Log_TreeMaple*)triggerShape.getActor().userData;

				if (TRUE == pLog->Get_bCheckHit())
				{
					NxVec3 vDir = otherShape.getActor().getGlobalPosition() - triggerShape.getGlobalPosition();
					vDir.y = 0;
					vDir.normalize();

					pPlayer->Set_HitAction();
					pPlayer->Set_HitPushDir(_vec3(vDir.x, vDir.y, vDir.z));
					pPlayer->Set_PlayerHp(4);
				}
			}

			if (pSrcShape != nullptr && strcmp(pSrcShape, "Dgn_End") == 0)	//던전 탈출 팝업 처리
			{
				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(true, 4);

				CDungeonOut_UI* pOut_UI = (CDungeonOut_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Dungeon_UI", 0);
				if (pOut_UI == nullptr)
					return;

				pOut_UI->Set_IsDraw(true);
			}

			// Touch Terminal Trigger
			if (strcmp(szSrc, "Create_SeekerSkill") == 0)
			{
				m_bIsTouch_Create_SeekerSkill = true;

				//
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(true, 1);
				//
			}

			// TwnObj_Touch Terminal Trigger
			if (strcmp(szSrc, "Obj_TouchTerminal") == 0)
			{
				m_pDgnShutter = (CGameObject*)triggerShape.getActor().userData;

				NxVec3 vPos = triggerShape.getActor().getGlobalPosition();
				NxVec3 vLook = triggerShape.getActor().getGlobalPose().M.getColumn(2);
				vLook.normalize();
				vPos.x -= 0.5f * vLook.x;
				vPos.z -= 0.5f * vLook.z;

				m_vTerminalPos = vPos;
				//
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(true, 0);
				//
			}

			// FldObj_EntranceElevator
			if (strcmp(szSrc, "FldObj_EntranceElevator") == 0)
			{
				//
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(true, 0);
				//

				m_pEntranceElevator = (CGameObject*)triggerShape.getActor().userData;
				m_vTerminalPos = triggerShape.getActor().getGlobalPosition();
			}

			// DgnObj_DungeonEntrance
			if (strcmp(szSrc, "DgnObj_DungeonEntrance") == 0)
			{
				GET_INSTANCE(CPhysXMgr)->Set_SceneChangeNum(*(_int*)triggerShape.userData);
			}

			if (strcmp(szSrc, "Trigger_Sound") == 0)
			{
				CGameObject* pPot = (CGameObject*)triggerShape.getActor().userData;
				pPot->Set_EnterWater(TRUE);	//사운드 트리거 작동
			}

			if (strcmp(szSrc, "Trigger_Door") == 0 || strcmp(szSrc, "Trigger_HotelDoor") == 0)
			{
				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);

				if (pKey_UI == nullptr)
					return;

				if (!pKey_UI->Get_IsDoor())
					pKey_UI->Set_IsDraw(true, 3);
				//
			}
			else if (strcmp(szSrc, "Door_Out") == 0)
			{
				//UI
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);

				if (pKey_UI == nullptr)
					return;

				if (!pKey_UI->Get_IsDoor())
					pKey_UI->Set_IsDraw(true, 3);
				//
			}


			if (strcmp(szSrc, "Weapon_MeleeOneHand") == 0 && strcmp(szDest, "Player") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				ITEM tItemInfo = dynamic_cast<CWeapon_MeleeOneHand*>(szSrc)->Get_ItemInfo();
				tItemInfo.iNum;

				if (tItemInfo.eItemStatus == ITEM_FIELD)
				{
					//UI는 필드일 때만 깜빡인다.
					//UI
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(true, 1);
					//
					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.

					CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
					if (pInven_UI == nullptr)
						return;
					pInven_UI->Set_Field_Item(szSrc);
					pInven_UI->Set_ItemInfo(&tItemInfo);
				}
			}

			if (strcmp(szSrc, "Weapon_Melee") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				ITEM tItemInfo = dynamic_cast<CWeapon_Melee*>(szSrc)->Get_ItemInfo();
				tItemInfo.iNum;

				if (tItemInfo.eItemStatus == ITEM_FIELD)
				{
					//UI는 필드일 때만 깜빡인다.
					//UI
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(true, 1);
					//
					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.

					CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
					if (pInven_UI == nullptr)
						return;
					pInven_UI->Set_Field_Item(szSrc);
					pInven_UI->Set_ItemInfo(&tItemInfo);
				}
			}

			//Weapon_Ranged
			if (strcmp(szSrc, "Weapon_Ranged") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				ITEM tItemInfo = dynamic_cast<CWeapon_Ranged*>(szSrc)->Get_ItemInfo();
				tItemInfo.iNum;

				if (tItemInfo.eItemStatus == ITEM_FIELD)
				{
					//UI는 필드일 때만 깜빡인다.
					//UI
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(true, 1);
					//
					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.

					CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
					if (pInven_UI == nullptr)
						return;
					pInven_UI->Set_Field_Item(szSrc);
					pInven_UI->Set_ItemInfo(&tItemInfo);
				}
			}
			if (strcmp(szSrc, "Weapon_Shield") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				ITEM tItemInfo = dynamic_cast<CWeapon_Shield*>(szSrc)->Get_ItemInfo();
				tItemInfo.iNum;

				if (tItemInfo.eItemStatus == ITEM_FIELD)
				{
					//UI는 필드일 때만 깜빡인다.
					//UI
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(true, 1);
					//

					CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
					_bool bGrab = szDest->Get_GrabState();

					//일단 이 함수는 한번만 탄다.

					CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
					if (pInven_UI == nullptr)
						return;
					pInven_UI->Set_Field_Item(szSrc);
					pInven_UI->Set_ItemInfo(&tItemInfo);
				}
			}

		}///
		if (strcmp(szSrc, "Player_Grab") == 0 && strcmp(szDest, "Player") != 0)
		{

			NxVec3 NxDist = triggerShape.getActor().getGlobalPosition();
			NxReal pDist = NxDist.distance(otherShape.getActor().getGlobalPosition());

			CGameObject* pDest = (CGameObject*)otherShape.getActor().userData;
			if (strcmp(szDest, "Item_Rupee_Green") == 0)
			{
				// 돈 증가.
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				pInven_UI->Set_iMoney(-1);
				pDest->Set_GameObject(true);
			}

			if (pDest == nullptr)
				return;

			m_DistMap.insert({ pDist, pDest });
		}

		if (strcmp(szSrc, "Player_Grab") == 0 && strcmp(szDest, "StoneBox") == 0)	//Enter
		{
			CGameObject* pDestObj = (CGameObject*)otherShape.getActor().userData;
			CStone_Treasure_Box* pStoneBox = dynamic_cast<CStone_Treasure_Box*>(pDestObj);

			CGameObject* pGameObj = (CGameObject*)triggerShape.getActor().userData;
			CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pGameObj);


			_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
			CTreasureBox_UI* pTreasureBox_UI = (CTreasureBox_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_TreasureBox_UI", 0);
			CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
			if (pTreasureBox_UI == nullptr || pInven_UI == nullptr)
				return;

			//UI
			CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
			if (pKey_UI == nullptr)
				return;
			pKey_UI->Set_IsDraw(true, 3);
			//
		}

		if (strcmp(szSrc, "Player_Grab") == 0)
		{

			if (strcmp(szDest, "FireBall") == 0)
			{
				CGameObject* pObj = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
				CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pObj);

				if (FALSE == pPlayer->Get_bBlocking())
				{
					CGameObject* pDest = (CGameObject*)otherShape.getActor().userData;
					dynamic_cast<CFireBall*>(pDest)->Set_Dead(100.f);
					pPlayer->Set_HitPushDir(dynamic_cast<CFireBall*>(pDest)->Get_PushDir());

					pPlayer->Set_HitAction();
					pPlayer->Set_PlayerHp(2);
					CHeart* pHeart = (CHeart*)GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Heart", 0);
					if (pHeart != nullptr)
						pHeart->Minus_HpHeart(2);


				}
			}

			if (strcmp(szDest, "Meteor") == 0)
			{
				CGameObject* pObj = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
				CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pObj);

				CGameObject* pDest = (CGameObject*)otherShape.getActor().userData;
				dynamic_cast<CMeteor*>(pDest)->Set_Dead(100.f);

				_vec3 vLook = *pPlayer->Get_NxTransform()->Get_Info(CNxTransform::INFO_LOOK);

				pPlayer->Set_HitPushDir(-vLook);
				pPlayer->Set_HitAction();
				pPlayer->Set_PlayerHp(8);
				CHeart* pHeart = (CHeart*)GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Heart", 0);
				if (pHeart != nullptr)
					pHeart->Minus_HpHeart(8);
			}

		}///

		// 몬스터 && 플레이어 HIT 충돌 체크 (방패 && 피격 등)
		if (strcmp(szSrc, "Weapon_Melee") == 0) // <-- 몬스터가 들고 있는 무기
		{
			CWeapon_Melee* pWeapon = dynamic_cast<CWeapon_Melee*>((CGameObject*)triggerShape.getActor().userData);
			CWeapon_Shield* pShield = dynamic_cast<CWeapon_Shield*>((CGameObject*)otherShape.getActor().userData);

			CGameObject* pObj = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
			CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pObj);

			_bool	bHitOnce = false;

			if (strcmp(szDest, "Weapon_Shield") == 0)
			{
				if (ITEM_MONSTER == pWeapon->Get_ItemInfo().eItemStatus && ITEM_PLAYER == pShield->Get_ItemInfo().eItemStatus
					&& TRUE == pPlayer->Get_bBlocking() && TRUE == pWeapon->Get_bIsAttacking())
				{
					pPlayer->Set_ShieldHitAction();
					pPlayer->Set_HitPushDir(pWeapon->Get_PushDir());
					bHitOnce = true;
				}
			}
			else if (strcmp(szDest, "Player") == 0)
			{
				if (ITEM_MONSTER == pWeapon->Get_ItemInfo().eItemStatus && TRUE == pWeapon->Get_bIsAttacking() && FALSE == bHitOnce)
				{
					pPlayer->Set_HitAction();
					pPlayer->Set_HitPushDir(pWeapon->Get_PushDir());
					pPlayer->Set_PlayerHp(1);
					bHitOnce = true;
					CHeart* pHeart = (CHeart*)GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Heart", 0);
					if (pHeart != nullptr)
						pHeart->Minus_HpHeart(1);
				}
			}
		}

		if (strcmp(szSrc, "LostKokko") == 0 && strcmp(szDest, "Chicken") == 0)
		{
			CFldObj_MiniGame_LostKokko* pLostKokko = (CFldObj_MiniGame_LostKokko*)triggerShape.getActor().userData;
			CChiken* pKokko = (CChiken*)otherShape.getActor().userData;

			// 들어오지않았을경우만 타게할것이다.
			if (FALSE == pKokko->Get_Enter())
			{
				pKokko->Set_Enter();
				pLostKokko->Set_EnterKokko();
				_int iNum = pKokko->Get_Number();

				if (iNum >= 0)
					GET_INSTANCE(CManagement)->Set_KokkoEnter(iNum);
			}
		}

		if (strcmp(szSrc, "Trigger_Water") == 0)
		{
			if ((strcmp(szDest, "Pot") == 0 || strcmp(szDest, "Bomb") == 0))
			{
				CGameObject* pPot = (CGameObject*)otherShape.getActor().userData;
				pPot->Set_EnterWater(TRUE);
				pPot->Set_WaterY(triggerShape.getActor().getGlobalPosition().y);
			}
			else if (strcmp(szDest, "Chicken") == 0)
			{
				CChiken* pChicken = (CChiken*)otherShape.getActor().userData;
				pChicken->Set_vReturnPos();
			}

		}///

		if (nullptr != pDestShape)	//Bullet 포함 아님
		{

			if (strcmp(szSrc, "Trigger_BowlingZone") == 0 && strcmp(pDestShape, "BowlingPin_Head") == 0)
			{
				CTrigger_BowlingZone* pBowlingZone = (CTrigger_BowlingZone*)triggerShape.getActor().userData;
				CBowlPin* pBowlPin = (CBowlPin*)otherShape.getActor().userData;

				if (FALSE == pBowlPin->Get_EnterTrigger())
				{
					pBowlPin->Set_EnterTrigger();
					// 닿으면 카운트를 하나 증가시킬것이다.
					pBowlingZone->Set_AddCnt();
				}
			}

			if (strcmp(szSrc, "MagnetBullet") == 0 && GET_INSTANCE(CPhysXMgr)->Get_Magnet() == FALSE)
			{
				if (strcmp(szDest, "IronDoorR") == 0)
				{
					CMagnetBullet* pMagnetBullet = dynamic_cast<CMagnetBullet*>
						((CGameObject*)triggerShape.getActor().userData);

					pMagnetBullet->m_bMagnet = TRUE;

					CDgnObj_IronDoorR* pDoorR = dynamic_cast<CDgnObj_IronDoorR*>
						((CGameObject*)otherShape.getActor().userData);

					//pDoorR->Set_TargetActor(&triggerShape.getActor());
					pDoorR->m_pTargetActor = &triggerShape.getActor();
					GET_INSTANCE(CPhysXMgr)->Set_Magnet(TRUE);

				}
				else if (strcmp(szDest, "IronDoorL") == 0)
				{
					CMagnetBullet* pMagnetBullet = dynamic_cast<CMagnetBullet*>
						((CGameObject*)triggerShape.getActor().userData);

					pMagnetBullet->m_bMagnet = TRUE;

					CDgnObj_IronDoorL* pDoorR = dynamic_cast<CDgnObj_IronDoorL*>
						((CGameObject*)otherShape.getActor().userData);

					pDoorR->m_pTargetActor = &triggerShape.getActor();
					GET_INSTANCE(CPhysXMgr)->Set_Magnet(TRUE);
				}
			}

			if (strcmp(szSrc, "MagnetBullet") == 0 && strcmp(pDestShape, "Magnet") == 0)
			{
				CMagnetBullet* pMagnetBullet = dynamic_cast<CMagnetBullet*>
					((CGameObject*)triggerShape.getActor().userData);

				if (GET_INSTANCE(CPhysXMgr)->Get_Magnet() == FALSE && pMagnetBullet->m_bMagnet == FALSE)
				{
					pMagnetBullet->m_bMagnet = TRUE;
					pMagnetBullet->m_pTargetActor = &otherShape.getActor();
					GET_INSTANCE(CPhysXMgr)->Set_Magnet(TRUE);

					// 중력 죽이기.
					otherShape.getActor().raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
				}


			}

			// Ladder
			if (strcmp(szSrc, "Ladder") == 0 && strcmp(pDestShape, "Player_Sub") == 0)
			{
				GET_INSTANCE(CPhysXMgr)->Set_Ladder(TRUE);
				CLadder* pLadder = dynamic_cast<CLadder*>((CGameObject*)triggerShape.getActor().userData);

				// Ladder에 미리 넣어뒀던 위치값과 각도를 가져온다.
				vector<_vec4> vecLadder = pLadder->m_vecLadder;
				map<_float, _vec4> mapDist;

				NxVec3 vPlayerPos = otherShape.getActor().getGlobalPosition();
				NxVec3 vLadderPos = { 0.f,0.f,0.f };

				for (auto& vLadder : vecLadder)
				{
					memcpy(&vLadderPos, &vLadder, sizeof(NxVec3));

					_float fDist = vPlayerPos.distance(vLadderPos);

					mapDist.insert({ fDist, vLadder });
				}

				m_vLadder = mapDist.begin()->second;
			}
			// IceCube Ladder
			else if (strcmp(szSrc, "IceMaker") == 0 && strcmp(pDestShape, "Player_Sub") == 0)
			{
				GET_INSTANCE(CPhysXMgr)->Set_Ladder(TRUE);
				CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);

				NxVec3 vPos = pPlayer->Get_NxTransform()->Get_Actor()->getGlobalPosition();
				_float fAngle = pPlayer->Get_NxTransform()->m_vAngle.y;

				m_vLadder = _vec4(vPos.x, vPos.y, vPos.z, D3DXToDegree(fAngle));
			}


		}///

		if (nullptr != pSrcShape && nullptr != pDestShape)
		{
			if (strcmp(pDestShape, "Enemy_Giant_RHand") == 0)
			{
				CEnemy_Giant* pEnemy = (CEnemy_Giant*)otherShape.getActor().userData;

				if (strcmp(pSrcShape, "Obj_Trigger_TreeApple") == 0 && FALSE == pEnemy->Get_bLog())
				{
					triggerShape.getActor().setName("Dead");
					CObj_TreeApple* pTreeApple = (CObj_TreeApple*)triggerShape.getActor().userData;
					pTreeApple->Set_Grab((CGameObject*)otherShape.getActor().userData);
				}
				else if (strcmp(pSrcShape, "Obj_Log_TreeApple") == 0)
				{
					CObj_Log_TreeApple* pLog = (CObj_Log_TreeApple*)triggerShape.getActor().userData;
					_bool bIsCatch = pEnemy->Get_CatchTree();

					// 잡고있지않는 상황에서만 다시 잡아야한다.
					if (nullptr != pLog && !bIsCatch)
					{
						pEnemy->Set_Catch();
						pLog->Set_AgainGrabObject(pEnemy);
					}
				}
				else if (strcmp(pSrcShape, "Obj_Trigger_TreeMaple") == 0 && FALSE == pEnemy->Get_bLog())
				{
					triggerShape.getActor().setName("Dead");
					CObj_TreeMaple* pTreeMaple = (CObj_TreeMaple*)triggerShape.getActor().userData;
					pTreeMaple->Set_Grab((CGameObject*)otherShape.getActor().userData);
				}
				else if (strcmp(pSrcShape, "Obj_Log_TreeMaple") == 0)
				{
					CObj_Log_TreeMaple* pLog = (CObj_Log_TreeMaple*)triggerShape.getActor().userData;
					_bool bIsCatch = pEnemy->Get_CatchTree();

					// 잡고있지않는 상황에서만 다시 잡아야한다.
					if (nullptr != pLog && !bIsCatch)
					{
						pEnemy->Set_Catch();
						pLog->Set_AgainGrabObject(pEnemy);
					}
				}
				//	else if(strcmp(pSrcShape, "Obj_Trigger_Tree") == 0)
			}
			//// Boss
			//if (strcmp(pSrcShape, "Obj_Trigger_Tree") == 0)
			//{
			//	// 오른손이 TreeApple과 부딪혔을때.Enemy_Giant
			//	if (strcmp(pDestShape, "Enemy_Giant_RHand") == 0)
			//	{
			//		triggerShape.getActor().setName("Dead");
			//		CObj_TreeApple* pTreeApple = (CObj_TreeApple*)triggerShape.getActor().userData;
			//		pTreeApple->Set_Grab((CGameObject*)otherShape.getActor().userData);
			//	}
			//}
		}

		if (strcmp(szSrc, "Weapon_Shield") == 0)  // 플레이어가 들고있는 방패와 몬스터들의 공격 충돌 
		{
			if (strcmp(szDest, "FireBall") == 0)
			{
				CGameObject* pObj = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
				CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pObj);

				CWeapon_Shield* pShield = dynamic_cast<CWeapon_Shield*>((CGameObject*)triggerShape.getActor().userData);

				if (ITEM_PLAYER == pShield->Get_ItemInfo().eItemStatus && TRUE == pPlayer->Get_bShieldReady() &&
					FALSE == pPlayer->Get_bShieldHit() && TRUE == pPlayer->Get_bBlocking())
				{
					CGameObject* pDest = (CGameObject*)otherShape.getActor().userData;
					dynamic_cast<CFireBall*>(pDest)->Set_Dead(100.f);
					pPlayer->Set_ShieldHitAction();
				}
			}
		}



		//폭발 시 모든 객체들을 넉백시킴
		if (strcmp(szSrc, "KaBomb") == 0)
		{
			NxVec3 nxBomb = triggerShape.getGlobalPosition();
			nxBomb.y -= 1.f;
			NxVec3 nxPos = otherShape.getGlobalPosition() - nxBomb;
			nxPos.normalize();
			nxPos.setMagnitude(NxReal(10));

			if (strcmp(szDest, "Pot") == 0)
			{
				CGameObject* pGameObject = (CGameObject*)otherShape.getActor().userData;
				pGameObject->Set_GameObject(TRUE);
			}
			else if (strcmp(szDest, "Obj_TreeApple") == 0)
			{
				otherShape.getActor().setName("Dead");
				CObj_TreeApple* pTreeApple = (CObj_TreeApple*)otherShape.getActor().userData;
				pTreeApple->Set_Velocity(nxPos);
			}
			else if (strcmp(szDest, "Obj_TreeMaple") == 0)
			{
				otherShape.getActor().setName("Dead");
				CObj_TreeMaple* pTreeMaple = (CObj_TreeMaple*)otherShape.getActor().userData;
				pTreeMaple->Set_Velocity(nxPos);
			}
			else
				otherShape.getActor().setLinearVelocity(nxPos);

			if (strcmp(szDest, "Bomb") == 0)
			{
				CTestBomb* pDest = dynamic_cast<CTestBomb*>((CGameObject*)otherShape.getActor().userData);
				pDest->Set_KABOOM(TRUE);
			}

		}

		//무기와 몬스터 충돌 (한손검)
		{
			if (strcmp(szSrc, "Weapon_MeleeOneHand") == 0)
			{
				CWeapon_MeleeOneHand* pWep = dynamic_cast<CWeapon_MeleeOneHand*>((CGameObject*)triggerShape.getActor().userData);

				if (strcmp(szDest, "Pot") == 0)
				{
					CGameObject* pGameObject = (CGameObject*)otherShape.getActor().userData;
					pGameObject->Set_GameObject(TRUE);
				}

				if (strcmp(szDest, "Bokoblin") == 0)
				{
					CBokoblin* pMob = dynamic_cast<CBokoblin*>((CGameObject*)otherShape.getActor().userData);

					if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER)
					{
						CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
						_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bOneHandAttacking();

						if (true == bAttacking)
						{
							NxVec3 NxDir;
							NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
							NxDir.y = 0.f;
							NxDir.normalize();

							pMob->Set_HitVeolocity(NxDir);

							if (false == pMob->Get_bIsDead())
							{
								pMob->Set_MobHp(20);
								pMob->Set_HitAnimation();

								if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
									GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

							}
						}
					}
				}

				if (strcmp(szDest, "WizzRobe") == 0)
				{
					CWizzRobe* pMob = dynamic_cast<CWizzRobe*>((CGameObject*)otherShape.getActor().userData);

					if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER)
					{
						CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
						_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bOneHandAttacking();

						if (true == bAttacking)
						{
							NxVec3 NxDir;
							NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
							NxDir.y = 0.f;
							NxDir.normalize();

							pMob->Set_HitVeolocity(NxDir);

							if (false == pMob->Get_bIsDead())
							{
								pMob->Set_MobHp(20);
								pMob->Set_HitAnimation();

								if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
									GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

							}
						}
					}
				}

				if (strcmp(szDest, "Enemy_Giant") == 0)
				{
					CEnemy_Giant* pMob = dynamic_cast<CEnemy_Giant*>((CGameObject*)otherShape.getActor().userData);

					if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER)
					{
						CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
						_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bOneHandAttacking();

						if (true == bAttacking)
						{
							NxVec3 NxDir;
							NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
							NxDir.y = 0.f;
							NxDir.normalize();

							pMob->Set_HitVeolocity(NxDir);

							if (false == pMob->Get_Dead())
							{
								pMob->Set_Damage(20);
								//pMob->Set_HitAnimation();

								if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
									GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

							}
						}
					}
				}

				if (strcmp(szDest, "Chicken") == 0)
				{
					CChiken* pChic = (CChiken*)otherShape.getActor().userData;
					pChic->Set_ChickenHit(TRUE);
				}

				if (pDestShape != nullptr)
				{
					if (strcmp(pDestShape, "TimeStop") == 0)
					{
						CGameObject* pTime = (CGameObject*)otherShape.getActor().userData;
						CWeapon_MeleeOneHand* pMelee = dynamic_cast<CWeapon_MeleeOneHand*>((CGameObject*)triggerShape.getActor().userData);

						if (pTime == nullptr || pMelee == nullptr)
							return;

						otherShape.getActor().wakeUp(5);

						if (pTime->m_bTimeStop)
						{
							_matrix matView;
							_vec3 vPos = pMelee->m_vPlayerLook;
							D3DXVec3Normalize(&vPos, &vPos);

							GET_INSTANCE(CPhysXMgr)->m_pDevice->GetTransform(D3DTS_VIEW, &matView);
							D3DXMatrixInverse(&matView, nullptr, &matView);
							_float  fY = matView._32;
							if (fY > 0.2f)
								fY = 0.2f;
							else if (fY < -0.2)
								fY = -0.2f;

							vPos.y += fY * 8.f;

							pTime->m_NxVelocity += (NxVec3)vPos * 10.f;
							GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_Hit_06.wav", CSoundMgr::FIELD_OBJECT0);
						}
					}
				}

				if (strcmp(szDest, "Obj_TreeApple") == 0)
				{
					// 트리거와 나무의 포스를 비교하고 해당 방향을 줄것이다.
					NxVec3 vTriggerPos = triggerShape.getActor().getGlobalPosition();
					NxVec3 vTreePos = otherShape.getActor().getGlobalPosition();

					otherShape.getActor().setName("Dead");
					CObj_TreeApple* pTreeApple = (CObj_TreeApple*)otherShape.getActor().userData;
					pTreeApple->Set_Velocity(NxVec3(vTreePos - vTriggerPos));
				}
				else if (strcmp(szDest, "Obj_TreeMaple") == 0)
				{
					// 트리거와 나무의 포스를 비교하고 해당 방향을 줄것이다.
					NxVec3 vTriggerPos = triggerShape.getActor().getGlobalPosition();
					NxVec3 vTreePos = otherShape.getActor().getGlobalPosition();

					otherShape.getActor().setName("Dead");
					CObj_TreeMaple* pTreeMaple = (CObj_TreeMaple*)otherShape.getActor().userData;
					pTreeMaple->Set_Velocity(NxVec3(vTreePos - vTriggerPos));
				}
			}
		}

		//무기와 몬스터 충돌 [양손검]
		if (strcmp(szSrc, "Weapon_Melee") == 0)
		{
			CWeapon_Melee* pWep = dynamic_cast<CWeapon_Melee*>((CGameObject*)triggerShape.getActor().userData);

			if (strcmp(szDest, "Bokoblin") == 0)
			{
				CBokoblin* pMob = dynamic_cast<CBokoblin*>((CGameObject*)otherShape.getActor().userData);

				if (pWep->Get_ItemInfo().eItemStatus == ITEM_FIELD && false == pMob->Get_bIsDead() && 0 == pMob->Get_bEquipState())
				{
					pMob->Set_ItemInfo(pWep->Get_ItemInfo());
					pMob->Set_PickedUpWeapon(pWep);
				}
				else if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER) // 보코블린이 플레이어가 들고 있는 무기에 처 맞는 것
				{
					CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
					_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bIsActionState();

					if (true == bAttacking)
					{
						NxVec3 NxDir;
						NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
						NxDir.y = 0.f;
						NxDir.normalize();

						pMob->Set_HitVeolocity(NxDir);

						if (false == pMob->Get_bIsDead())
						{
							pMob->Set_MobHp(20);
							pMob->Set_HitAnimation();

							if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
								GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

						}
					}
				}
				else if (pWep->Get_ItemInfo().eItemStatus == ITEM_MONSTER)
				{
					if (strcmp(szDest, "Player") == 0)
					{
					}
				}
			}

			if (strcmp(szDest, "WizzRobe") == 0)
			{
				CWizzRobe* pMob = dynamic_cast<CWizzRobe*>((CGameObject*)otherShape.getActor().userData);

				if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER)
				{
					CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
					_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bIsActionState();

					if (true == bAttacking)
					{
						NxVec3 NxDir;
						NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
						NxDir.y = 0.f;
						NxDir.normalize();

						pMob->Set_HitVeolocity(NxDir);

						if (false == pMob->Get_bIsDead())
						{
							pMob->Set_MobHp(20);
							pMob->Set_HitAnimation();

							if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
								GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

						}
					}
				}

			}

			if (strcmp(szDest, "Enemy_Giant") == 0)
			{
				CEnemy_Giant* pMob = dynamic_cast<CEnemy_Giant*>((CGameObject*)otherShape.getActor().userData);

				if (pWep->Get_ItemInfo().eItemStatus == ITEM_PLAYER)
				{
					CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
					_bool bAttacking = dynamic_cast<CNxPlayer*>(pPlayer)->Get_bIsActionState();

					if (true == bAttacking)
					{
						NxVec3 NxDir;
						NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
						NxDir.y = 0.f;
						NxDir.normalize();

						pMob->Set_HitVeolocity(NxDir);

						if (false == pMob->Get_Dead())
						{
							pMob->Set_Damage(20);
							//pMob->Set_HitAnimation();

							if (FALSE == GET_INSTANCE(CTimeMgr)->Check_SlowTime())
								GET_INSTANCE(CTimeMgr)->Set_SlowTime(TRUE);

						}
					}
				}

			}
			//무기 스테이트가 공격 상태일때만 공격이 들어가도록 수정
			//pMob->Set_MobHp(10);

			if (strcmp(szDest, "Chicken") == 0)
			{
				CChiken* pChic = (CChiken*)otherShape.getActor().userData;
				pChic->Set_ChickenHit(TRUE);
			}

			if (strcmp(szDest, "Pot") == 0)
			{
				CGameObject* pGameObject = (CGameObject*)otherShape.getActor().userData;
				pGameObject->Set_GameObject(TRUE);
			}

			if (strcmp(szDest, "Obj_TreeApple") == 0)
			{
				// 트리거와 나무의 포스를 비교하고 해당 방향을 줄것이다.
				NxVec3 vTriggerPos = triggerShape.getActor().getGlobalPosition();
				NxVec3 vTreePos = otherShape.getActor().getGlobalPosition();

				otherShape.getActor().setName("Dead");
				CObj_TreeApple* pTreeApple = (CObj_TreeApple*)otherShape.getActor().userData;
				pTreeApple->Set_Velocity(NxVec3(vTreePos - vTriggerPos));
			}
			else if (strcmp(szDest, "Obj_TreeMaple") == 0)
			{
				// 트리거와 나무의 포스를 비교하고 해당 방향을 줄것이다.
				NxVec3 vTriggerPos = triggerShape.getActor().getGlobalPosition();
				NxVec3 vTreePos = otherShape.getActor().getGlobalPosition();

				otherShape.getActor().setName("Dead");
				CObj_TreeMaple* pTreeMaple = (CObj_TreeMaple*)otherShape.getActor().userData;
				pTreeMaple->Set_Velocity(NxVec3(vTreePos - vTriggerPos));
			}

			if (pDestShape != nullptr)
			{
				if (strcmp(pDestShape, "TimeStop") == 0)
				{
					CGameObject* pTime = (CGameObject*)otherShape.getActor().userData;
					CWeapon_Melee* pMelee = dynamic_cast<CWeapon_Melee*>((CGameObject*)triggerShape.getActor().userData);
					CGameObject* pObj = (CGameObject*)otherShape.getActor().userData;
					pObj->Set_Magnet(TRUE);

					if (pTime == nullptr || pMelee == nullptr)
						return;

					otherShape.getActor().wakeUp(5);

					if (pTime->m_bTimeStop)
					{
						_matrix matView;
						_vec3 vPos = pMelee->m_vPlayerLook;
						D3DXVec3Normalize(&vPos, &vPos);

						GET_INSTANCE(CPhysXMgr)->m_pDevice->GetTransform(D3DTS_VIEW, &matView);
						D3DXMatrixInverse(&matView, nullptr, &matView);
						_float  fY = matView._32;
						if (fY > 0.2f)
							fY = 0.2f;
						else if (fY < -0.2)
							fY = -0.2f;

						vPos.y += fY * 8.f;

						pTime->m_NxVelocity += (NxVec3)vPos * 10.f;
						GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_Hit_06.wav", CSoundMgr::FIELD_OBJECT0);
					}
				}
			}
		}

		if (pSrcShape != nullptr)
		{
			if ((strcmp(pSrcShape, "ClimbCol") == 0))
			{
				if (strcmp(szDest, "Terrain") == 0)
				{
					CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
					dynamic_cast<CNxPlayer*>(pPlayer)->Set_bEnterClimb(true);
				}
			}
		}

		if (strcmp(szSrc, "Bullet_Arrow") == 0 && strcmp(szDest, "Player") != 0)
		{
			CGameObject* pGameObject = (CGameObject*)triggerShape.getActor().userData;

			if (pDestShape == nullptr)
				pGameObject->Set_GameObject(true);
			else if (strcmp(pDestShape, "Trigger") != 0)
				pGameObject->Set_GameObject(true);

			if (strcmp(szDest, "Chicken") == 0)
			{
				CChiken* pChic = (CChiken*)otherShape.getActor().userData;
				pChic->Set_ChickenHit(TRUE);
			}

			if (strcmp(szDest, "Pot") == 0)
			{
				CGameObject* pGameObject = (CGameObject*)otherShape.getActor().userData;
				pGameObject->Set_GameObject(TRUE);
			}

			if (strcmp(szDest, "WizzRobe") == 0)
			{
				NxVec3 NxDir;
				NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
				NxDir.y = 0.f;
				NxDir.normalize();

				CWizzRobe* pWizzRobe = (CWizzRobe*)otherShape.getActor().userData;
				pWizzRobe->Set_HitVeolocity(NxDir);

				if (false == pWizzRobe->Get_bIsDead())
				{
					pWizzRobe->Set_MobHp(50);
					pWizzRobe->Set_HitAnimation();
				}
			}

			if (strcmp(szDest, "Bokoblin") == 0)
			{
				NxVec3 NxDir;
				NxDir = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
				NxDir.y = 0.f;
				NxDir.normalize();

				CBokoblin* pBoko = (CBokoblin*)otherShape.getActor().userData;
				pBoko->Set_HitVeolocity(NxDir);

				if (false == pBoko->Get_bIsDead())
				{
					pBoko->Set_MobHp(50);
					pBoko->Set_HitAnimation();
				}
			}

			if (strcmp(szDest, "Obj_YabusameTarget_B") == 0 || strcmp(szDest, "Obj_YabusameTarget_A") == 0)
				otherShape.getActor().setName("Dead");

			if (strcmp(szDest, "Obj_TreeApple") == 0)
			{
				otherShape.getActor().setName("Dead");
				CObj_TreeApple* pTreeApple = (CObj_TreeApple*)otherShape.getActor().userData;
				pTreeApple->Set_Velocity(triggerShape.getActor().getLinearVelocity());
			}

			if (strcmp(szDest, "Obj_TreeMaple") == 0)
			{
				otherShape.getActor().setName("Dead");
				CObj_TreeMaple* pTreeMaple = (CObj_TreeMaple*)otherShape.getActor().userData;
				pTreeMaple->Set_Velocity(triggerShape.getActor().getLinearVelocity());
			}

			if (strcmp(szDest, "Enemy_Giant") == 0)
			{
				CEnemy_Giant* pGiant = (CEnemy_Giant*)otherShape.getActor().userData;
				pGiant->Set_Damage(5);		// 데미지 10

											// 깨우기.
											// 깨울때와 깨우고 나서의 처리.
				if (!pGiant->Get_WakeUp())
					pGiant->Set_WakeUp();
				else
				{
					if (nullptr != pDestShape && strcmp(pDestShape, "Enemy_Giant_EyeBall") == 0)
					{
						pGiant->Set_Damage(10);
						pGiant->Set_EyeHit();
					}
				}

				if (0 >= pGiant->Get_Hp())
					pGiant->Set_Dead();
			}

			if (nullptr != pDestShape)
			{
				if (strcmp(pDestShape, "TimeStop") == 0)
				{
					CGameObject* pTime = (CGameObject*)otherShape.getActor().userData;
					CBullet_ArrowNormal* pBullet = dynamic_cast<CBullet_ArrowNormal*>((CGameObject*)triggerShape.getActor().userData);

					if (pTime == nullptr || pBullet == nullptr)
						return;

					otherShape.getActor().wakeUp(5);

					if (pTime->m_bTimeStop)
					{
						_matrix matView;
						NxVec3 nxPos = otherShape.getActor().getGlobalPosition() - triggerShape.getActor().getGlobalPosition();
						nxPos.normalize();

						GET_INSTANCE(CPhysXMgr)->m_pDevice->GetTransform(D3DTS_VIEW, &matView);
						D3DXMatrixInverse(&matView, nullptr, &matView);
						_float  fY = matView._32;
						if (fY > 0.2f)
							fY = 0.2f;
						else if (fY < -0.2)
							fY = -0.2f;

						nxPos.y += fY * 8.f;

						pTime->m_NxVelocity += nxPos * 10.f;
						GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_Hit_06.wav", CSoundMgr::FIELD_OBJECT0);
					}
				}
			}

		}///

		 // Npc
		if (strcmp(szSrc, "Npc") == 0 && strcmp(szDest, "Player") == 0)
		{
			CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
			if (pKey_UI == nullptr)
				return;
			pKey_UI->Set_IsDraw(true, 2);

			CNxPlayer* pPlayer = (CNxPlayer*)otherShape.getActor().userData;
			pPlayer->Set_Npc((CNpc*)triggerShape.getActor().userData);
		}

	}
	if (status & NX_TRIGGER_ON_LEAVE)
	{
		//Ladder
		const char* pSrc = triggerShape.getActor().getName();
		const char* pDest = otherShape.getActor().getName();

		const char* pSrcShape = triggerShape.getName();
		const char* pDestShape = otherShape.getName();

		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

		if (strcmp(pSrc, "Player_Grab") == 0)	//Leave
		{
			CGameObject* pSrc = (CGameObject*)triggerShape.getActor().userData;
			pSrc->Set_GrabState(FALSE);

			CGameObject* pDest = (CGameObject*)otherShape.getActor().userData;
			pSrc->Set_GrabState(FALSE);

			m_DistMap.clear();
		}


		if (strcmp(pSrc, "Trigger_Camera") == 0)
		{
			if (strcmp(pDest, "StoneBall_Big") == 0 && GET_INSTANCE(CPhysXMgr)->m_eScene == CPhysXMgr::STAGE_FIELD)
			{
				CGameObject* pTrigger = (CGameObject*)triggerShape.getActor().userData;

				if (pTrigger->Get_GrabState())
				{
					pTrigger->Set_EnterWater(TRUE);	//카메라 트리거 작동

					CGameObject* pBall = (CGameObject*)otherShape.getActor().userData;
					pBall->Set_EnterWater(TRUE);	//카메라 트리거 작동
				}
			}
		}

		if (strcmp(pSrc, "Trigger_Sound") == 0)
		{
			if (strcmp(pDest, "BlockCrack_Brk") == 0 && GET_INSTANCE(CPhysXMgr)->m_eScene == CPhysXMgr::STAGE_01)
			{
				CGameObject* pPot = (CGameObject*)triggerShape.getActor().userData;
				pPot->Set_EnterWater(TRUE);	//사운드 트리거 작동
			}

			if (strcmp(pDest, "DgnObj_FenceDynamic") == 0 && GET_INSTANCE(CPhysXMgr)->m_eScene == CPhysXMgr::STAGE_02)
			{
				CGameObject* pPot = (CGameObject*)triggerShape.getActor().userData;
				pPot->Set_EnterWater(TRUE);	//사운드 트리거 작동
			}

			if (strcmp(pDest, "StoneBall_Big") == 0 && GET_INSTANCE(CPhysXMgr)->m_eScene == CPhysXMgr::STAGE_03)
			{
				CGameObject* pPot = (CGameObject*)triggerShape.getActor().userData;
				pPot->Set_EnterWater(TRUE);	//사운드 트리거 작동
			}
			if (pDestShape != nullptr && GET_INSTANCE(CPhysXMgr)->m_eScene == CPhysXMgr::STAGE_04)
			{
				if (strcmp(pDestShape, "Magnet") == 0)
				{
					CGameObject* pPot = (CGameObject*)triggerShape.getActor().userData;
					pPot->Set_EnterWater(TRUE);	//사운드 트리거 작동
				}
			}
		}

		if (strcmp(pDest, "Player") == 0)
		{
			const char* pSrcShape = triggerShape.getName();

			if (pSrcShape != nullptr && strcmp(pSrcShape, "Dgn_End") == 0)	//던전 탈출 팝업 처리
			{
				_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);
				CDungeonOut_UI* pOut_UI = (CDungeonOut_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Dungeon_UI", 0);
				if (pOut_UI == nullptr)
					return;
				pOut_UI->Set_IsDraw(false);
				pOut_UI->Set_Popup(false);
			}

			if (strcmp(pSrc, "Trigger_Door") == 0 || strcmp(pSrc, "Trigger_HotelDoor") == 0)
			{
				const char* pSrcShape = triggerShape.getName();

				if (strcmp(pSrcShape, "Door_In") == 0)
				{
					_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(false, 2);

				}

				if (strcmp(pSrcShape, "Door_Out") == 0)
				{
					_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
					CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
					if (pKey_UI == nullptr)
						return;
					pKey_UI->Set_IsDraw(false, 2);


				}
			}

			// Touch Terminal Trigger
			if (strcmp(pSrc, "Create_SeekerSkill") == 0)
			{
				m_bIsTouch_Create_SeekerSkill = false;

				//
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);
				//
			}
			// TwnObj_Touch Terminal Trigger
			if (strcmp(pSrc, "Obj_TouchTerminal") == 0)
			{
				m_vTerminalPos = NxVec3(0.f, 0.f, 0.f);
				m_pDgnShutter = nullptr;

				//
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);
				//
			}
			// FldObj_EntranceElevator
			if (strcmp(pSrc, "FldObj_EntranceElevator") == 0)
			{
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				m_pEntranceElevator = nullptr;
				m_vTerminalPos.zero();
			}

			//무기와 플레이어 충돌
			if (strcmp(pSrc, "Weapon_Melee") == 0)
			{

				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 1);

				ITEM item;
				ZeroMemory(&item, sizeof(ITEM));
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pInven_UI == nullptr)
					return;
				pInven_UI->Set_Field_Item(nullptr);
				pInven_UI->Set_ItemInfo(&item);
				//

			}

			if (strcmp(pSrc, "Weapon_MeleeOneHand") == 0)
			{

				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 1);

				ITEM item;
				ZeroMemory(&item, sizeof(ITEM));
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pInven_UI == nullptr)
					return;
				pInven_UI->Set_Field_Item(nullptr);
				pInven_UI->Set_ItemInfo(&item);
				//

			}

			//Weapon_Ranged
			if (strcmp(pSrc, "Weapon_Ranged") == 0)
			{

				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 1);

				ITEM item;
				ZeroMemory(&item, sizeof(ITEM));
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pInven_UI == nullptr)
					return;
				pInven_UI->Set_Field_Item(nullptr);
				pInven_UI->Set_ItemInfo(&item);
				//

			}

			//무기와 플레이어 충돌
			if (strcmp(pSrc, "Weapon_Shield") == 0)
			{

				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				//UI
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 1);
				//

				ITEM item;
				ZeroMemory(&item, sizeof(ITEM));
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pInven_UI == nullptr)
					return;
				pInven_UI->Set_Field_Item(nullptr);
				pInven_UI->Set_ItemInfo(&item);
				//
			}

			// Npc_Bolson
			if (strcmp(pSrc, "Npc") == 0)
			{
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 2);

				CNxPlayer* pPlayer = (CNxPlayer*)otherShape.getActor().userData;
				pPlayer->Set_Npc(nullptr);

				CQuest_Ballon_UI* pQuestBallon = (CQuest_Ballon_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Ballon_UI", 0);
				if (nullptr != pQuestBallon)
					pQuestBallon->Set_IsDraw(false, 0);

				CQuest_Koko_UI* pQuestKoko = (CQuest_Koko_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Koko_UI", 0);
				if (nullptr != pQuestKoko)
					pQuestKoko->Set_IsDraw(false, 0);

				CQuest_Bowling_UI* pQuestBowling = (CQuest_Bowling_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Bowling_UI", 0);
				if (nullptr != pQuestBowling)
					pQuestBowling->Set_IsDraw(false, 0);

				CMainQuest* pMainQuest = (CMainQuest*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_MainQuest", 0);
				if (nullptr != pMainQuest)
					pMainQuest->Set_IsDraw(false, 0);

			}

			if (strcmp(pSrc, "Shop_Head_Front") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(0);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}

			if (strcmp(pSrc, "Shop_Lower_Front") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(1);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}
			if (strcmp(pSrc, "Shop_Upper_Front") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(2);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}
			if (strcmp(pSrc, "Shop_Head_Behind") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(3);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}
			if (strcmp(pSrc, "Shop_Upper_Behind") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(4);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}
			if (strcmp(pSrc, "Shop_Lower_Behind") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				CShop_Tailor* pShop_Tailor = (CShop_Tailor*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Shop_Tailor", 0);
				if (pShop_Tailor == nullptr)
					return;

				pShop_Tailor->Set_TailorType(5);
				pShop_Tailor->Set_BoardRender(false);
				pShop_Tailor->Set_DialogRender(false);
			}

			//상점에서의 충돌
			if (strcmp(pSrc, "Shop_Milk") == 0)
			{
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();

				CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
				if (pShop_Expendable == nullptr)
					return;

				pShop_Expendable->Set_UI_RenderMilk(false);
				pShop_Expendable->Set_CurFoodState(5);


				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

			}
			else if (strcmp(pSrc, "Mashroom_Max") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
				if (pShop_Expendable == nullptr)
					return;

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;
				pKey_UI->Set_IsDraw(false, 0);

				pShop_Expendable->Set_UI_RenderMax(false);
				pShop_Expendable->Set_CurFoodState(5);
			}
			else if (strcmp(pSrc, "MashroomStemina") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
				if (pShop_Expendable == nullptr)
					return;

				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;

				pKey_UI->Set_IsDraw(false, 0);
				//
				CGameObject* szDest = (CGameObject*)otherShape.getActor().userData;
				_bool bGrab = szDest->Get_GrabState();

				pShop_Expendable->Set_UI_RenderStemina(false);
				pShop_Expendable->Set_CurFoodState(5);
			}
			else if (strcmp(pSrc, "Bird_Egg") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;
				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
				if (pShop_Expendable == nullptr)
					return;
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;

				pKey_UI->Set_IsDraw(false, 0);

				pShop_Expendable->Set_UI_RenderEgg(false);
				pShop_Expendable->Set_CurFoodState(5);
			}
			else if (strcmp(pSrc, "hylian_rice") == 0)
			{
				// 아이템을 이벤토리로 넣어준다
				CGameObject* szSrc = (CGameObject*)triggerShape.getActor().userData;

				_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
				CShop_Expendable* pShop_Expendable = (CShop_Expendable*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_ShopExpendable", 0);
				if (pShop_Expendable == nullptr)
					return;
				CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
				if (pKey_UI == nullptr)
					return;

				pKey_UI->Set_IsDraw(false, 0);

				pShop_Expendable->Set_UI_RenderRice(false);
				pShop_Expendable->Set_CurFoodState(5);
			}

		}///

		if (strcmp(pSrc, "Player_Grab") == 0 && strcmp(pDest, "StoneBox") == 0)
		{
			CGameObject* pDestObj = (CGameObject*)otherShape.getActor().userData;
			CStone_Treasure_Box* pStoneBox = dynamic_cast<CStone_Treasure_Box*>(pDestObj);

			CGameObject* pGameObj = (CGameObject*)triggerShape.getActor().userData;
			CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(pGameObj);

			_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
			CTreasureBox_UI* pTreasureBox_UI = (CTreasureBox_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_TreasureBox_UI", 0);
			CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
			if (pTreasureBox_UI == nullptr || pInven_UI == nullptr)
				return;

			pInven_UI->Set_ItemBox(nullptr);

			//UI
			CKey_UI* pKey_UI = (CKey_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Key_UI", 0);
			if (pKey_UI == nullptr)
				return;
			pKey_UI->Set_IsDraw(false, 3);
			//
		}

		if (strcmp(pSrc, "Camera_Eye") == 0 && strcmp(pDest, "Player") == 0)
		{
			CNxPlayer* pDest = dynamic_cast<CNxPlayer*>((CGameObject*)otherShape.getActor().userData);
			pDest->Set_RenderState(TRUE);
		}

		if (nullptr != pDestShape)
		{
			// Ladder
			if (strcmp(pSrc, "Ladder") == 0 && strcmp(pDestShape, "Player_Sub") == 0)
				GET_INSTANCE(CPhysXMgr)->Set_Ladder(FALSE);
			else if (strcmp(pSrc, "IceMaker") == 0 && strcmp(pDestShape, "Player_Sub") == 0)
				GET_INSTANCE(CPhysXMgr)->Set_Ladder(FALSE);
		}

		if (pSrcShape != nullptr)
		{
			if ((strcmp(pSrcShape, "ClimbCol") == 0))
			{
				if (strcmp(pDest, "Terrain") == 0)
				{
					CGameObject* pPlayer = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
					dynamic_cast<CNxPlayer*>(pPlayer)->Set_bEnterClimb(false);
				}
			}
		}

		if (strcmp(pSrc, "Trigger_WindZone") == 0 && strcmp(pDest, "Obj_YabusameTarget_A") == 0)
		{
			otherShape.getActor().setName("ZoneOut");
		}


	}
}

bool CPhysXMgr::RAYCAST_ICECUBE::onHit(const NxRaycastHit & hit)
{
	const char* pName = hit.shape->getActor().getName();
	const char* pShape = hit.shape->getName();

	if (strcmp(pName, "Trigger_Water") == 0)
	{
		NxVec3 vPos = hit.worldImpact;
		_vec3 m_vOutPos = _vec3(vPos.x, vPos.y, vPos.z);

		_float fDist = hit.distance;

		m_mapDIst.insert({ fDist, m_vOutPos });
		m_bIsHit = TRUE;
	}

	if (pShape != nullptr && strcmp(pShape, "IceCube_Ladder") == 0)
		return FALSE;

	return m_bIsHit;
}

bool CPhysXMgr::RAYCAST_TIMESTOP::onHit(const NxRaycastHit & hit)
{
	const char* pName = hit.shape->getActor().getName();
	const char* pTime = hit.shape->getName();

	if (pTime == nullptr)
		return FALSE;

	if (strcmp(pTime, "TimeStop") == 0)
	{
		NxVec3 vPos = hit.worldImpact;
		_vec3 m_vOutPos = _vec3(0.f, 0.f, 0.f);

		_float fDist = hit.distance;

		m_mapDIst.insert({ fDist, m_vOutPos });

		NxActor& pActor = hit.shape->getActor();

		CGameObject* pObj = (CGameObject*)pActor.userData;

		if (pObj == nullptr)
			return FALSE;

		pObj->m_bTimeStop = TRUE;

		m_bIsHit = TRUE;
		return m_bIsHit;

	}

	return m_bIsHit;
}

bool CPhysXMgr::RAYCAST_LADDER::onHit(const NxRaycastHit & hit)
{
	const char* pName = hit.shape->getActor().getName();

	return true;
}

bool CPhysXMgr::RAYCAST_CAMERA::onHit(const NxRaycastHit & hit)
{
	const char* pName = hit.shape->getActor().getName();


	if (strcmp(pName, "Camera_Eye") != 0 && strcmp(pName, "Player") != 0 && strcmp(pName, "Camera") != 0)
	{
		NxVec3 nxPos = hit.worldImpact;
		_float fDist = hit.distance;
		NxVec3 vNormal = hit.worldNormal;

		if (fDist > 6.5f)
			return FALSE;

		m_mapDIst.insert({ fDist, pName });
		m_mapDIst2.insert({ fDist, nxPos });
		m_bIsHit = TRUE;
		return m_bIsHit;
	}

	return m_bIsHit;
}
