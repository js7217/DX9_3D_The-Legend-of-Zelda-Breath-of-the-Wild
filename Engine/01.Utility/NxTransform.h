#pragma once

#include "Component.h"
#include "Stream.h"
#include "PhysXMgr.h"

//Physx Collider + Transform
class ENGINE_DLL CNxTransform :	public CComponent
{
public:
	enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

private:
	explicit CNxTransform(LPDIRECT3DDEVICE9 pDevice);
	explicit CNxTransform(CNxTransform& rhs);
	virtual ~CNxTransform() = default;

public:
	void Set_LimitVelocity(_float fLimit) { m_fLimit = fLimit; }
	void Set_Mesh(LPD3DXMESH pMesh) { m_pMesh = pMesh; }
	void Set_AimMode(_bool bCheck) { m_bAimMode = bCheck; }
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	void	Set_ParentMatrix(const _matrix* pParentMatrix) {m_matWorld *= *pParentMatrix;}
	void	Set_WeaponMatrix(const _matrix* pParentMatrix) { m_matWeaponWorld *= *pParentMatrix; }
	void	Set_NxParentMatrix(NxMat34* pParentMatrix) { m_pActor->setGlobalPose(*pParentMatrix); }
	void	Set_Position(const _vec3* pPos) { m_vInfo[INFO_POS] = *pPos; }
	void	Set_Pos_Direction(const _vec3* const pDir) { m_vInfo[INFO_POS] += *pDir; }
	void	Set_NxPosition(const NxVec3* pPos) { m_pActor->setGlobalPosition(*pPos); }
	void	Set_NxPos_Direction(NxVec3* pDir)
	{
		m_vSpeed = *pDir;
		NxVec3 NxTemp = m_pActor->getGlobalPosition() + *pDir;
		m_pActor->setGlobalPosition(NxTemp);
	}
	void	Set_Scale(const _vec3* const pScale)
	{
		m_vScale.x = pScale->x;
		m_vScale.y = pScale->y;
		m_vScale.z = pScale->z;
	}
	void Set_Grab(_bool bCheck) { m_bGrab = bCheck; }
	void Set_Bomb(_bool bCheck) { m_bBomb = bCheck; }
	void Set_BombPosition(_vec3& vPos) {m_vBombPos = vPos;}
	void Set_LinearVelocity(NxVec3 nxDir, _float fSpeed = 1.f) { m_pActor->setLinearVelocity(nxDir * fSpeed);	 }

	//void Set_TargetMatrix(const _matrix* pTargetMatrix) { m_matWorld *= *pTargetMatrix; }

public:
	NxActor*		Get_Actor() { return m_pActor; }
	NxShape*		Get_Shape(_int i = 0) { return m_pActor->getShapes()[i]; }
	NxActor*		Get_SubActor(_uint idx = 0) { return m_pSubActor[idx]; }
	NxVec3			Get_GlobalPos() { return m_pActor->getGlobalPosition(); }
	_bool			Get_BombState() { return m_bBomb; }
	_bool			Get_Grab() { return m_bGrab; }
	_vec3&			Get_BombPosistion() { return m_vBombPos; }
	_bool&			Get_BombGrabState() { return m_bGrab; }
	NxVec3			Get_NxPos_Direction() { return m_vSpeed; }
	void		Get_Info(INFO eType, _vec3* pInfo)
	{		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));	}

	//const _vec3*	Get_Info(INFO eType)
	//{		memcpy(&m_vInfo[eType], &m_matWorld.m[eType][0], sizeof(_vec3));		return &m_vInfo[eType];	}

	const _vec3*	Get_Info(INFO eType)
	{		return &m_vInfo[eType];	}

	const _vec3		Get_vLook()
	{
		_vec3 vNewDir = m_vInfo[INFO_LOOK];
		vNewDir.y = 0.f;
		return *D3DXVec3Normalize(&vNewDir, &vNewDir);
	}

	_float Calc_TargetDistance(CNxTransform* pTrans);
	_vec3 Calc_TargetDirection(CNxTransform* pTrans);
	_vec3 Calc_CameraPos(_float fCamDist, _vec3 vTargetPos, _float fHeigth);
	_vec3 Calc_CameraDir(_float fCamDist, _vec3 vTargetPos);

