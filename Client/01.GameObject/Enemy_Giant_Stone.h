#pragma once

#include "NxPlayer.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
	class CMesh_Dynamic;
	class CGameObject;
}

class CEnemy_Giant_Stone :	public CGameObject
{
public:
	explicit CEnemy_Giant_Stone(LPDIRECT3DDEVICE9 pDevice);
	explicit CEnemy_Giant_Stone(const CEnemy_Giant_Stone& rhs);
	virtual ~CEnemy_Giant_Stone() = default;

public:
	void	Set_Enemy_Giant_Mesh(CMesh_Dynamic* pMesh) { m_pEnmey_Giant_Mesh = pMesh; }
	void	Set_GrabObject(CGameObject* pObj) { m_pGrabObject = pObj; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Move_Target();
	_bool	Check_Dead();

private:
	void TimeStop_Sound(_double CurTime);
	_double m_SoundTime = 0.0;

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
private:
	CNxPlayer*			m_pPlayer = nullptr;
	CNxTransform*		m_pTarget = nullptr;

public:
	_float		m_fTime = 0.f;
	_float		m_fFlowTime = 0.f;
	_float		m_fBlinkTime = 0.f;
	_bool		m_bBlinkTime = FALSE;
	_bool				m_bIsCheckDead = false;
	_double				m_TimeStop  = 0;
	_float				m_fPower = 1.0;
	_float				m_fHandUp = 60.f;
	CMesh_Dynamic*		m_pEnmey_Giant_Mesh = nullptr;
	CGameObject*		m_pGrabObject = nullptr;
	NxVec3				m_vRHandPos;
	NxVec3				m_vTargetPos;
	_float				m_fDist;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Set_TargetTransform(_uint pSceneTag, _tchar* pLayerTag);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

