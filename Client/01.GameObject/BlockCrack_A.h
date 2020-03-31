#pragma once

#include "DgnObj.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}
class CBlockCrack_Brk;
class CBlockCrack_A :	public CDgnObj
{
public:
	explicit CBlockCrack_A(LPDIRECT3DDEVICE9 pDevice);
	explicit CBlockCrack_A(const CBlockCrack_A& rhs);
	virtual ~CBlockCrack_A() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*		m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;

private:
	list<CGameObject*>		m_BrkList;

public:
	_float		m_fTime = 0.f;
	_float		m_fFlowTime = 0.f;
	_float		m_fBlinkTime = 0.f;
	_bool		m_bBlinkTime = FALSE;
	_double		m_TimeStop = 0;
	_float		m_fPower = 1.0;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

