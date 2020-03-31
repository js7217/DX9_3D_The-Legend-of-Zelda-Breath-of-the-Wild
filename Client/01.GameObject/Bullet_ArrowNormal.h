#pragma once

#include "DgnObj.h"
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

class CTrail;

class CBullet_ArrowNormal :	public CDgnObj
{
public:
	explicit CBullet_ArrowNormal(LPDIRECT3DDEVICE9 pDevice);
	explicit CBullet_ArrowNormal(const CBullet_ArrowNormal& rhs);
	virtual ~CBullet_ArrowNormal() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
	void			Create_ArrowHit();

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*		m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	CTrail*						m_pTrail = nullptr;
	CEffectManager*	m_pEffect = nullptr;

private:
	_matrix						m_matWorld;
	_bool							m_bIsDead = false;
	NxShape*					m_pTriggerShape = nullptr;
	
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

