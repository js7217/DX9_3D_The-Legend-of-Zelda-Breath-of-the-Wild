#pragma once

#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
	class CFrustum;
	class CGameObject;
}

class CObj_TreeApple :	public CGameObject
{
public:
	explicit CObj_TreeApple(LPDIRECT3DDEVICE9 pDevice);
	explicit CObj_TreeApple(const CObj_TreeApple& rhs);
	virtual ~CObj_TreeApple() = default;

public:
	void Set_Velocity(NxVec3 vVelocity) { m_vVelocity = vVelocity; }
	void Set_Grab(CGameObject* pTarget) { m_pTarget_Grab = pTarget; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh_Tree = nullptr;
	CMesh_Static*			m_pMesh_Leafs = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
private:
	_bool					m_bIsAttack = false;
	_bool					m_bDraw = false;
	CGameObject*			m_pTarget_Grab = nullptr;
	NxVec3					m_vVelocity;
private:
	CGameObject*			m_pStump = nullptr;
	CGameObject*			m_pTrunk = nullptr;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

