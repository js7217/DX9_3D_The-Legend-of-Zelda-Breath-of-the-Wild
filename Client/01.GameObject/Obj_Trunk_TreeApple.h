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
	class CPhysXMgr;
}

class CObj_Trunk_TreeApple : public CGameObject
{
public:
	explicit CObj_Trunk_TreeApple(LPDIRECT3DDEVICE9 pDevice);
	explicit CObj_Trunk_TreeApple(const CObj_Trunk_TreeApple& rhs);
	virtual ~CObj_Trunk_TreeApple() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT		Create_NxTransform(NxVec3 vVelocity);
	void		Set_Target_Grab(CGameObject* pTarget) { m_pTarget_Grab = pTarget; }
	void		Check_CreateLog();
private:
	CPhysXMgr*				m_pPhysx = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh_Tree = nullptr;
	CMesh_Static*			m_pMesh_Leafs = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
private:
	_bool					m_bIsCreate_Log = false;

private:
	CGameObject*			m_pLog = nullptr;
	list<CGameObject*>		m_LeafList;
	_matrix					m_matWorld;
	CGameObject*			m_pTarget_Grab = nullptr;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

