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

class CBoxShutter_Plane :	public CDgnObj
{
public:
	explicit CBoxShutter_Plane(LPDIRECT3DDEVICE9 pDevice);
	explicit CBoxShutter_Plane(const CBoxShutter_Plane& rhs);
	virtual ~CBoxShutter_Plane() = default;

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
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

