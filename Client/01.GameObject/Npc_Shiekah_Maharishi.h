#pragma once

#include "DgnObj.h"
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

class CNpc_Shiekah_Maharishi : public CDgnObj
{
	

private:
	explicit CNpc_Shiekah_Maharishi(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc_Shiekah_Maharishi(const CNpc_Shiekah_Maharishi& rhs);
	virtual ~CNpc_Shiekah_Maharishi() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	VOID Set_Animation(_uint iAniState);

private:

private:
	//Component
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;

private:
	//Manager
	CDebug_Console*		m_pDebug = nullptr;
	CPhysXMgr*				m_pPhysx = nullptr;

private:
	//Animate State
	_uint m_iCurAniState = 0;
	_uint m_iPreAniState = 0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Create_Capsule_Coll(NxVec3 nxPos);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

