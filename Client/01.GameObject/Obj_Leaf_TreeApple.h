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

class CObj_Leaf_TreeApple :	public CGameObject
{
public:
	explicit CObj_Leaf_TreeApple(LPDIRECT3DDEVICE9 pDevice);
	explicit CObj_Leaf_TreeApple(const CObj_Leaf_TreeApple& rhs);
	virtual ~CObj_Leaf_TreeApple() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT Create_NxTransform(const _matrix matWorld);
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
private:
	_float					m_fScele = 1.0;
	_bool					m_bDraw = false;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

