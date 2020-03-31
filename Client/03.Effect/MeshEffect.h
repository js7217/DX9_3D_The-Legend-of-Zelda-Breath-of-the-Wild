#pragma once

#include "Effect.h"

BEGIN(ENGINE)
class CMesh_Static;
END

class CMeshEffect :
	public CEffect
{
public:
	explicit CMeshEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMeshEffect(const CMeshEffect& rhs);
	virtual ~CMeshEffect() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	HRESULT Ready_GameObject(_matrix* pArg);
	virtual _int Update_GameObject(const _double& fTimeDelta);
	virtual _int LateUpdate_GameObject(const _double& fTimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CMesh_Static*	m_pMesh = nullptr;
	_bool			m_bOnce = FALSE;
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMeshEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr);
	virtual void Free();
};

