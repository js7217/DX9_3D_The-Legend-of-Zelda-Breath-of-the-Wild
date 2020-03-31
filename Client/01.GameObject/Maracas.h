#pragma once

#include "GameObject.h"
#include "NxTransform.h"
#include "Debug_Console.h"
#include "Management.h"

namespace ENGINE
{
	class CKeyMgr;
	class CShader;
	class CTexture;
	class CRenderer;
	class CMesh_Dynamic;
}

class CMaracas : public CGameObject
{
	

private:
	explicit CMaracas(LPDIRECT3DDEVICE9 pDevice);
	explicit CMaracas(const CMaracas& rhs);
	virtual ~CMaracas() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:

private:
	//Component
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustum = nullptr;

private:
	//Manager
	CDebug_Console*		m_pDebug = nullptr;
	CPhysXMgr*			m_pPhysx = nullptr;

private:
	// Weapon Holster
	WEAPONHOLSTER		m_tWeaponHolsterInfo;
	_vec3				m_vDropPos;
	_bool				m_bDraw;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Create_Capsule_Coll(NxVec3 nxPos);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

