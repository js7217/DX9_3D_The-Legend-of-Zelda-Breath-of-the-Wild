#pragma once

#include "GameObject.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}

class CIceCube :	public CGameObject
{
private:
	explicit CIceCube(LPDIRECT3DDEVICE9 pDevice);
	explicit CIceCube(CIceCube& rhs);
	virtual ~CIceCube() = default;

public:	//아이스큐브 생성 확정 함수
	void Set_bDone(_bool bCheck);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
	
	//Shader
	virtual void Render_Shadow(_int iPass = 1);
	virtual void Render_RimLight();

private:
	void Resize_IceMaker(const _double& TimeDelta);
	HRESULT		Move_IceMaker(const _double& TimeDelta);
	HRESULT		Create_IceCube_Ladder(COLL_STATE tState);

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*		m_pTransform = nullptr;
	CNxTransform*		m_pTarget = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CTexture*				m_pTexture = nullptr;
	CTexture*				m_pTextureNoise = nullptr;

private:
	_bool				m_bPreCreate = TRUE;
	_bool				m_bDone = FALSE;

	_float				m_fTime = 0.f;

	_float				m_fLocal = 0.f;
	_float				m_fHeight = 0.f;
	_float				m_fMakeAcc = 0.4f;
	_float				m_fTotalHeigh = 0.f;
	_double			m_LifeTime = 0.0;
	_double			m_TwinkleTime = 0.0;
	
	LPDIRECT3DTEXTURE9	m_pReadyCube = nullptr;

private:
	HRESULT Add_Component(_vec3 vWorldPos);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Set_TargetTransform(_uint pSceneTag, _tchar* pLayerTag);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