public:
	void	Get_WorldMatrix(_matrix* pWorlMatrix) const { *pWorlMatrix = m_matWorld; }
	const _matrix*	Get_WorldMatrixPointer(void) const { return &m_matWorld; }
	void	Rotation(ROTATION eType, const _float& fAngle)
	{		*(((_float*)&m_vAngle) + eType) += fAngle;	}
	void	FixRotation(ROTATION eType, const _float& fAngle)
	{		*(((_float*)&m_vAngle) + eType) = fAngle;	}
	void	Move_TargetPos(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	void Grab_Advanced(CNxTransform* pTrans, _float fAngle, _float fHeight = 0.4f, _bool bCheck = FALSE);
	void Grab_Item(CNxTransform* pTrans, _float fHeight = 1.75f);
	void Grab_Chicken(CNxTransform* pTrans, _float fAngle, _float fHeight = 1.75f);
	void NxRotation_Y(CNxTransform* pTrans);

public:
	HRESULT Create_BombActor();
	HRESULT Create_LadderActor(_float fHeight);
	HRESULT Create_LadderActor_Up();
	HRESULT Create_LadderActor_Down();
	HRESULT Create_Door_Right();
	HRESULT Create_Door_Left();
	HRESULT Create_GrabActor();
	HRESULT Create_CameraActor();
	HRESULT Create_DgnEntranceTrigger();
	HRESULT Create_SubActor(NxVec3 vGlobalPos, _uint iNum);
	HRESULT	Create_SubShape_Trigger_Box(NxVec3 vLocalPos);
	HRESULT Create_SubShape_Trigger_Sphere(NxVec3 vLocalPos);
	HRESULT Create_SubShape_Trigger_ClimbSphere(NxVec3 vLocalPos);
	NxTriangleMeshShapeDesc Create_NxMeshDesc();
public:
	HRESULT		Ready_Component(void);
	HRESULT		Ready_Status(void* pArg = nullptr);
	virtual		_int Update_Component(const _double & TimeDelta);
	virtual		 _int LateUpdate_Component(const _double & TimeDelta);

private:
	//Create Collider
	HRESULT Set_BodyDesc(NxActorDesc* pActorDesc);
	HRESULT Create_Capsule_Coll();
	HRESULT Create_Box_Coll();
	HRESULT Create_Sphere_Coll();
	HRESULT Create_TriangleMesh();
	NxActor* CreateConvexMesh();
	void Limit_Velocity();
	
	
private:
	COLL_STATE						m_tState;
	CNxTransform*					m_pTarget;

public:
	_vec3		m_vInfo[INFO_END];
	//NxVec3z	m_NxInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	NxVec3		m_vSpeed;

	_matrix		m_matWorld;	//Trasnform World
	_matrix		m_matWeaponWorld;	//Trasnform World
	NxMat34		m_NxWorld;	//Physx World 3334

	_matrix		m_matNRotWorld;
	NxMat34		m_NxNRotWorld;
	_float			m_fLimit = 14.f;

public:
	//Physx 물리 정보가 저장된 변수
	NxActor*					m_pActor = nullptr;
	NxActor*					m_pSubActor[2] = { nullptr, nullptr };
	LPD3DXMESH				m_pMesh;

public:
	//SkillState
	_uint m_SkillState = 0;
	_bool m_bBomb = FALSE;
	_bool m_bGrab = FALSE;	//아이템을 실제로 집은 상태
	_bool m_bAimMode = FALSE;
	_bool m_bSpellMode = FALSE;
	_vec3 m_vBombPos; 
	_vec3 m_vIceCubePos;

public:
	const _matrix*			pTargetHandMatrix;
	const _matrix*			pTargetLeftHandMatrix;
	
public:
	static CNxTransform* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent*	Clone_Component(void* pArg /* = nullptr */);
	virtual void Free();

};

