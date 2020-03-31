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

class CDgnObj_FenceDynamic :	public CDgnObj
{
public:
	explicit CDgnObj_FenceDynamic(LPDIRECT3DDEVICE9 pDevice);
	explicit CDgnObj_FenceDynamic(const CDgnObj_FenceDynamic& rhs);
	virtual ~CDgnObj_FenceDynamic() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

