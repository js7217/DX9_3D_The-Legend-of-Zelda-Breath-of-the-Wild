#ifndef PHYSXMGR_H__
#define PHYSXMGR_H__

#include "Base.h"

BEGIN(ENGINE)

class CGameObject;
class ENGINE_DLL CPhysXMgr : public CBase
{
public:
	enum PHYSX_SCENE
	{
		STAGE_TEST, STAGE_FIELD, STAGE_00, STAGE_01, STAGE_02,
		STAGE_03, STAGE_04, STAGE_05, STAGE_END
	};

public:
	typedef struct _DEBUG_RENDER_VERTEX
	{
		_vec3 vPos;
		DWORD color;
	}DEBUG_RENDER_VERTEX;

	typedef struct _USERDATA
	{
		NxU32 ContactPairFlag;
		CGameObject* pObject;
	} USERDATA;

	class ContactCallBack :public NxUserContactReport
	{
		void onContactNotify(NxContactPair& pair, NxU32 events);
	};

	class TriggerCallBack : public NxUserTriggerReport
	{

	public:
		void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
	public:
		map<NxReal, CGameObject*> m_DistMap;
		_bool			m_bIsTouch_Create_SeekerSkill;					// 던전 내부 터치 터미널  UI 표시.
		CGameObject*	m_pEntranceElevator = nullptr;					// 던전 입구 엘레베이터.
		CGameObject*	m_pDgnShutter = nullptr;				// 던전 입구를 열기 위함.
		NxVec3			m_vTerminalPos = { 0.f, 0.f, 0.f };		// 던전 입구 터치터미널 위치 잡아주기.
		_vec4			m_vLadder;
	};

	class RAYCAST_ICECUBE : public NxUserRaycastReport
	{
	public:
		_vec3 Get_Pos() { return m_mapDIst.begin()->second; }
		_bool Get_IsHit() { return m_bIsHit; }
		virtual bool onHit(const NxRaycastHit& hit);
	private:
		_bool m_bIsHit = FALSE;
		map<_float, _vec3> m_mapDIst;
	};

	class RAYCAST_TIMESTOP : public NxUserRaycastReport
	{
	public:
		_vec3 Get_Pos() { return m_mapDIst.begin()->second; }
		_bool Get_IsHit() { return m_bIsHit; }
		virtual bool onHit(const NxRaycastHit& hit);
	private:
		_bool m_bIsHit = FALSE;
		map<_float, _vec3> m_mapDIst;
	};

	class RAYCAST_LADDER : public NxUserRaycastReport
	{
	public:
		virtual bool onHit(const NxRaycastHit& hit);
	};

	class RAYCAST_CAMERA : public NxUserRaycastReport
	{
	public:
		map<_float, const char*>& Get_MapDist() { return m_mapDIst; }
		map<_float, NxVec3>& Get_MapDist2() { return m_mapDIst2; }
		//_vec3 Get_Pos() { return m_mapDIst.begin()->second; }
		_bool Get_IsHit() { return m_bIsHit; }
		virtual bool onHit(const NxRaycastHit& hit);
	private:
		_bool m_bIsHit = FALSE;
		map<_float, const char*> m_mapDIst;
		map<_float, NxVec3> m_mapDIst2;
	};

	DECLARE_SINGLETON(CPhysXMgr)

private:
	explicit CPhysXMgr();
	virtual ~CPhysXMgr() = default;

public:
	NxScene*			Get_NxScene(_uint iType = 0);	//Device 역할
	NxPhysicsSDK*		Get_NxPhysicsSDK() { return m_pNxPhysicsSDK; }	//SDK 역할
public:
	void Set_Magnet(_bool bCheck) { m_bMagnet = bCheck; }
	void Set_Ladder(_bool bCheck) { m_bLadder = bCheck; }
	void Set_vecLadder(NxVec3 vPos) { m_NxLadder = vPos; }
	void Set_NxMatLadder(NxMat34 Nxmat) { m_NxMatrix = Nxmat; }
	void Set_SceneNum(_int iNum) { m_iSceneNum = iNum; }
	void Set_SceneChangeNum(_int iNum) { m_iChangeNum = iNum; }

