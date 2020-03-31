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
}

class CTwnObj_Static_TriMesh :	public CDgnObj
{
public:
	explicit CTwnObj_Static_TriMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CTwnObj_Static_TriMesh(const CTwnObj_Static_TriMesh& rhs);
	virtual ~CTwnObj_Static_TriMesh() = default;

public:
	void	Set_fFrustum(_float fFrustum) { m_fFrustum = fFrustum; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Set_FarMeshLoad(const _tchar* pName);
public:
	HRESULT Set_NxTransform(const _matrix& matWorld);

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	_bool					m_bDraw = false;
	_bool					m_bIsFarMeshRender = false;
	_float					m_fFrustum = 10.f;
private:
	CMesh_Static*			m_pMesh_Far = nullptr;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

