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

class CLadder :	public CGameObject
{
public:
	explicit CLadder(LPDIRECT3DDEVICE9 pDevice, const _tchar* pMeshName);
	explicit CLadder(CLadder& rhs);
	virtual ~CLadder() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	const _vec4& Ladder_Sort(const NxVec3& vPos);
private:
	CNxTransform*			m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	const _tchar*			m_pMeshName = nullptr;
public:
	vector<_vec4>			m_vecLadder;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pMeshName);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};