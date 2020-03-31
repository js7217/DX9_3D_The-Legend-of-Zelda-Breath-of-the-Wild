#pragma once

#include "GameObject.h"
#include "NxTransform.h"
#include "Mesh_Static.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}

class CDungeon :	public CGameObject
{
private:
	explicit CDungeon(LPDIRECT3DDEVICE9 pDevice);
	explicit CDungeon(CDungeon& rhs);
	virtual ~CDungeon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual void Render_Shadow(_int iPass = 1);
	virtual void Render_RimLight();

private:
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CNxTransform*		m_pTransform = nullptr;
	CMesh_Static*		m_pMesh = nullptr;
	_float				m_fTime = 0.f;
	_float				m_fFlowTime = 0.f;
	_bool				m_bMagnet = false;
	_bool				m_bTimestop = false;
	_vec3				m_vPlayerPos;
	_float				m_fDist;

private:
	COLL_STATE				m_tState;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

