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

class CCapShutter :	public CDgnObj
{
public:
	explicit CCapShutter(LPDIRECT3DDEVICE9 pDevice);
	explicit CCapShutter(const CCapShutter& rhs);
	virtual ~CCapShutter() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void		Move_CapShutter(const _double& TimeDelta);

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*		m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	
public:
	_bool		m_bTimeStop = FALSE;
	_double m_TimeStop  = 0;
	NxVec3 m_NxVelocity;
	_float   m_fPower = 1.0;
	_double m_RotAngle = 0.0;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();

};