	_bool Get_Magnet() { return m_bMagnet; }
	_bool Get_Ladder() { return m_bLadder; }
	NxVec3 Get_vecLadder() { return m_NxLadder; }
	NxMat34 Get_NxMatLadder() { return m_NxMatrix; }
	_int	Get_SceneNum() { return m_iSceneNum; }
	_int	Get_SceneChangeNum() { return m_iChangeNum; }

public:
	HRESULT SetUp_NxPhysX(LPDIRECT3DDEVICE9 pDevice);
	HRESULT SetUp_NxScene(PHYSX_SCENE eScene);
	HRESULT Create_PhysxPlane();
	void			Update_PhysxManager(const _double & TimeDelta);

	HRESULT Render_PhysxCollider();
	HRESULT Render_Buffer(const D3DPRIMITIVETYPE Type, const int VertexCount);
	void Render_PhysxEnd();

public:
	HRESULT Set_ContactActor(const char* Actor1, const char* Actor2);
	_bool		NxPicking_IceCube(HWND hWnd, _vec3* pOutPos);
	_bool		NxPicking_TimeStop(HWND hWnd, _vec3* pOutPos);
	_bool		NxPicking_Ladder(const NxVec3& vOrig, const NxVec3& vDir);
	_bool		RayCast_ClosestShapes(_vec3 * pOutPos, CAMERA_DESC pDesc);
	_bool		RayCast_RefelectClosest(_vec3* pFireBallPos, _vec3* pFireBallDir, _vec3* pOutPos, _vec3* pOutNormal);
	_bool		RayCast_FowardObjectNormal(_vec3 * pPlayerPos, _vec3 * pPlayerDir, _vec3 * pOutPos, _vec3 * pOutNormal);
	_bool		RayCast_ClosestShapes_Back(CAMERA_DESC pDesc);
	_bool		RayCast_ClosestTerrain(_vec3 * pOutPos, _vec3 vPos);
	_bool		Raycast_ClosestIceCube(HWND hWnd, _vec3* pOutPos);

public:
	LPDIRECT3DDEVICE9 m_pDevice;

private:
	NxPhysicsSDK*		m_pNxPhysicsSDK;
	NxScene*					m_pNxScene[STAGE_END];	//== m_pDevice 역할
	ContactCallBack*		m_pCallBack = nullptr;
	TriggerCallBack*		m_pTrigger = nullptr;
	//SL_TEMP
	USERDATA				m_UserDataPlane;
	USERDATA				m_UserDataMesh;

public:
	//Physx Plane
	NxActor*					m_pActorPlane = nullptr;
	NxActor*					m_pActorMesh = nullptr;

	//Physx Material
	NxMaterial*				defaultMaterial = nullptr;

	//Physx SceneDesc
	NxSceneDesc m_NxSceneDesc;

	//Render_Collider
	DEBUG_RENDER_VERTEX*	m_DebugRenderVertex = nullptr;
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;		//Line Color

private:
	_bool m_bLadder = FALSE;
	_bool m_bMagnet = FALSE;
	NxVec3 m_NxLadder;
	NxMat34 m_NxMatrix;
	_bool m_bIsLadder = FALSE;
	_int	m_iSceneNum = 99;				// SceneChange 를 위한 넘버링.
	_int	m_iChangeNum = 99;				// SceneChange 를 위한 넘버링.

public:
	PHYSX_SCENE m_eScene = STAGE_END;

private:
	map<const _tchar*, list<NxActor*>> m_MapActor[STAGE_END];
	typedef map<const _tchar*, list<NxActor*>> ACTORMAP;

//public:
//	HRESULT Insert_MapActor(const _tchar* szName, NxActor* pActor);
//	list<NxActor*>& Get_ActorList(const _tchar* szName);
//	NxActor* Find_MapActor(const _tchar* szName, _uint iIndex = 0);

public:
	virtual void Free(void);
};
END
#endif
