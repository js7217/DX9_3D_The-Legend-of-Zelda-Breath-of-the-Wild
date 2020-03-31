#pragma once

#include "GameObject.h"
#include "NxTransform.h"

class CTriger_Water : public CGameObject
{
private:
	explicit CTriger_Water(LPDIRECT3DDEVICE9 pDevice);
	explicit CTriger_Water(CTriger_Water& rhs);
	virtual ~CTriger_Water() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual void Render_RimLight();
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CTexture*				m_pTexture = nullptr;
	CTexture*				m_pTextureNoise = nullptr;
	_float					m_fTime = 0.f;

	// AlphaTest
	_float					m_fTest = 0.1f;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

